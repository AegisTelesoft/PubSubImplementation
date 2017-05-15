#include <iostream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


#include "TestPub.h"
#include "TestSub.h"
#include "Broker.h"


using namespace rapidjson;


int main() {

    const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
    Document d;
    d.Parse(json);

    TestSub* sub1Ptr;
    TestSub sub1;
    sub1Ptr = &sub1;

    TestSub* sub2Ptr;
    TestSub sub2;
    sub2Ptr = &sub2;

    PubSub::Broker::AddSubscription(sub1Ptr, "tag1");
    PubSub::Broker::AddSubscription(sub2Ptr, "tag2");

    TestPub pub1;

    for(int i = 0; i < 100; i++)
    {
        rapidjson::Document aCopy;
        aCopy.CopyFrom(d, d.GetAllocator());
        pub1.Publish("tag2", std::move(aCopy));
    }

    system("pause");

    return 0;
}