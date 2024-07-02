#pragma once
#ifndef DNS_SERVER_CACHE_H
#define DNS_SERVER_CACHE_H

#include <map>
#include <chrono>
#include <string>
#include <list>
#include <shared_mutex>
#include <vector>

#include "log.h"
#include "dns.h"

class CacheValue
{
private:
    DNSQuery query; // 查询
    DNS dns; // DNS

    std::chrono::time_point<std::chrono::steady_clock> insert_time; // 插入时间
public:
    const DNSQuery &get_query(); // 获取查询
    const DNS &get_dns(); // 获取 DNS
    const std::chrono::time_point<std::chrono::steady_clock> &get_insert_time(); // 获取插入时间

    CacheValue(const DNSQuery &query, const DNS &dns); // 构造函数
};

class DNSCache
{
private:
    Log &log; // 日志
    std::map<DNSQuery, std::list<CacheValue>::iterator> cache; // 缓存
    std::list<CacheValue> cache_list; // 缓存列表
    std::shared_mutex mutex; // 互斥锁

    size_t max_size; // 最大大小
public:
    DNSCache(Log &log, size_t max_size = 1000); // 构造函数

    void add(const DNSQuery &query, const DNS &dns); // 添加
    bool exist(const DNSQuery &query); // 是否存在
    DNS get(const DNSQuery &query); // 获取
    void remove(const DNSQuery &query); // 移除
    void clear(); // 清空
    void clean(); // 清理
};

#endif // DNS_SERVER_CACHE_H