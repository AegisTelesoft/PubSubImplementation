#include "TaskQueue.h"

namespace PubSub
{

    TaskQueue::TaskQueue() : m_head(0), m_tail(0)
    {

    }

    TaskQueue::~TaskQueue()
    {

    }

    void TaskQueue::Push(std::function<void()> task)
    {
        std::unique_lock<std::mutex> lock(m_headMutex);

        m_head = ++m_head % MAX_QUEUE_SIZE;

        while(Size() >= MAX_QUEUE_SIZE);

        m_buffer[m_head] = std::move(task);
    }

    std::function<void()> TaskQueue::Pop()
    {
        std::unique_lock<std::mutex> lock(m_tailMutex);

        m_tail = ++m_tail % MAX_QUEUE_SIZE;

        if (Size() < 50)
        {
            std::unique_lock<std::mutex> headLock(m_headMutex);

            if (m_tail != m_head)
                return std::move(m_buffer[m_tail]);
            else
                return nullptr;
        }
        else
            return std::move(m_buffer[m_tail]);
    }

    int TaskQueue::Size()
    {
        int size = 0;

        if(m_head > m_tail)
            size = m_head - m_tail;
        else if (m_tail > m_head)
            size = (m_head + MAX_QUEUE_SIZE) - m_tail;

        return size;

    }
}