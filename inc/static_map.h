#pragma once
#ifndef DNS_SERVER_STATIC_MAP_H
#define DNS_SERVER_STATIC_MAP_H

#include <unordered_map>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <shared_mutex>
#include <condition_variable>
#include <mutex>
#include <fstream>
#include <atomic>

#include "log.h"
#include "dns.h"
#include "file.hpp"

class StaticIPMap
{
private:
    std::shared_ptr<Log> log; // 日志
    std::unordered_map<std::string, std::vector<IP>> IPmap; // IP 映射
    std::shared_mutex mutex; // 互斥锁
    FilePath file_path; // 文件路径
    std::jthread watch_thread; // 监视线程
    std::atomic<bool> running{true}; // 运行标志

    std::condition_variable cv; // 条件变量
    std::mutex cv_mutex; // 条件变量锁

    void watch_file(); // 监视文件
    void load(); // 加载
    void file_exist(); // 文件是否存在
public:
    StaticIPMap(std::shared_ptr<Log> log, FilePath file_path); // 构造函数
    ~StaticIPMap(); // 析构函数

    std::vector<IP> get(const std::string &domain_name); // 获取 IP
    void stop(); // 停止
};

#endif