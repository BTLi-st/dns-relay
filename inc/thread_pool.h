#pragma once
#ifndef DNS_SERVER_THREAD_POOL_H
#define DNS_SERVER_THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool
{
private:
    std::vector<std::thread> jthreads; // 线程
    std::queue<std::function<void()>> tasks; // 任务
    std::mutex mutex; // 互斥锁
    std::condition_variable cv; // 条件变量
    std::atomic<bool> running{true}; // 运行标志
public:
    ThreadPool(size_t size = 4); // 构造函数
    ~ThreadPool(); // 析构函数

    template <class F, class... Args>
    void add_task(F &&f, Args &&...args); // 添加任务
    void stop(); // 停止
};

template <class F, class... Args>
void ThreadPool::add_task(F &&f, Args &&...args)
{
    auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (!running)
            throw std::runtime_error("ThreadPool is stopped");
        tasks.emplace(task);
    }
    cv.notify_one();
}

#endif