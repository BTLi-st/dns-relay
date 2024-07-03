#include <thread>
#include <chrono>
#include <atomic>
#include "log.h"
#include "file.hpp"
#include "static_map.h"
#include "socket.h"
#include "thread_pool.h"
#include "server.h"
#include "dns.h"
int main(int argc, char* argv[])
{
    Level log_level = INFO;
    IP server_ip;
    server_ip.set_ip("10.3.9.4");
    if (argc > 1)
    {
        int tmp = std::stoi(argv[1]);
        if (tmp >= 0 && tmp <= 5)
            log_level = static_cast<Level>(tmp);
        else
            std::cout << "Invalid log level, using INFO" << std::endl,
            log_level = INFO;
    }
    if (argc > 2)
    {
        if(!server_ip.set_ip(argv[2]))
            std::cout << "Invalid server ip, using 10.3.9.4" << std::endl,
            server_ip.set_ip("10.3.9.4");
    }
    if (argc > 3)
    {
        std::cout << "Usage: " << argv[0] << " [log_level] [server_ip]" << std::endl;
        return 0;
    }
    auto log = std::make_shared<Log>(FilePath().add_dir("log").add_file("server.log"), log_level);
    DNSRelayServer server(log, "0.0.0.0", 53, FilePath().add_dir("data").add_file("ip_map.txt"), 10);
    server.set_server(server_ip.get_ip(), 53);
    server.run();
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }
    server.stop();
    return 0;
}