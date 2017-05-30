#include "Broker.h"

namespace PubSub
{
    Broker* Broker::m_instance = nullptr;

    class ThreadData
    {
    public:
        ThreadData(message_queue& messageQueue,
                   std::mutex& queueMutex, std::unordered_map<std::string, Topic>& topics,
                   std::mutex& topicMutex, std::condition_variable& condition,
                   CancelationToken& token
        );

    public:
        // Message queue refs
        std::queue<std::pair<std::string, std::string>>& MessageQueue;
        std::mutex& QueueMutex;

        // Topic refs
        std::unordered_map<std::string, Topic>& Topics;
        std::mutex& TopicMutex;

        // thread conditioning refs
        std::condition_variable& Condition;
        CancelationToken& Token;
    };

    void dispatcher_task(ThreadData data)
    {
        while(true)
        {
            std::pair<std::string, std::string> message;
            {
                std::unique_lock<std::mutex> lock(data.QueueMutex);

                data.Condition.wait(lock, [data] {
                    return data.Token.IsCanceled() || !data.MessageQueue.empty();
                });

                if (data.Token.IsCanceled() && data.MessageQueue.empty())
                    return;

                message = data.MessageQueue.front();
                data.MessageQueue.pop();
            }


            std::unique_lock<std::mutex> lock(data.TopicMutex);
            auto topic = data.Topics.find(message.first);
            lock.unlock();

            if (topic ==  data.Topics.end())
                return;

            for (unsigned i = 0; i < topic->second.GetSubCount(); i++)
            {

                rapidjson::Document doc;
                doc.Parse(message.second.c_str());

                topic->second.GetSubscribers()[i]->OnReceive(message.first, doc);
            }
        }
    }

    void Broker::AddSubscription(ISubscriber& subscriber, std::string topicTag)
    {
        Broker* instance = Broker::getInstance();

        std::unique_lock<std::mutex> topicLock(instance->m_topicMutex);
        auto topic = instance->m_topics.find(topicTag);

        if(topic != instance->m_topics.end())
        {
            topicLock.unlock();
            topic->second.AddSubscriber(subscriber);
        }
        else
        {
            topicLock.unlock();

            Topic newTopic(topicTag);
            newTopic.AddSubscriber(subscriber);

            topicLock.lock();
            instance->m_topics.insert(std::make_pair(topicTag, newTopic));
            topicLock.unlock();
        }
    }

    void Broker::Unsubscribe(ISubscriber* subscriber)
    {
        Broker* instance = Broker::getInstance();

        std::unique_lock<std::mutex> topicLock(instance->m_topicMutex);

        for(auto i = instance->m_topics.begin(); i != instance->m_topics.end(); i++)
        {
            i->second.Remove(subscriber);
        }
    }

    void Broker::EnqueueMessage(std::string topicTag, const rapidjson::Document& body)
    {
        Broker* instance = Broker::getInstance();

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        body.Accept(writer);
        std::string jsonData(buffer.GetString());

        std::unique_lock<std::mutex> queueLock(instance->m_queueMutex);
        instance->m_messageQueue.push(std::move(std::make_pair(std::move(topicTag), std::move(jsonData))));
        queueLock.unlock();

        instance->m_condition.notify_one();
    }

    Broker *Broker::getInstance()
    {
        if (!m_instance) {
            m_instance = new Broker();
        }
        return m_instance;
    }

    Broker::Broker()
    {
        ThreadData data(m_messageQueue,m_queueMutex, m_topics, m_topicMutex, m_condition, m_cancelationToken);

        for(int i = 0; i < 1000; i++)
            m_dispatchers.push_back(std::thread(dispatcher_task, data));
    }

    Broker::~Broker()
    {
        m_cancelationToken.Cancel();
        m_condition.notify_all();

        for(int i = 0; i < m_dispatchers.size(); i++)
            m_dispatchers[i].join();
    }

    ThreadData::ThreadData(message_queue& messageQueue,
                           std::mutex& queueMutex, std::unordered_map<std::string, Topic>& topics,
                           std::mutex& topicMutex, std::condition_variable& condition,
                           CancelationToken& token)
    : Token(token), MessageQueue(messageQueue), QueueMutex(queueMutex),
          Topics(topics), TopicMutex(topicMutex), Condition(condition)
    {

    }
}