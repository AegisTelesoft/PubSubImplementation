#pragma once

#include <string>

class Publisher
{
public:
    Publisher();
    ~Publisher();

public:
    void Publish(std::string topicTag, std::string data);
};