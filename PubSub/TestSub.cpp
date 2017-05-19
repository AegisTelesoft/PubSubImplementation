#include "TestSub.h"

#include <iostream>
#include <chrono>

TestSub::TestSub() : i(0)
{

}

void TestSub::OnReceive(std::string topicTag, rapidjson::Document& body)
{
    long long int msStart = body["start_time"].GetInt64();

    auto now = std::chrono::system_clock::now();
    auto msNow = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch();

    long long int diff = msNow.count() - msStart;

    std::cout << diff << std::endl;


    /*std::unique_lock<std::mutex> lock(m);
    i++;
    std::cout << i << std::endl;
    lock.unlock();*/
}

TestSub::~TestSub()
{

}