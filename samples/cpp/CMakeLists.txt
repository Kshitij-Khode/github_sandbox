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
    # Recursively find all .cpp files, excluding build and .venv directories
    file(GLOB_RECURSE ALL_CPP_FILES "*.cpp")
    list(FILTER ALL_CPP_FILES EXCLUDE REGEX "(build/|.venv/)")
    
    # Create a separate executable for each .cpp file
    foreach(cpp_file ${ALL_CPP_FILES})
        get_filename_component(exe_name ${cpp_file} NAME_WE)
        message("Building: ${exe_name} from ${cpp_file}")
        add_executable(${exe_name} ${cpp_file})
    endforeach()
endif()
