# Add (create) "Metal-cpp" library to the project using "/source.cpp"
add_library(Metal-cpp ${CMAKE_CURRENT_SOURCE_DIR}/source.cpp)

# Add the headers in "/metal-cpp" to "Metal-cpp" library
target_include_directories(Metal-cpp PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/metal-cpp")

# Link frameworks
target_link_libraries(Metal-cpp
  "-framework Metal"
  "-framework Foundation"
  "-framework QuartzCore"
  )
