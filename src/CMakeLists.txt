project(jpeg2000_src)

add_subdirectory(dwt)

if(BUILD_EXTERNALS AND EXISTS ${JP3D_PATH})
    add_subdirectory(${JP3D_PATH})
endif()

set(SOURCES
    demo_dwt.cpp
    demo_queue.cpp
    demo_opencv.cpp
    check_entropy.cpp
    arg_parser.cpp
    main.cpp
)

add_executable(jpeg2000 ${SOURCES})

set_target_properties(jpeg2000 PROPERTIES COMPILE_FLAGS "-save-temps")

if(BUILD_EXTERNALS)
    target_include_directories(jpeg2000 SYSTEM PRIVATE
        ${OPENCV_PATH}/modules/core/include
        ${OPENCV_PATH}/modules/imgcodecs/include
        ${OPENCV_PATH}/modules/imgproc/include
        ${CMAKE_BINARY_DIR} # OpenCV is looking for opencv2/opencv_modules.hpp
        ${CXXOPTS_PATH}/include
    )
    target_compile_definitions(jpeg2000 PRIVATE BUILD_OPENCV)
endif()

target_include_directories(jpeg2000 SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${TIMER_PATH}
)

target_link_libraries(jpeg2000 PRIVATE
    dwt
    cxxopts
)

if(BUILD_EXTERNALS)
    target_link_libraries(jpeg2000 PRIVATE
        opencv_core
        opencv_imgcodecs
        opencv_imgproc
    )
    if(EXISTS ${JP3D_PATH})
        target_link_libraries(jpeg2000 PRIVATE jp3d)
    endif()
endif()
