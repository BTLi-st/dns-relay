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
int main()
{
    // Log log(FilePath().add_dir("log").add_file("server.log"), DBG);
    auto log = std::make_shared<Log>(FilePath().add_dir("log").add_file("server.log"), DBG);
    DNSRelayServer server(log, "0.0.0.0", 53, FilePath().add_dir("data").add_file("ip_map.txt"), 10);
    server.set_server("8.8.8.8", 53);
    server.run();
    std::this_thread::sleep_for(std::chrono::seconds(20));
    server.stop();
    return 0;
}