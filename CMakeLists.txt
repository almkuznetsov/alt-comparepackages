cmake_minimum_required(VERSION 3.9)
project(main)

find_package(Boost REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main main.cpp)

add_library(boost INTERFACE)
target_include_directories(boost INTERFACE /opt/homebrew/include)

find_library(comparepackages libcomparepackages.1.0.0.dylib PATHS lib/build)
target_link_libraries(main PRIVATE boost)
if(comparepackages)
    target_link_libraries(main PRIVATE ${comparepackages})
else()
    message(FATAL_ERROR "Библиотека libcomparepackages.1.0.0.dylib не найдена.")
endif()


