#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


#include "SocketServer.h"


using namespace rapidjson;


int main()
{
    SocketServer server;
    server.Init();
    server.Run();

    system("pause");

    return 0;
}