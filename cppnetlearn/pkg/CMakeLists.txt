cmake_minimum_required(VERSION 3.10)

# 项目名称和版本
project(MyProject VERSION 1.0)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加一个可执行文件
add_executable(testExecutable test.cpp ../pkg/threadpool.cpp ../pkg/TaskQueue.cpp)
#add_executable(testExecutable test.cpp ../pkg2/threadpool.cpp ../pkg2/TaskQueue.cpp)
# 包含头文件目录
target_include_directories(testExecutable PRIVATE ..)