#include "Topic.h"

namespace PubSub {

    Topic::Topic(std::string tag) : m_topicTag(tag) {

    }

    Topic::~Topic() {

    }

    std::string Topic::GetTopicTag() {
        return m_topicTag;
    }

    void Topic::AddSubscription(std::function<void(std::string)> subscription) {
        m_subscriptions.push_back(subscription);
    }

    std::vector<std::function<void(std::string)>> &Topic::GetSubscriptions() {
        return m_subscriptions;
    }
}