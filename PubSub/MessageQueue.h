#pragma once

#include <functional>
#include <mutex>

#ifndef MAX_QUEUE_SIZE
#define MAX_QUEUE_SIZE 50000
#endif

namespace PubSub {

    class MessageQueue {
    public:
        MessageQueue();

        ~MessageQueue();

    public:
        void Push(std::function<void()> task);

        std::function<void()> Pop();

        int Size();

    private:
        int m_head;
        int m_tail;
        std::mutex m_headMutex;
        std::mutex m_tailMutex;
        std::function<void()> m_buffer[MAX_QUEUE_SIZE];

    };
}