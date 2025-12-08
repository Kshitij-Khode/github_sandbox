cmake_minimum_required(VERSION 3.10)

project(test_project LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)

# Add perf optimizations for release build based on string compare
# if (CMAKE_BUILD_TYPE STREQUAL "Release")
#     add_compile_options(/O2 /arch:AVX512 /fp:fast /GL)
# endif()

# Add perf optimizations for release build based on prebuilt string
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /arch:AVX512 /fp:fast /GL")

option(TEST "Build test code" ON)

if(TEST)
    file(GLOB TEST_SOURCE test.cpp)
    message("Building: ${TEST_SOURCE}")
    add_executable(test ${TEST_SOURCE})
endif()
