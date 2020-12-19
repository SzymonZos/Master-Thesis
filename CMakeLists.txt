cmake_minimum_required(VERSION 3.17)
project(jpeg2000)

set(CMAKE_CXX_STANDARD 20)

set(TOOLCHAIN_BUILD "${CMAKE_CXX_COMPILER_ID}${CMAKE_BUILD_TYPE}")
set(OPENCV_PATH external/opencv)

# OpenCV junk
set(OPENCV_ENABLE_ALLOCATOR_STATS OFF CACHE BOOL "")
set(BUILD_opencv_gapi OFF CACHE BOOL "")
set(CPU_DISPATCH "" CACHE STRING "")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(
        -stdlib=libc++
        -fno-exceptions
    )
    add_link_options(
        -stdlib=libc++
        -fuse-ld=lld
        -lc++abi
    )
endif()

add_subdirectory(${OPENCV_PATH})

set(SOURCE_FILES src/main.cpp)
add_executable(jpeg2000 ${SOURCE_FILES})

include_directories(jpeg2000 SYSTEM PRIVATE
    ${OPENCV_PATH}/modules/core/include
    ${OPENCV_PATH}/modules/imgcodecs/include
    ${CMAKE_CURRENT_BINARY_DIR} # OpenCV is looking for opencv2/opencv_modules.hpp
)

include_directories(jpeg2000 PRIVATE
    include
)

target_compile_options(jpeg2000 PRIVATE
    -Wall
    -Wextra
    -Wshadow # Warn if variable overshadows parent context
    -Wnon-virtual-dtor # Warn if class with virtual func has no virtual dtor
    -Wcast-align # Warn for potential performance problem casts
    -Wunused # Warn on anything being unused
    -Wno-overloaded-virtual # Don't warn if overload a virtual function
    -Wpedantic # Warn if non-standard C++ is used
    -Wconversion # Warn on type conversions that may lose data
    -Wsign-conversion # Warn on sign conversions
    -Wdouble-promotion # Warn if float is implicit promoted to double
    -Wold-style-cast # Warn if c style cast is performed
    -fconstexpr-depth=1024
)

target_link_libraries(jpeg2000
    opencv_core
    opencv_imgcodecs
)
