cmake_minimum_required(VERSION 3.10)
project(TrafficCounter)

# C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# OpenCV
find_package(OpenCV REQUIRED)

# .cpp
add_executable(TrafficCounter 
    main.cpp 
    Tracker.cpp
)

# .hpp
target_include_directories(TrafficCounter PRIVATE include)

# connect OpenCV
target_link_libraries(TrafficCounter ${OpenCV_LIBS})