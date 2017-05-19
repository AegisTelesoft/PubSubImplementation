#pragma once

#include "ISubscriber.h"

#include <mutex>

class TestSub : public virtual ISubscriber
{
public:
    TestSub();
    ~TestSub();

    void OnReceive(std::string topicTag, rapidjson::Document& body);

    std::mutex m;
    int i;


};