#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "TaskQueue.h"

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
        TaskQueue m_tasks;

        std::condition_variable m_condition;
        std::mutex m_conditionMutex;
        std::mutex m_stopMutex;
        bool m_stop;
    };
}