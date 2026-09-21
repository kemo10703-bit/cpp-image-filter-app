cmake_minimum_required(VERSION 3.10)
project(image_filters CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(imgfilters src/main.cpp src/filters.cpp)
target_include_directories(imgfilters PRIVATE src third_party)
