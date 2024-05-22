cmake_minimum_required(VERSION 3.10)
project(ParallServer)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(./pkg)

add_executable(ParallServer ./parallserver.cpp ./pkg/threadpool.cpp)