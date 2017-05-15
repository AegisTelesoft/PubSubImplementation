#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>

#include "Topic.h"
#include "ISubscriber.h"
#include "ThreadPool.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace PubSub
{
    typedef std::unordered_map<std::string, Topic*> topicHashmap;

    class Broker
    {
    public:
        static void EnqueueMessage(std::string topicTag, rapidjson::Document body);
        static void AddSubscription(ISubscriber *subscriber, std::string topicTag);

    private:
        static Broker* getInstance();
        Broker();
        ~Broker();

    private:
        topicHashmap m_topicHashmap;
        static Broker* m_instance;
        ThreadPool m_threadPool;

        std::mutex m_mutex;
    };
}