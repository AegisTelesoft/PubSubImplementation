cmake_minimum_required(VERSION 3.7)
project(PubSub)

set(CMAKE_CXX_STANDARD 11)

include_directories("${CMAKE_SOURCE_DIR}/rapidjson/include")

set(SOURCE_FILES main.cpp Topic.cpp Broker.cpp TaskPool.cpp Publisher.cpp ISubscriber.h TestSub.h TestSub.cpp TestPub.h TestPub.cpp MessageQueue.cpp)

add_executable(PubSub ${SOURCE_FILES})