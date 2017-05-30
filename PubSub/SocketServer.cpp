#include "SocketServer.h"

#include <iostream>
#include <string>

#include "Broker.h"
#include "Message.pb.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

HandlerData::HandlerData(CancelationToken& token, SOCKET clientSocket)
        : Token(token), Client(clientSocket)
{

}


SocketServer::SocketServer() : m_ListenSocket(INVALID_SOCKET)
{

}

int SocketServer::Init()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    int iResult;
    struct addrinfo server;
    struct addrinfo *result = NULL;

    iResult = WSAStartup(MAKEWORD(2,2), &m_WSAData);
    if (iResult != 0)
    {
        std::cout << "WSA Startup failed with error: " << iResult << std::endl;
        return 1;
    }

    ZeroMemory(&server, sizeof(server));
    server.ai_family = AF_INET;
    server.ai_socktype = SOCK_STREAM;
    server.ai_protocol = IPPROTO_TCP;
    server.ai_flags = AI_PASSIVE;


    // Resolve the server address and port
    iResult = getaddrinfo(NULL, PORT, &server, &result);
    if ( iResult != 0 )
    {
        std::cout << "getaddrinfo failed with error: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    m_ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (m_ListenSocket == INVALID_SOCKET)
    {
        std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( m_ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        std::cout << "bind failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        closesocket(m_ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(m_ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        std::cout << "listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(m_ListenSocket);
        WSACleanup();
        return 1;
    }
}

int SocketServer::Run()
{
    struct sockaddr_in client;
    int c = sizeof(struct sockaddr_in);

    SOCKET clientSocket;

    while((clientSocket = accept(m_ListenSocket, (struct sockaddr *)&client, &c)) != INVALID_SOCKET && !m_token.IsCanceled())
    {
        std::cout << "Connection accepted" << std::endl;

        HandlerData data(m_token, clientSocket);
        m_connectionHandlers.emplace_back(handleConnection, std::move(data));
    }

    ShutDown();

    return 0;
}

int SocketServer::handleConnection(HandlerData data)
{
    int bytesSent;
    int bytesRecv;

    char sendBuffer[BUFFER_SIZE];
    char recvBuffer[BUFFER_SIZE];

    std::vector<NetSubscriber*> clientSubs;

    do
    {
        bytesRecv = recv(data.Client, recvBuffer, BUFFER_SIZE, 0);
        if (bytesRecv > 0)
        {
            std::cout << "Bytes received: " << bytesRecv << std::endl;

            PubSubMessage::Message incomingMessage;
            incomingMessage.ParseFromArray(&recvBuffer, BUFFER_SIZE);

            switch(incomingMessage.type())
            {
                case PubSubMessage::SUBSCRIBE:
                {

                    std::cout << "New Sub" << std::endl;

                    NetSubscriber* newSub = new NetSubscriber(data.Client);
                    PubSub::Broker::AddSubscription(*newSub, incomingMessage.topic());

                    clientSubs.push_back(std::move(newSub));
                    break;
                }
                case PubSubMessage::PUBLISH:
                {
                    std::cout << "New Post" << std::endl;

                    rapidjson::Document d;
                    d.Parse(incomingMessage.jsonbody().c_str());

                    PubSub::Broker::EnqueueMessage(incomingMessage.topic(), d);
                    break;
                }
                default:
                    break;
            }

        }
        else
        {
            std::cout << "Client disconnected" << std::endl;
            closesocket(data.Client);
            break;
        }

    } while (bytesRecv > 0 && !data.Token.IsCanceled());

    for(int i = clientSubs.size()-1; i >= 0; i--)
    {
        PubSub::Broker::Unsubscribe(clientSubs[i]);
        delete clientSubs[i];
    }

    closesocket(data.Client);

    return 0;
}

int SocketServer::ShutDown()
{
    m_token.Cancel();

    for(int c = 0; c < m_connectionHandlers.size(); c++)
    {
        m_connectionHandlers[c].join();
    }

    // shutdown the connection since we're done
    int iResult = shutdown(m_ListenSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        std::cout << "shutdown failed with error: " << WSAGetLastError() << std::endl;
        closesocket(m_ListenSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(m_ListenSocket);
    WSACleanup();
}

SocketServer::~SocketServer()
{
    ShutDown();
}