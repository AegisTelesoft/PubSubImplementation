#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

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

    std::vector<std::thread> spammers;

    system("pause");

    for(int i = 0; i < 10; i++)
    {
        spammers.push_back(std::thread([&pub1]()
        {
            const char* json = "{\"start_time\":0}";

            Document d;
            d.Parse(json);

            for(int i = 0; i < 1000; i++)
            {
                auto now = std::chrono::system_clock::now();
                auto msNow = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch();

                Value& s = d["start_time"];
                s.SetInt64(msNow.count());

                pub1.Publish("tag2", d);
            }
        }));
    }

    for(int i = 0; i < spammers.size(); i++)
    {
        spammers[i].join();
    }

    system("pause");

    return 0;
}