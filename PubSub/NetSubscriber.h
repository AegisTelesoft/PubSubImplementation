#pragma once

#include "ISubscriber.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class NetSubscriber : public virtual ISubscriber
{
public:
    NetSubscriber();
    NetSubscriber(const NetSubscriber &obj);
    NetSubscriber(SOCKET clientConnection);
    ~NetSubscriber();

public:
    void OnReceive(std::string topicTag, rapidjson::Document& body);

private:
    SOCKET m_clientSocket;

};