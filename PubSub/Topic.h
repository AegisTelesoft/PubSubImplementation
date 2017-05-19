#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>

#include "ISubscriber.h"

namespace PubSub
{
    class Topic
    {
    public:
        Topic(std::string tag);
        Topic( const Topic &obj);
        ~Topic();

    public:
        void AddSubscriber(ISubscriber* subscriber);
        std::vector<ISubscriber*>& GetSubscribers();
        unsigned GetSubCount();

    private:
        std::vector<ISubscriber*> m_subscribers;
        unsigned m_subCount;
        std::string m_tag;
        std::mutex m_count;
        std::mutex m_subs;

    };
}

