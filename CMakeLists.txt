cmake_minimum_required(VERSION 3.9)
project(main)

find_package(Boost 1.76 REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main main.cpp)

add_library(boost INTERFACE)
target_include_directories(boost INTERFACE /usr/local/boost_1_76_0)

find_library(comparepackages libcomparepackages.so.1.0.0 PATHS lib/build)
target_link_libraries(main PRIVATE boost)
if(comparepackages)
    target_link_libraries(main PRIVATE ${comparepackages})
else()
    message(FATAL_ERROR "libcomparepackages.so.1.0.0 not found")
endif()


