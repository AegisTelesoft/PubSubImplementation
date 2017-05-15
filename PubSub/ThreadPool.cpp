#include "ThreadPool.h"

#include <iostream>

#include <chrono>

namespace PubSub
{
    ThreadPool::ThreadPool(unsigned threads) : m_stop(false)
    {
        for(unsigned i = 0; i < threads; i++)
        {
            m_workers.emplace_back([this]{
                for(;;)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->m_quequeMutex);
                        this->m_condition.wait(lock, [this] {
                            return this->m_stop || !this->m_tasks.empty();
                        });

                        if (this->m_stop && this->m_tasks.empty())
                            return;

                        task = std::move(this->m_tasks.front());
                        this->m_tasks.pop();
                    }

                    task();
                }
            });
        }
    }

    void ThreadPool::Enqueque(std::function<void()> task)
    {
        std::unique_lock<std::mutex> lock(this->m_quequeMutex);

        if(!m_stop)
        {
            this->m_tasks.emplace(task);
            m_condition.notify_one();
        }
        lock.unlock();
    }

    ThreadPool::~ThreadPool()
    {
        std::unique_lock<std::mutex> lock(this->m_quequeMutex);
        m_stop = true;
        lock.unlock();

        std::cout << "cyaaaaaaaaaa" << std::endl;
        m_condition.notify_all();

        for(int i = 0; i < m_workers.size(); i++)
            m_workers[i].join();
    }
}