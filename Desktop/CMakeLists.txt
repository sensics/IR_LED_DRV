cmake_minimum_required(VERSION 3.1.0)
project(OSVR-IR-Driver-Desktop)

# Get me C++11!
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(JsonCpp REQUIRED)
find_package(Eigen3)

set(USER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../User")
include_directories("${USER_DIR}")

add_executable(DumpPatterns
    DumpPatterns.cpp
    PatternString.h
    BeaconOrder.h
    "${USER_DIR}/array_init.c"
    "${USER_DIR}/array_init.h")
target_link_libraries(DumpPatterns PRIVATE JsonCpp::JsonCpp)

add_executable(DumpSPI
    DumpSPI.cpp
    PatternString.h
    "${USER_DIR}/array_init.c"
    "${USER_DIR}/array_init.h")

add_executable(MatchPatterns
    MatchPatterns.cpp
    Patterns.h
    PatternString.h
    "${USER_DIR}/array_init.c"
    "${USER_DIR}/array_init.h")

add_executable(GenerateMask
    GenerateMask.cpp
    BeaconOrder.h
    "${USER_DIR}/array_init.c"
    "${USER_DIR}/array_init.h")

if(EIGEN3_FOUND)
    add_executable(BrightNeighbors
        BrightNeighbors.cpp
        Patterns.h
        Positions.h)
    target_include_directories(BrightNeighbors PRIVATE ${EIGEN3_INCLUDE_DIR})
endif()
