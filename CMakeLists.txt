cmake_minimum_required(VERSION 2.8)

add_definitions(-std=c++11)

project(client_server)    

set(SOURCE_CLIENT client.cpp)    
set(SOURCE_SERVER server.cpp)

include_directories(sockets)

add_executable(server ${SOURCE_SERVER})
add_executable(client ${SOURCE_CLIENT})

add_subdirectory(sockets)

target_link_libraries(client sockets)
target_link_libraries(server sockets)

