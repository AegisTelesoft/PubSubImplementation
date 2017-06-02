cmake_minimum_required(VERSION 3.7)
project(PubSub)

set(CMAKE_CXX_STANDARD 14)

include_directories(
    "${CMAKE_SOURCE_DIR}/rapidjson/include"
    "${CMAKE_SOURCE_DIR}/protobuf/include"
    "${CMAKE_SOURCE_DIR}/spdlog/include"
)

link_directories(${CMAKE_SOURCE_DIR}/protobuf/lib/)

set(SOURCE_FILES main.cpp Broker.cpp ISubscriber.h Topic.cpp Topic.h CancelationToken.cpp NetPublisher.cpp NetSubscriber.cpp Publisher.cpp SocketServer.cpp Algorithm.cpp Message.pb.cc)

add_executable(PubSub ${SOURCE_FILES})

target_link_libraries(PubSub libprotobufd)