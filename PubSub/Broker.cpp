#include "Broker.h"

#include <functional>
#include <iostream>
#include <string>

#include "Topic.h"

namespace PubSub
{
    Broker* Broker::m_instance = nullptr;

    void Broker::EnqueueMessage(std::string topicTag, rapidjson::Document body)
    {
        Broker * instance = Broker::getInstance();

        instance->m_threadPool.Enqueque([instance, topicTag, &body](){

            std::unique_lock<std::mutex> lock(instance->m_mutex);

            topicHashmap::const_iterator topic = instance->m_topicHashmap.find(topicTag);
			 
            if(topic != instance->m_topicHashmap.end())
            {
                unsigned count = topic->second->GetSubCount();

                for (unsigned i = 0; i < count; i++) {
                    rapidjson::Document aCopy;
                    aCopy.CopyFrom(body, body.GetAllocator());

                    if (topic->second->GetSubscribers()[i] != NULL)
                        topic->second->GetSubscribers()[i]->OnReceive(topicTag, std::move(aCopy));

                }
            }
            lock.unlock();
        });
    }

    void Broker::AddSubscription(ISubscriber *subscriber, std::string topicTag)
    {
        Broker * instance = Broker::getInstance();


        instance->m_threadPool.Enqueque([instance, topicTag, subscriber](){

            std::unique_lock<std::mutex> lock(instance->m_mutex);

            topicHashmap::const_iterator topic = instance->m_topicHashmap.find(topicTag);

            if(topic != instance->m_topicHashmap.end())
            {
                topic->second->AddSubscriber(std::move(subscriber));
            }
            else
            {
                Topic * newTopic = new Topic(topicTag);
                newTopic->AddSubscriber(std::move(subscriber));
                instance->m_topicHashmap.emplace(topicTag, newTopic);
            }

        });
    }

    Broker* Broker::getInstance()
    {
        if(!m_instance)
        {
            m_instance = new Broker();
        }

        return m_instance;
    }

    Broker::Broker() : m_threadPool(3)
    {

    }

    Broker::~Broker()
    {
        Broker * instance = Broker::getInstance();

        for (auto it : instance->m_topicHashmap)
            delete it.second;
    }
}