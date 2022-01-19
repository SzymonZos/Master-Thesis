project(jpeg2000_src)

add_subdirectory(dwt)

if(EXISTS ${JP3D_PATH})
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

target_include_directories(jpeg2000 SYSTEM PRIVATE ${CXXOPTS_PATH}/include)

if(BUILD_OPENCV)
    target_include_directories(jpeg2000 SYSTEM PRIVATE
        ${OPENCV_PATH}/modules/core/include
        ${OPENCV_PATH}/modules/imgcodecs/include
        ${OPENCV_PATH}/modules/imgproc/include
        ${CMAKE_BINARY_DIR} # OpenCV is looking for opencv2/opencv_modules.hpp
    )
else()
    target_include_directories(jpeg2000 SYSTEM PRIVATE
        ${OpenCV_INCLUDE_DIRS}
    )
endif()

target_include_directories(jpeg2000 SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${TIMER_PATH}
)

target_link_libraries(jpeg2000 PRIVATE
    dwt
    cxxopts
    opencv_core
    opencv_imgcodecs
    opencv_imgproc
    TBB::tbb
)

if(EXISTS ${JP3D_PATH})
    target_link_libraries(jpeg2000 PRIVATE jp3d)
endif()

set_target_properties(jpeg2000
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
