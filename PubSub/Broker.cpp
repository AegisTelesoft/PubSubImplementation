#include "Broker.h"

#include <functional>
#include <iostream>
#include <string>

#include "Topic.h"

namespace PubSub
{
    Broker* Broker::m_instance = nullptr;

    void Broker::EnqueueMessage(std::string topicTag, const rapidjson::Document& body)
    {
        Broker * instance = Broker::getInstance();


        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        body.Accept(writer);
        std::string jsonData(buffer.GetString());

        instance->m_threadPool.Enqueque(std::move([instance, topicTag, jsonData](){

            std::unique_lock<std::mutex> lock(instance->m_mutex);

            topicHashmap::const_iterator topic = instance->m_topicHashmap.find(topicTag);
			 
            if (topic != instance->m_topicHashmap.end())
            {
                for (unsigned i = 0; i < topic->second->GetSubCount(); i++)
                {
                    if (topic->second->GetSubscribers()[i] != nullptr)
                    {
                        //lock.unlock();
                        rapidjson::Document doc;
                        doc.Parse(jsonData.c_str());

                        topic->second->GetSubscribers()[i]->OnReceive(topicTag, doc);
                    }
                }
            }
        }));
    }

    void Broker::AddSubscription(ISubscriber *subscriber, std::string topicTag)
    {
        Broker * instance = Broker::getInstance();


        instance->m_threadPool.Enqueque(std::move([instance, topicTag, subscriber](){

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

    Broker::Broker() : m_threadPool(50)
    {

    }

    Broker::~Broker()
    {
        Broker * instance = Broker::getInstance();

        for (auto it : instance->m_topicHashmap)
            delete it.second;
    }
}