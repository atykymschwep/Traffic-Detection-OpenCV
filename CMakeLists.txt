cmake_minimum_required(VERSION 3.10)
project(TrafficCounter)

# C++
set(CMAKE_CXX_STANDARD 17)

# OpenCV
find_package(OpenCV REQUIRED)

# main.cpp
add_executable(TrafficCounter main.cpp)

# connect OpenCV
target_link_libraries(TrafficCounter ${OpenCV_LIBS})