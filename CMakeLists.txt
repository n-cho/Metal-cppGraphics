cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
project(Graphics)

add_subdirectory(cmake-metal)

add_executable(Graphics main.cpp)
target_link_libraries(Graphics Metal-cpp)
