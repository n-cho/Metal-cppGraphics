cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
project(Graphics)

add_subdirectory(cmake-metal)

set(SOURCES
    src/main.cpp
    src/Renderer.cpp
    src/AppDelegate.cpp
    src/MTKViewDelegate.cpp)

add_executable(Graphics ${SOURCES})
target_link_libraries(Graphics Metal-cpp)

set(DIRS
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/shaders)

target_include_directories(Graphics PRIVATE ${DIRS})
