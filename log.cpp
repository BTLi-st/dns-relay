#include "log.h"

std::string Log::get_time()
{
    using namespace std::chrono;
    auto now = system_clock::now(); // 获取当前时间
    auto local_now = zoned_time{current_zone(), floor<seconds>(now)}; // 获取本地时间
    return std::format("[{:%Y-%m-%d %H:%M:%S}]", local_now.get_local_time()); // 格式化时间
}

void Log::to_queue(std::string console, std::string file)
{
    std::lock_guard<std::mutex> lock(queue_mutex);
    log_queue.push(std::make_pair(console, file));
    cv.notify_one(); // 通知线程
}

void Log::process_log()
{
    std::unique_lock<std::mutex> lock(queue_mutex); // 上锁
    do
    {
        cv.wait(lock, [this] { return !log_queue.empty() || stop; }); // 等待通知
        while (!log_queue.empty())
        {
            if (write_to_console)
            {
                std::cerr << log_queue.front().first << std::endl; // 写入控制台
            }
            if (write_to_file)
            {
                file << log_queue.front().second << std::endl; // 写入文件
            }
            log_queue.pop(); // 弹出队列
        }
    } while (!stop);
}

Log::Log(Level level, bool write_to_console)
{
    write_to_file = false;
    this->write_to_console = write_to_console;
#if defined(DEBUG)
    total_level = DEBUG;
#else
    total_level = level;
#endif
    log_thread = std::thread(&Log::process_log, this);
}

Log::Log(FilePath path, Level level, bool write_to_console)
{
    write_to_file = true;
    this->write_to_console = write_to_console;
#if defined(DEBUG)
    total_level = DEBUG;
#else
    total_level = level;
#endif
    file_path = path;
    file.open(file_path(), std::ios::out | std::ios::app);
    log_thread = std::thread(&Log::process_log, this);
}

Log::~Log()
{
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        stop = true;
    }
    cv.notify_one(); // 通知线程退出
    log_thread.join(); // 等待线程退出
    if (write_to_file)
    {
        file.close(); // 关闭文件
    }
}