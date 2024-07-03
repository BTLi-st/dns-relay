#include "thread_pool.h"

ThreadPool::ThreadPool(size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        jthreads.emplace_back([this]() {
            while (true)
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(mutex);
                    cv.wait(lock, [this]() { return !running || !tasks.empty(); });
                    if (!running && tasks.empty())
                        return;
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    if (running.load())
        stop();
}

void ThreadPool::stop()
{
    running = false;
    cv.notify_all();
    for (auto &jthread : jthreads)
        jthread.join();
}