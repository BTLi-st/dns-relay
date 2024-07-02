#pragma once
#ifndef DNS_RELAY_ID_GEN_H
#define DNS_RELAY_ID_GEN_H

#include <mutex>
#include <map>
#include <random>

class IDGenerator
{
private:
    std::mutex mutex; // 互斥量
    std::map<unsigned short, bool> ids; // ID 列表
    std::random_device rd; // 随机设备
    std::mt19937 gen; // 随机数生成器
    std::uniform_int_distribution<unsigned short> dis; // 均匀分布

public:
    IDGenerator(); // 构造函数

    unsigned short generate(); // 生成 ID
    void release(unsigned short id); // 释放 ID
};

#endif // DNS_RELAY_ID_GEN_H