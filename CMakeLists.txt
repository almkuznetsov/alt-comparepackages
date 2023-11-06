cmake_minimum_required(VERSION 3.9)
project(main)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main main.cpp)
add_library(boost INTERFACE)
target_include_directories(boost INTERFACE /opt/homebrew/include)
target_link_libraries(main PRIVATE boost)