cmake_minimum_required(VERSION 3.17)
project(pe)

set(CMAKE_CXX_STANDARD 20)

set(COMPILE_DEFS )
if (DEFINED PE_NO_STD)
    set(COMPILE_DEPS ${COMPILE_DEPS} PE_NO_STD=1)
endif ()

add_library(pe STATIC
        src/pe.cpp
        src/section.cpp
        src/export.cpp
        src/pe_base.cpp
        src/import.cpp
        src/exception.cpp
        src/debug.cpp)
target_include_directories(pe
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_definitions(pe PUBLIC ${COMPILE_DEPS})
target_compile_options(pe PRIVATE -fno-exceptions)
#target_link_options(pe PRIVATE -nostdlib -nolibc -nodefaultlibs)

add_executable(pe_test test.cpp)
target_include_directories(pe_test PRIVATE include)
target_link_libraries(pe_test pe)

if (DEFINED PE_VIEWER)
    add_subdirectory(viewer)
endif ()
