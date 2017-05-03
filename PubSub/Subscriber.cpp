#include "Subscriber.h"
#include "Topics.h"

Subscriber::Subscriber()
{

}

Subscriber::~Subscriber()
{

}

void Subscriber::Subscribe(std::string tag, std::function<void(std::string)> callback)
{
    PubSub::Topics::Subscribe(tag, callback);
}