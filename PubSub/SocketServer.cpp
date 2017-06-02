#include "SocketServer.h"

#include <iostream>
#include <string>

#include "Broker.h"
#include "Message.pb.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

HandlerData::HandlerData(CancelationToken& token, SOCKET clientSocket, struct sockaddr_in clientAddr, std::shared_ptr<spdlog::logger>& logger)
        : Token(token), Client(clientSocket), ClientAddr(clientAddr), Logger(logger)
{

}

SocketServer::SocketServer() : m_ListenSocket(INVALID_SOCKET)
{
    try
    {
        CreateDirectoryA("logs", NULL);
        m_fileLogger = spdlog::daily_logger_mt("daily_logger", "logs/server_log_", 12, 10);
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cout << "Failed to initiate logger: " << ex.what() << std::endl;
    }
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
        m_fileLogger->error("WSA Startup failed with error: {}", iResult);
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
        m_fileLogger->error("Getaddrinfo failed with error: {}", iResult);

        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connection listening
    m_ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (m_ListenSocket == INVALID_SOCKET)
    {
        m_fileLogger->error("Socket failed with error: {}", WSAGetLastError());

        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( m_ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        m_fileLogger->error("Socket bind failed with error: {}", WSAGetLastError());

        freeaddrinfo(result);
        closesocket(m_ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(m_ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        m_fileLogger->error("Socket listen failed with error: {}", WSAGetLastError());

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
        m_fileLogger->info("Connection accepted: {}.{}.{}.{}",
                              int(client.sin_addr.s_addr&0xFF),
                              int((client.sin_addr.s_addr&0xFF00)>>8),
                              int((client.sin_addr.s_addr&0xFF0000)>>16),
                              int((client.sin_addr.s_addr&0xFF000000)>>24)
        );

        HandlerData data(m_token, clientSocket, client, m_fileLogger);
        m_connectionHandlers.emplace_back(handleConnection, std::move(data));
    }

    ShutDown();

    return 0;
}

int SocketServer::handleConnection(HandlerData data)
{
    std::vector<NetSubscriber*> clientSubs;
	int bytesRecv;

    do
    {
		char recvBuffer[BUFFER_SIZE];

        bytesRecv = recv(data.Client, recvBuffer, BUFFER_SIZE, 0);
        if (bytesRecv > 0)
        {
            data.Logger->info("{}.{}.{}.{} - Bytes received: {}",
                              int(data.ClientAddr.sin_addr.s_addr&0xFF),
                              int((data.ClientAddr.sin_addr.s_addr&0xFF00)>>8),
                              int((data.ClientAddr.sin_addr.s_addr&0xFF0000)>>16),
                              int((data.ClientAddr.sin_addr.s_addr&0xFF000000)>>24),
                              bytesRecv
            );

			int delimiter = 0;
			std::string buffer(recvBuffer);
			std::vector<std::string> messages;

			do
			{
				int oldDelimiter = delimiter;
				delimiter = buffer.find('\b\x1\x12', oldDelimiter + 9);

				messages.push_back(buffer.substr(oldDelimiter, delimiter - oldDelimiter));

			} while (delimiter != std::string::npos);

			for (int i = 0; i < messages.size(); i++)
			{
				PubSubMessage::Message incomingMessage;
				incomingMessage.ParseFromArray(messages[i].c_str(), messages[i].size());

				switch(incomingMessage.type())
				{
					case PubSubMessage::SUBSCRIBE:
					{
                        data.Logger->info("{}.{}.{}.{} - New Subscription - topic_tag\":{}\"",
                                          int(data.ClientAddr.sin_addr.s_addr&0xFF),
                                          int((data.ClientAddr.sin_addr.s_addr&0xFF00)>>8),
                                          int((data.ClientAddr.sin_addr.s_addr&0xFF0000)>>16),
                                          int((data.ClientAddr.sin_addr.s_addr&0xFF000000)>>24),
                                          incomingMessage.topic()
                        );

						NetSubscriber* newSub = new NetSubscriber(data.Client);
						PubSub::Broker::AddSubscription(*newSub, incomingMessage.topic());

						clientSubs.push_back(std::move(newSub));
						break;
					}
					case PubSubMessage::PUBLISH:
					{
                        data.Logger->info("{}.{}.{}.{} - New post - topic_tag\":{}\", json_body:\"{}\"",
                                          int(data.ClientAddr.sin_addr.s_addr&0xFF),
                                          int((data.ClientAddr.sin_addr.s_addr&0xFF00)>>8),
                                          int((data.ClientAddr.sin_addr.s_addr&0xFF0000)>>16),
                                          int((data.ClientAddr.sin_addr.s_addr&0xFF000000)>>24),
                                          incomingMessage.topic(),
                                          incomingMessage.jsonbody()
                        );

						rapidjson::Document d;
						d.Parse(incomingMessage.jsonbody().c_str());

						PubSub::Broker::EnqueueMessage(incomingMessage.topic(), d);
						break;
					}
					default:
						break;
				}
			}

        }
        else
        {
            data.Logger->info("{}.{}.{}.{} - Client disconnected.",
                              int(data.ClientAddr.sin_addr.s_addr&0xFF),
                              int((data.ClientAddr.sin_addr.s_addr&0xFF00)>>8),
                              int((data.ClientAddr.sin_addr.s_addr&0xFF0000)>>16),
                              int((data.ClientAddr.sin_addr.s_addr&0xFF000000)>>24)
            );

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

    int iResult = shutdown(m_ListenSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        m_fileLogger->error("shutdown failed with error: {}", WSAGetLastError());

        closesocket(m_ListenSocket);
        WSACleanup();
        return 1;
    }
    else
        m_fileLogger->info("Server shutdown successfully.");

    closesocket(m_ListenSocket);
    WSACleanup();
    spdlog::drop_all();
}

SocketServer::~SocketServer()
{
    ShutDown();
}