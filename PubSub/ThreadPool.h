#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace PubSub
{
    class ThreadPool
    {
    public:
        ThreadPool(unsigned size);
        ~ThreadPool();

        void Enqueque(const std::function<void()> task);

    private:
        std::vector<std::thread> m_workers;
        std::queue<std::function<void()>> m_tasks;

        std::condition_variable m_condition;
        std::mutex m_quequeMutex;
        bool m_stop;
    };
}