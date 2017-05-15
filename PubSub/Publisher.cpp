#include "Publisher.h"
#include "Broker.h"

void Publisher::Publish(std::string topicTag, rapidjson::Document body)
{
    rapidjson::Document aCopy;
    aCopy.CopyFrom(body, body.GetAllocator());
    PubSub::Broker::EnqueueMessage(topicTag, std::move(aCopy));
}