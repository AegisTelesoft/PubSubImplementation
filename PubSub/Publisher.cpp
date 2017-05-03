#include "Publisher.h"
#include "Topics.h"

Publisher::Publisher()
{

}

Publisher::~Publisher()
{

}

void Publisher::Publish(std::string topicTag, std::string data)
{
    PubSub::Topics::Post(topicTag, data);
}