cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
project(Graphics)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    src/main.cpp
    src/Renderer.cpp
    src/AppDelegate.cpp
    src/MTKViewDelegate.cpp)

add_executable(Graphics ${SOURCES})

set(DIRS
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/shaders
    ${PROJECT_SOURCE_DIR}/assets
    ${PROJECT_SOURCE_DIR}/dependencies/metal-cpp
    ${PROJECT_SOURCE_DIR}/dependencies/metal-cpp-extensions)

target_include_directories(Graphics PRIVATE ${DIRS})

target_link_libraries(Graphics
  "-framework Metal"
  "-framework Foundation"
  "-framework QuartzCore"
  "-framework MetalKit"
  "-framework AppKit"
  )
