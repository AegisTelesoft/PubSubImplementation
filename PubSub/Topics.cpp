#include "Topics.h"

#include <iostream>

#include <algorithm>
#include <thread>
#include <mutex>

namespace PubSub {

    Topics *Topics::m_topicSingleton = nullptr;

    void Topics::Subscribe(std::string topicTag, std::function<void(std::string)> callback) {

        if (!m_topicSingleton) {
            m_topicSingleton = new Topics();
        }

        std::unique_lock<std::mutex> lock(m_topicSingleton->m_mutex);

        auto result = std::find_if(m_topicSingleton->m_topics.begin(), m_topicSingleton->m_topics.end(),
                                   [topicTag](Topic &element) {
                                       return element.GetTopicTag().compare(topicTag) == 0;
                                   });

        if (result == std::end(m_topicSingleton->m_topics)) {
            Topic newTopic(topicTag);
            newTopic.AddSubscription(callback);

            std::cout << "neradom "<< std::endl;

            m_topicSingleton->m_topics.push_back(std::move(newTopic));
        } else {
            auto iterator = std::distance(m_topicSingleton->m_topics.begin(), result);
            m_topicSingleton->m_topics[iterator].AddSubscription(callback);
        }
        lock.unlock();
    }

    void Topics::Post(std::string topicTag, std::string data) {

        if (!m_topicSingleton) {
            m_topicSingleton = new Topics();
        }

        std::unique_lock<std::mutex> lock(m_topicSingleton->m_mutex);

        auto result = std::find_if(m_topicSingleton->m_topics.begin(), m_topicSingleton->m_topics.end(),
                                   [topicTag](Topic &element) {
                                       return element.GetTopicTag().compare(topicTag) == 0;
                                   });

        if (result != std::end(m_topicSingleton->m_topics)) {
            auto iterator = std::distance(m_topicSingleton->m_topics.begin(), result);



            std::vector<std::function<void(
                    std::string)>> &subscriptions = m_topicSingleton->m_topics[iterator].GetSubscriptions();
            for (int i = 0; i < subscriptions.size(); i++)
            {
                subscriptions[i](data);
            }
        }
        lock.unlock();
    }
}