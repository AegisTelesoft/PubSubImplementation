#pragma once

#include <string>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

class Publisher
{
public:
    void Publish(std::string topicTag, rapidjson::Document& body);

};