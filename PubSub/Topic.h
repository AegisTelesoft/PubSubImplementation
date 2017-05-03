#pragma once

#include <string>
#include <vector>
#include <functional>

namespace PubSub {

    class Topic {

    public:
        Topic(std::string tag);
        ~Topic();

    public:
        std::string GetTopicTag();
        void AddSubscription(std::function<void(std::string)> subscription);
        std::vector<std::function<void(std::string)>> &GetSubscriptions();

    private:
        std::string m_topicTag;
        std::vector<std::function<void(std::string)>> m_subscriptions;
    };
}