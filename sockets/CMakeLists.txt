cmake_minimum_required(VERSION 2.8)

add_definitions(-std=c++11)

project(sockets)    

set(SOURCE_LIB sockets.cpp)    

add_library(sockets STATIC ${SOURCE_LIB})

