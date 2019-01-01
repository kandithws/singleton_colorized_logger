cmake_minimum_required(VERSION 2.7)
project(sclogger)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_executable(sclogger_example example.cpp include/sclogger.h)
