#include "ThreadPool.h"

#include <iostream>

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
                        std::unique_lock<std::mutex> lock(this->m_stopMutex);

                        this->m_condition.wait(lock, [this] {
                            return this->m_stop || this->m_tasks.Size() > 0;
                        });

                        if (this->m_stop)
                            return;
                    }

                    task = std::move(this->m_tasks.Pop());

                    if(task != nullptr)
                        task();

                    std::cout << this->m_tasks.Size() << std::endl;

                }
            });
        }
    }

    void ThreadPool::Enqueque(std::function<void()> task)
    {
        std::unique_lock<std::mutex> stopLock(this->m_stopMutex);
        if(!m_stop)
        {
            stopLock.unlock();

            m_tasks.Push(std::move(task));

            m_condition.notify_one();
        }
    }

    ThreadPool::~ThreadPool()
    {
        std::unique_lock<std::mutex> stopLock(this->m_stopMutex);
        m_stop = true;
        stopLock.unlock();

        m_condition.notify_all();

        for(int i = 0; i < m_workers.size(); i++)
            m_workers[i].join();
    }
}