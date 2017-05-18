#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>

#include "Topic.h"
#include "ISubscriber.h"
#include "TaskPool.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace PubSub
{
    typedef std::unordered_map<std::string, Topic*> topicHashmap;

    class Broker
    {
    public:
        static void EnqueueMessage(std::string topicTag, const rapidjson::Document& body);
        static void AddSubscription(ISubscriber *subscriber, std::string topicTag);

    private:
        static Broker* getInstance();
        static void masterTask();
        Broker();
        ~Broker();

    private:
        topicHashmap m_topicHashmap;
        static Broker* m_instance;
        TaskPool m_taskPool;
        std::mutex m_mutex;
        std::mutex m_queueMutex;

        std::queue<std::pair<std::string, std::string>> m_messageQueue;
        std::thread m_masterThread;
        std::condition_variable m_condition;
        bool m_stop;
    };
}