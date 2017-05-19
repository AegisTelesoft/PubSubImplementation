#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <condition_variable>

#include "ISubscriber.h"
#include "Topic.h"
#include "CancelationToken.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


namespace PubSub
{
    class Broker
    {
    public:
        static void EnqueueMessage(std::string topicTag, const rapidjson::Document& body);
        static void AddSubscription(ISubscriber *subscriber, std::string topicTag);

    private:
        static Broker* getInstance();
        Broker();
        ~Broker();

    private:
        static Broker* m_instance;

        // Topic members
        std::mutex m_topicMutex;
        std::unordered_map<std::string, PubSub::Topic> m_topics;

        //MessageQueue members
        std::mutex m_queueMutex;
        std::queue<std::pair<std::string, std::string>> m_messageQueue;

        //Tread pool members
        std::vector<std::thread> m_dispatchers;
        std::condition_variable m_condition;
        CancelationToken m_cancelationToken;
    };
}

