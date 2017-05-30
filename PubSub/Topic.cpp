#include "Topic.h"

namespace PubSub
{
    Topic::Topic(std::string tag) : m_tag(tag)
    {

    }

    Topic::Topic(const Topic &obj)
    {
        m_tag = obj.m_tag;
        m_subscribers = obj.m_subscribers;
    }

    Topic::~Topic()
    {

    }

    void Topic::AddSubscriber(ISubscriber& subscriber)
    {
        std::unique_lock<std::mutex> addSubLock(m_subs);
        m_subscribers.push_back(std::move(&subscriber));
        addSubLock.unlock();

    }

    void Topic::Remove(ISubscriber *subscriber)
    {
        std::unique_lock<std::mutex> SubLock(m_subs);
        m_subscribers.erase(std::remove(m_subscribers.begin(), m_subscribers.end(), subscriber), m_subscribers.end());
        SubLock.unlock();
    }

    unsigned Topic::GetSubCount()
    {
        std::unique_lock<std::mutex> lock(m_subs);
        return m_subscribers.size();
    }

    std::vector<ISubscriber*>& Topic::GetSubscribers()
    {
        std::unique_lock<std::mutex> lock(m_subs);
        return m_subscribers;
    }
}