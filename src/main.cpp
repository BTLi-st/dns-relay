#include <thread>
#include <chrono>
#include "log.h"
#include "file.hpp"
#include "static_map.h"
#include "socket.h"
#include "dns.h"

int main()
{
    Log log(FilePath().add_dir("log").add_file("server.log"), DBG);
    // UDP_SOCKET socket(log);
    // socket.bind("0.0.0.0", 53);
    // while (true)
    // {
    //     std::string ip;
    //     int port;
    //     std::string client_ip;
    //     int client_port;
    //     DNS dns_from_client(log);
    //     std::string data;
    //     int buffer_size = 4096;
    //     data.resize(buffer_size);
    //     int size = socket.recvfrom(ip, port, data.data(), buffer_size);
    //     if (size == -1)
    //     {
    //         log.error("Recvfrom failed.");
    //         continue;
    //     }
    //     if (size == 0)
    //     {
    //         log.fatal("Connection closed.");
    //         break;
    //     }
    //     data.resize(size);
    //     log.debug("Recvfrom {}:{}", ip, port);
    //     client_ip = ip;
    //     client_port = port;
    //     if (!dns_from_client.deserilize(data))
    //     {
    //         log.error("Deserilize failed.");
    //         continue;
    //     }
    //     log.debug("Deserilize success.");
    //     std::string server_ip = "8.8.8.8";
    //     int server_port = 53;
    //     dns_from_client.serialize(data);
    //     if (data == "")
    //     {
    //         log.error("Serialize failed.");
    //         continue;
    //     }
    //     log.debug("Serialize success.");
    //     if (!socket.sendto(server_ip, server_port, data.data(), data.size()))
    //     {
    //         log.error("Sendto failed.");
    //         continue;
    //     }
    //     log.debug("Sendto {}:{}", server_ip, server_port);
    //     size = socket.recvfrom(ip, port, data.data(), buffer_size);
    //     if (size == -1)
    //     {
    //         log.error("Recvfrom failed.");
    //         continue;
    //     }
    //     if (size == 0)
    //     {
    //         log.fatal("Connection closed.");
    //         break;
    //     }
    //     if (ip != server_ip || port != server_port)
    //     {
    //         log.error("Recvfrom from illegal server.");
    //         continue;
    //     }
    //     data.resize(size);
    //     log.debug("Recvfrom {}:{}", ip, port);
    //     DNS dns_from_server(log);
    //     if (!dns_from_server.deserilize(data))
    //     {
    //         log.error("Deserilize failed.");
    //         continue;
    //     }
    //     log.debug("Deserilize success.");
    //     dns_from_server.serialize(data);
    //     if (data == "")
    //     {
    //         log.error("Serialize failed.");
    //         continue;
    //     }
    //     log.debug("Serialize success.");
    //     if (!socket.sendto(client_ip, client_port, data.data(), data.size()))
    //     {
    //         log.error("Sendto failed.");
    //         continue;
    //     }
    //     log.debug("Sendto {}:{}", client_ip, client_port);
    // }
    // socket.close();
    StaticIPMap map(log, FilePath().add_file("ip_map.txt"));
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        auto ips = map.get("www.baidu.com");
        for (auto &ip : ips)
        {
            log.info("www.baidu.com: {}", ip);
        }
    }
    return 0;
}