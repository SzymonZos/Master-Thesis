project(jpeg2000_src)

add_subdirectory(dwt)

add_executable(jpeg2000 main.cpp)

if(BUILD_EXTERNALS)
    target_include_directories(jpeg2000 SYSTEM PRIVATE
        ${OPENCV_PATH}/modules/core/include
        ${OPENCV_PATH}/modules/imgcodecs/include
        ${CMAKE_CURRENT_BINARY_DIR} # OpenCV is looking for opencv2/opencv_modules.hpp
    )
endif()

target_include_directories(jpeg2000 SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(jpeg2000 PRIVATE
    dwt
)

if (BUILD_EXTERNALS)
    target_link_libraries(jpeg2000
        opencv_core
        opencv_imgcodecs
        jp3d
    )
endif()
