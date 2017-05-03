cmake_minimum_required(VERSION 3.7)
project(PubSub)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Publisher.cpp Subscriber.cpp Topic.cpp Topics.cpp)
add_executable(PubSub ${SOURCE_FILES})