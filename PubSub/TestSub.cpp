#include "TestSub.h"

#include <iostream>

TestSub::TestSub()
{

}

void TestSub::OnReceive(std::string topicTag, rapidjson::Document& body)
{
    //int shit = body["iteration"].GetInt();

    std::cout<< "hey" << std::endl;
}

TestSub::~TestSub()
{

}