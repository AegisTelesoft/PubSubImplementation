#pragma once

#include <vector>
#include <string>
#include <functional>
#include <mutex>

#include "Topic.h"

namespace PubSub {

    class Topics {

    public:
        static void Subscribe(std::string topicTag, std::function<void(std::string)> callback);
        static void Post(std::string topicTag, std::string data);

    private:
        Topics() {};
        ~Topics() {};

    private:
        std::vector<Topic> m_topics;
        static Topics *m_topicSingleton;
        std::mutex m_mutex;
    };
}