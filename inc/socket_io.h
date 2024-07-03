#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <queue>
#include <memory>
#include <tuple>
#include <chrono>

#include "log.h"
#include "socket.h"

class SocketIO
{
private:
    std::shared_ptr<Log> log; // 日志
    UDP_SOCKET socket; // 套接字
    std::jthread read_thread; // 读线程
    std::jthread write_thread; // 写线程
    std::atomic<bool> running{true}; // 运行标志
    std::queue<std::tuple<std::string, int, std::string>> write_queue; // 写队列
    std::mutex write_queue_mutex; // 写队列锁
    std::condition_variable write_queue_cv; // 写队列条件变量
    std::function<void(std::string, int, std::string)> read_callback; // 读回调

    void do_read(); // 读
    void do_write(); // 写
public:
    SocketIO(std::shared_ptr<Log> log, std::string ip, int port, std::function<void(std::string, int, std::string)> read_callback); // 构造函数
    ~SocketIO(); // 析构函数

    void write(std::string ip, int port, std::string data); // 写
    void run(); // 运行
    void stop(); // 停止

    bool is_running() const { return running.load(); } // 是否运行
};