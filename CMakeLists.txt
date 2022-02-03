cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)
option(BUILD_OPENCV OFF)
option(BUILD_TESTS OFF)
option(USE_LIBCPP OFF)

set(OPENCV_PATH ${CMAKE_SOURCE_DIR}/external/opencv)
set(JP3D_PATH ${CMAKE_SOURCE_DIR}/src/jp3d)
set(PYBIND11_PATH ${CMAKE_SOURCE_DIR}/external/pybind11)
set(TIMER_PATH ${CMAKE_SOURCE_DIR}/external/Timer)
set(CXXOPTS_PATH ${CMAKE_SOURCE_DIR}/external/cxxopts)

if(BUILD_OPENCV)
    set(OPENCV_ENABLE_ALLOCATOR_STATS OFF CACHE BOOL "")
    set(BUILD_opencv_gapi OFF CACHE BOOL "")
    set(CPU_DISPATCH "" CACHE STRING "")
endif()

# cxxopts mess
set(CXXOPTS_BUILD_EXAMPLES OFF CACHE BOOL "")
set(CXXOPTS_BUILD_TESTS OFF CACHE BOOL "")
set(CXXOPTS_ENABLE_INSTALL OFF CACHE BOOL "")
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)

project(jpeg2000)

if(USE_LIBCPP AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
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

find_package(TBB 2021.5 REQUIRED)
if(BUILD_OPENCV)
    add_subdirectory(${OPENCV_PATH})
else()
    find_package(OpenCV 4.2 REQUIRED)
endif()

add_subdirectory(src)
add_subdirectory(${CXXOPTS_PATH})

if(BUILD_TESTS)
    find_package(Python COMPONENTS Interpreter Development REQUIRED)
    add_subdirectory(${PYBIND11_PATH})
    add_subdirectory(test)
endif()
