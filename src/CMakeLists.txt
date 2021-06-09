project(jpeg2000_src)

add_subdirectory(dwt)

set(SOURCES
    demo_dwt.cpp
    demo_queue.cpp
    arg_parser.cpp
    main.cpp
)

add_executable(jpeg2000 ${SOURCES})

set_target_properties(jpeg2000 PROPERTIES COMPILE_FLAGS "-save-temps")

if(BUILD_EXTERNALS)
    target_include_directories(jpeg2000 SYSTEM PRIVATE
        ${OPENCV_PATH}/modules/core/include
        ${OPENCV_PATH}/modules/imgcodecs/include
        ${CMAKE_CURRENT_BINARY_DIR} # OpenCV is looking for opencv2/opencv_modules.hpp
    )
endif()

target_include_directories(jpeg2000 SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${TIMER_PATH}
)

target_link_libraries(jpeg2000 PRIVATE
    dwt
    cxxopts
)

if (BUILD_EXTERNALS)
    target_link_libraries(jpeg2000
        opencv_core
        opencv_imgcodecs
        jp3d
    )
endif()
