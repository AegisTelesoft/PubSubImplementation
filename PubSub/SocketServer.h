#pragma Once

#include "spdlog/spdlog.h"
#include <memory>

#define BUFFER_SIZE 10240
#define PORT "8082"

#include <winsock2.h>
#include <ws2tcpip.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>
#include <thread>

// Link with Ws2_32.lib
#pragma comment (lib, "ws2_32.lib")

#include "CancelationToken.h"
#include "NetSubscriber.h"

class HandlerData
{
public:
    HandlerData(CancelationToken& token, SOCKET clientSocket, struct sockaddr_in clientAddr, std::shared_ptr<spdlog::logger>& logger);

public:
    std::shared_ptr<spdlog::logger>& Logger;
    CancelationToken&                Token;
    SOCKET                           Client;
    struct sockaddr_in               ClientAddr;
};

class SocketServer
{
public:
    SocketServer();
    ~SocketServer();
    int Init();
    int ShutDown();
    int Run();

private:
    static int handleConnection(HandlerData data);

private:
    WSADATA m_WSAData;
    SOCKET  m_ListenSocket;
    std::shared_ptr<spdlog::logger> m_fileLogger;

    std::vector<std::thread>   m_connectionHandlers;
    CancelationToken           m_token;

};