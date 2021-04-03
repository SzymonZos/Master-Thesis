cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)
option(BUILD_EXTERNALS OFF)

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

project(jpeg2000)

if(BUILD_EXTERNALS)
    add_subdirectory(${OPENCV_PATH})
    add_subdirectory(src/jp3d)
endif()

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "src/*.cpp")
#file(GLOB_RECURSE INCLUDES CONFIGURE_DEPENDS "include/*.hpp")
add_executable(jpeg2000 ${SRC})

if(BUILD_EXTERNALS)
    target_include_directories(jpeg2000 SYSTEM PRIVATE
        ${OPENCV_PATH}/modules/core/include
        ${OPENCV_PATH}/modules/imgcodecs/include
        ${CMAKE_CURRENT_BINARY_DIR} # OpenCV is looking for opencv2/opencv_modules.hpp
    )
endif()

target_include_directories(jpeg2000 SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
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
    -Wno-float-conversion
    -Wno-implicit-float-conversion
    -fconstexpr-depth=1024
)

if (BUILD_EXTERNALS)
    target_link_libraries(jpeg2000
        opencv_core
        opencv_imgcodecs
        jp3d
    )
endif()
