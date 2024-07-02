#include "static_map.h"

void StaticIPMap::watch_file()
{
    file_exist();
    auto last_write_time = std::filesystem::last_write_time(file_path());
    while (running)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        file_exist();
        auto new_write_time = std::filesystem::last_write_time(file_path());
        if (new_write_time != last_write_time)
        {
            last_write_time = new_write_time;
            load();
        }
    }
}

void StaticIPMap::load()
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    log.debug("Load ip map from file: {}", file_path().string());
    IPmap.clear();
    file_exist();
    std::ifstream file(file_path());
    if (!file.is_open())
    {
        log.error("Open file failed: {}", file_path().string());
        log.info("The ip map is empty.");
        return;
    }
    std::string line;
    int line_number = 0;
    while (std::getline(file, line))
    {
        line_number++;
        std::string ip;
        std::string domain_name;
        std::istringstream iss(line);
        iss >> ip;
        iss >> domain_name;
        if (ip == "" || domain_name == "")
            if (ip == "" && domain_name == "")
            {
                log.debug("Empty line on line {}", line_number);
                continue;
            }
            else
            {
                log.error("Invalid line on line {}", line_number);
                continue;
            }
        IP ip_obj;
        if (!ip_obj.set_ip(ip))
        {
            log.error("Invalid ip on line {}", line_number);
            continue;
        }
        if (IPmap.find(domain_name) == IPmap.end())
            IPmap[domain_name] = {ip_obj};
        else
            IPmap[domain_name].push_back(ip_obj);
    }
    file.close();
    log.info("Load ip map success.");
}

void StaticIPMap::file_exist()
{
    if (std::filesystem::exists(file_path()) == false)
    {
        log.error("File not exists: {}", file_path().string());
        std::ofstream new_file(file_path());
        new_file.close();
        log.info("Created empty ip map file: {}", file_path().string());
    }
}

StaticIPMap::StaticIPMap(Log &log, FilePath file_path) : log(log), file_path(file_path)
{
    load();
    watch_thread = std::jthread(&StaticIPMap::watch_file, this);
}

StaticIPMap::~StaticIPMap()
{
    running = false;
}

std::vector<IP> StaticIPMap::get(const std::string &domain_name)
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    if (IPmap.find(domain_name) == IPmap.end())
    {
        log.debug("Domain name not found: {}", domain_name);
        return {};
    }
    return IPmap[domain_name];
}
