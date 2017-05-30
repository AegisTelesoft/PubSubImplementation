#include "NetSubscriber.h"

#include "Message.pb.h"
#include "Broker.h"

#include <iostream>
#include <chrono>

NetSubscriber::NetSubscriber()
{

}

NetSubscriber::NetSubscriber(const NetSubscriber &obj) : m_clientSocket(obj.m_clientSocket)
{
    std::cout << "copy" << std::endl;
}

NetSubscriber::NetSubscriber(SOCKET clientConnection) : m_clientSocket(clientConnection)
{

}

void NetSubscriber::OnReceive(std::string topicTag, rapidjson::Document& body)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    body.Accept(writer);
    std::string jsonData(buffer.GetString());

    PubSubMessage::Message newMessage;

    newMessage.set_type(PubSubMessage::DATA);
    newMessage.set_topic(topicTag);
    newMessage.set_jsonbody(jsonData);

    send(m_clientSocket , newMessage.SerializeAsString().c_str() , newMessage.ByteSize() , 0);
}

NetSubscriber::~NetSubscriber()
{

}