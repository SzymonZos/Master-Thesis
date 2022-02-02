add_subdirectory(dwt)
add_subdirectory(include-test)

add_executable(jpeg2000_ut
    main.cpp
    ${TEST_UT_DWT_SOURCES}
    ${TEST_UT_INCLUDES}
)

set_target_properties(jpeg2000_ut
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_include_directories(jpeg2000_ut PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(jpeg2000_ut PRIVATE dwt)
