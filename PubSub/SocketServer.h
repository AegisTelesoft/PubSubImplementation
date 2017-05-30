#pragma Once

#define BUFFER_SIZE 1024
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
    HandlerData(CancelationToken& token, SOCKET clientSocket);

public:
    CancelationToken& Token;
    SOCKET            Client;
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

    std::vector<std::thread>   m_connectionHandlers;
    CancelationToken           m_token;

};