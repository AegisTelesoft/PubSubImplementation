#include "TestSub.h"

#include <iostream>

TestSub::TestSub()
{

}

void TestSub::OnReceive(std::string topicTag, const rapidjson::Document body)
{
    std::cout<< "Hey" << std::endl;
}

TestSub::~TestSub()
{

}