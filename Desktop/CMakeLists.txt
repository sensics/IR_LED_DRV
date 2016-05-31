cmake_minimum_required(VERSION 3.1.0)
project(OSVR-IR-Driver-Desktop)

# Get me C++11!
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(JsonCpp REQUIRED)

set(USER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../User")
include_directories("${USER_DIR}")

add_executable(DumpPatterns
    DumpPatterns.cpp
    "${USER_DIR}/array_init.c"
    "${USER_DIR}/array_init.h")
target_link_libraries(DumpPatterns PRIVATE JsonCpp::JsonCpp)
