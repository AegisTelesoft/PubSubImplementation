cmake_minimum_required(VERSION 3.7)
project(PubSub)

set(CMAKE_CXX_STANDARD 14)

include_directories("${CMAKE_SOURCE_DIR}/rapidjson/include")

set(SOURCE_FILES main.cpp Broker.cpp ISubscriber.h Topic.cpp Topic.h CancelationToken.cpp TestPub.cpp TestSub.cpp Publisher.cpp)

add_executable(PubSub ${SOURCE_FILES})