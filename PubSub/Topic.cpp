#include "Topic.h"

namespace PubSub
{
    Topic::Topic(std::string tag) : m_tag(tag), m_subCount(0)
    {

    }

    Topic::~Topic()
    {

    }

    std::string Topic::GetTag()
    {
        return m_tag;
    }

    void Topic::AddSubscriber(ISubscriber* subscriber)
    {
        std::unique_lock<std::mutex> addSubLock(m_subs);
        m_subscribers.push_back(std::move(subscriber));
        addSubLock.unlock();

        std::unique_lock<std::mutex> incCountLock(m_count);
        m_subCount++;
        incCountLock.unlock();
    }

    unsigned Topic::GetSubCount()
    {
        std::unique_lock<std::mutex> lock(m_count);
        return m_subCount;
    }

    std::vector<ISubscriber*>& Topic::GetSubscribers()
    {
        std::unique_lock<std::mutex> lock(m_subs);
        return m_subscribers;
    }
}