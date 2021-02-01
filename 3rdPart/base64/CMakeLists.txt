CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(base64)

include_directories(include)
add_library(base64 STATIC src/base64.cpp)
target_link_libraries(base64)