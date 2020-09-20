cmake_minimum_required(VERSION 3.17)
project(jpeg2000)

set(CMAKE_CXX_STANDARD 20)

set(TOOLCHAIN_BUILD "${CMAKE_CXX_COMPILER_ID}${CMAKE_BUILD_TYPE}")

add_compile_options(
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

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(
        -stdlib=libc++
        -fno-exceptions
    )
    add_link_options(
        -stdlib=libc++
        -fuse-ld=lld
        -lc++abi
    )
endif ()

include_directories(SYSTEM
    include
)

set(SOURCE_FILES src/main.cpp)
add_executable(jpeg2000 ${SOURCE_FILES})
