#pragma once

#include <string>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

class ISubscriber
{
public:
    virtual void OnReceive(std::string topicTag, const rapidjson::Document body) = 0;
};