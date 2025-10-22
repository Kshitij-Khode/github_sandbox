cmake_minimum_required(VERSION 3.10)

project(test_project LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)

option(TEST "Build test code" ON)

if(TEST)
    file(GLOB TEST_SOURCE test.cpp)
    message("Building: ${TEST_SOURCE}")
    add_executable(test ${TEST_SOURCE})
endif()