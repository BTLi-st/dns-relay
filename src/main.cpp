#include <thread>
#include <chrono>
#include <atomic>
#include "log.h"
#include "file.hpp"
#include "static_map.h"
#include "socket.h"
#include "thread_pool.h"
#include "dns.h"
int main()
{
    Log log(FilePath().add_dir("log").add_file("server.log"), DBG);
    StaticIPMap map(log, FilePath().add_file("ip_map.txt"));
    ThreadPool pool(4);
    for (int i = 0; i < 4; i++)
    {
        pool.add_task([&map, &log] (int i) {
            while (true)
            {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                log.debug("{}", map.get("www.baidu.com").size());
                log.debug("{}", i);
            }
        }, i);
    }
    
    return 0;
}