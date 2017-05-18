#include "TaskPool.h"

#include <iostream>

namespace PubSub
{
    TaskPool::TaskPool(unsigned threads) : m_stop(false)
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

    void TaskPool::Enqueue(std::function<void()> task)
    {
        if(!m_stop)
        {

            std::unique_lock<std::mutex> lock(m_quequeMutex);
            m_tasks.push(std::move(task));
            lock.unlock();

            m_condition.notify_one();
        }
    }

    TaskPool::~TaskPool()
    {
        std::unique_lock<std::mutex> lock(this->m_quequeMutex);
        m_stop = true;
        lock.unlock();

        m_condition.notify_all();

        for(int i = 0; i < m_workers.size(); i++)
            m_workers[i].join();
    }
}