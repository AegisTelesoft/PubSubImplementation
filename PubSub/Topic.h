#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <algorithm>

#include "ISubscriber.h"

namespace PubSub
{
    class Topic
    {
    public:
        Topic(std::string tag);
        Topic(const Topic &obj);
        ~Topic();

    public:
        void AddSubscriber(ISubscriber& subscriber);
        std::vector<ISubscriber*>& GetSubscribers();
        void Remove(ISubscriber* subscriber);
        unsigned GetSubCount();

    private:
        std::vector<ISubscriber*> m_subscribers;
        std::string m_tag;
        std::mutex m_subs;

    };
}

