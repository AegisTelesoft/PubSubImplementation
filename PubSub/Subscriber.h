#pragma once

#include <string>
#include <functional>

class Subscriber
{
public:
    Subscriber();
    ~Subscriber();

public:
    void Subscribe(std::string tag, std::function<void(std::string)> callback);
};