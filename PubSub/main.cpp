#include <iostream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


#include "TestPub.h"
#include "TestSub.h"
#include "Broker.h"


using namespace rapidjson;


int main() {


    TestSub* sub1Ptr;
    TestSub sub1;
    sub1Ptr = &sub1;

    TestSub* sub2Ptr;
    TestSub sub2;
    sub2Ptr = &sub2;

    PubSub::Broker::AddSubscription(sub1Ptr, "tag1");
    PubSub::Broker::AddSubscription(sub2Ptr, "tag2");

    TestPub pub1;

    for(int i = 0; i < 10000; i++)
    {
        std::string jsonString("{\"iteration\": %d}", i);
        Document d;
        d.Parse(jsonString.c_str());

        pub1.Publish("tag2", d);
    }

    system("pause");

    return 0;
}