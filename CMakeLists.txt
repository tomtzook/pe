cmake_minimum_required(VERSION 3.17)
project(pe)

set(CMAKE_CXX_STANDARD 17)

add_library(pe STATIC
        src/pe.cpp
        src/section.cpp
        src/export.cpp src/debug.cpp src/pe_base.cpp)
target_include_directories(pe PRIVATE include)


add_subdirectory(examples)

add_executable(pe_test test.cpp)
target_include_directories(pe_test PRIVATE include)
target_link_libraries(pe_test pe)
