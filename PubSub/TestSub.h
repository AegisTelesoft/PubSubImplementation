#pragma once

#include "ISubscriber.h"

class TestSub : public virtual ISubscriber
{
public:
    TestSub();
    ~TestSub();

    void OnReceive(std::string topicTag, rapidjson::Document& body);

};