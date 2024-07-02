#pragma once
#ifndef DNS_RELAY_SERVER_H
#define DNS_RELAY_SERVER_H

#include <map>
#include <shared_mutex>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <atomic>

#include "log.h"
#include "socket_io.h"
#include "thread_pool.h"
#include "dns.h"
#include "cache.h"
#include "static_map.h"
#include "id_gen.h"

struct Request
{
    std::string ip; // IP
    int port; // 端口
    std::shared_ptr<DNS> dns; // DNS
};

struct QueryStore
{
    std::shared_ptr<DNS> dns; // DNS
    std::chrono::time_point<std::chrono::steady_clock> insert_time; // 插入时间
    std::string ip; // IP
    int port; // 端口

    QueryStore() = default;
    QueryStore(std::shared_ptr<DNS> dns, std::string ip, int port)
        : dns(dns), ip(ip), port(port), insert_time(std::chrono::steady_clock::now()) {}

    bool is_timeout(std::chrono::seconds timeout) const
    {
        return std::chrono::steady_clock::now() - insert_time > timeout;
    }
};

class DNSRelayServer
{
private:
    Log &log; // 日志
    SocketIO socket_io; // 套接字 IO
    ThreadPool pool; // 线程池
    DNSCache cache; // 缓存
    StaticIPMap map; // 静态 IP 映射
    IDGenerator id_gen; // ID 生成器
    std::map<unsigned short, QueryStore> queries; // 查询列表
    std::shared_mutex queries_mutex; // 查询列表互斥锁

    std::jthread clean_thread; // 清理线程

    std::atomic<bool> running; // 运行状态

    int server_port = 53; // 服务器端口
    std::string server_ip = "8.8.8.8"; // 服务器 IP

    void clean(); // 清理

    void handle_info(std::string ip, int port, std::string data); // 处理信息

    void handle_query(std::string ip, int port, std::shared_ptr<DNS> dns); // 处理查询
    void handle_response(std::string ip, int port, std::shared_ptr<DNS> dns); // 处理响应

    void relay(std::string ip, int port, std::shared_ptr<DNS> dns); // 转发

    void handle_query_A(std::string ip, int port, std::shared_ptr<DNS> dns); // 处理查询 A
    void handle_query_ptr(std::string ip, int port, std::shared_ptr<DNS> dns); // 处理查询 PTR

    void handle_query_other(std::string ip, int port, std::shared_ptr<DNS> dns); // 处理查询其他

    void handle_edns_query(std::string ip, int port, std::shared_ptr<DNS> dns); // 处理 EDNS 查询
public:
    DNSRelayServer(Log &log, std::string ip, int port, FilePath map_file, int pool_size = 5); // 构造函数
    ~DNSRelayServer(); // 析构函数

    void set_server(std::string ip, int port); // 设置服务器

    void run(); // 运行
    void stop(); // 停止
};

#endif // DNS_RELAY_SERVER_H