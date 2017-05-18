#include "Broker.h"

#include <functional>
#include <iostream>
#include <string>

#include "Topic.h"

namespace PubSub
{
    Broker* Broker::m_instance = nullptr;

    void Broker::masterTask()
    {
        Broker * instance = Broker::getInstance();

        while(!instance->m_stop)
        {
            std::pair<std::string, std::string> message;

            {
                std::unique_lock<std::mutex> lock(instance->m_queueMutex);

                instance->m_condition.wait(lock, [instance] {
                    return !instance->m_messageQueue.empty();
                });
            }

            std::unique_lock<std::mutex> lock(instance->m_mutex);

            if (instance->m_stop)
                return;

            lock.unlock();

            std::unique_lock<std::mutex> queueLock(instance->m_queueMutex);

            message = std::move(instance->m_messageQueue.front());
            instance->m_messageQueue.pop();
            queueLock.unlock();

            std::unique_lock<std::mutex> topicLock(instance->m_mutex);
            topicHashmap::const_iterator topic = instance->m_topicHashmap.find(message.first);

            if (topic != instance->m_topicHashmap.end())
            {
                for (unsigned i = 0; i < topic->second->GetSubCount(); i++)
                {
                    if (topic->second->GetSubscribers()[i] != nullptr)
                    {

                        instance->m_taskPool.Enqueue([&topic, i, message]() {
                            rapidjson::Document doc;
                            doc.Parse(message.second.c_str());

                            topic->second->GetSubscribers()[i]->OnReceive(message.first, doc);
                        });
                    }
                }
            }
            //topicLock.unlock();
        }
    }

    void Broker::EnqueueMessage(std::string topicTag, const rapidjson::Document& body)
    {
        Broker * instance = Broker::getInstance();

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        body.Accept(writer);
        std::string jsonData(buffer.GetString());

        instance->m_taskPool.Enqueue(std::move([instance, topicTag, jsonData]() {

            std::unique_lock<std::mutex> lock(instance->m_queueMutex);
            instance->m_messageQueue.push(std::move(std::make_pair(std::move(topicTag), std::move(jsonData))));
            lock.unlock();

        }));

        instance->m_condition.notify_one();
    }

    void Broker::AddSubscription(ISubscriber *subscriber, std::string topicTag)
    {
        Broker * instance = Broker::getInstance();

        instance->m_taskPool.Enqueue(std::move([instance, topicTag, subscriber]() {

            topicHashmap::const_iterator topic = instance->m_topicHashmap.find(topicTag);

            if (topic != instance->m_topicHashmap.end()) {
                std::unique_lock<std::mutex> lock(instance->m_mutex);
                topic->second->AddSubscriber(std::move(subscriber));
                lock.unlock();
            } else {
                Topic *newTopic = new Topic(topicTag);
                newTopic->AddSubscriber(std::move(subscriber));
                std::unique_lock<std::mutex> lock(instance->m_mutex);
                instance->m_topicHashmap.emplace(topicTag, newTopic);
                lock.unlock();
            }

        }));
    }

    Broker* Broker::getInstance()
    {
        if(!m_instance)
        {
            m_instance = new Broker();
        }
        return m_instance;
    }

    Broker::Broker() : m_taskPool(50), m_stop(false)
    {
        m_masterThread = std::thread(masterTask);
    }

    Broker::~Broker()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_stop = true;
        lock.unlock();

        m_condition.notify_all();

        m_masterThread.join();

        for (auto it : m_topicHashmap)
            delete it.second;
    }
}