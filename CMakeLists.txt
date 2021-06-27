cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)
option(BUILD_EXTERNALS OFF)
option(BUILD_TESTS OFF)

set(TOOLCHAIN_BUILD "${CMAKE_CXX_COMPILER_ID}${CMAKE_BUILD_TYPE}")
set(OPENCV_PATH ${CMAKE_SOURCE_DIR}/external/opencv)
set(JP3D_PATH ${CMAKE_SOURCE_DIR}/src/jp3d)
set(PYBIND11_PATH ${CMAKE_SOURCE_DIR}/external/pybind11)
set(TIMER_PATH ${CMAKE_SOURCE_DIR}/external/Timer)
set(CXXOPTS_PATH ${CMAKE_SOURCE_DIR}/external/cxxopts)

# OpenCV junk
set(OPENCV_ENABLE_ALLOCATOR_STATS OFF CACHE BOOL "")
set(BUILD_opencv_gapi OFF CACHE BOOL "")
set(CPU_DISPATCH "" CACHE STRING "")

# cxxopts mess
set(CXXOPTS_BUILD_EXAMPLES OFF CACHE BOOL "")
set(CXXOPTS_BUILD_TESTS OFF CACHE BOOL "")
set(CXXOPTS_ENABLE_INSTALL OFF CACHE BOOL "")

project(jpeg2000)

set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(
        -stdlib=libc++
    )
    add_link_options(
        -stdlib=libc++
        -fuse-ld=lld
        -lc++abi
    )
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_link_options(
        -pthread
    )
endif()

add_subdirectory(src)

if(BUILD_TESTS)
    find_package(Python COMPONENTS Interpreter Development REQUIRED)
    add_subdirectory(${PYBIND11_PATH})
    add_subdirectory(test)
endif()

if(BUILD_EXTERNALS)
    add_subdirectory(${OPENCV_PATH})
endif()

add_subdirectory(${CXXOPTS_PATH})
