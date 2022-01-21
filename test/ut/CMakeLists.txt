add_subdirectory(dwt)
add_executable(jpeg2000_ut main.cpp ${TEST_UT_DWT_SOURCES})

set_target_properties(jpeg2000_ut
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# As for now MSYS2 does not provide ASAN support apart from clang64 toolchain
# https://github.com/msys2/MINGW-packages/issues/3163
# https://github.com/msys2/MINGW-packages/issues/8552
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(GOS_SANITIZER_OPTIONS -fsanitize=undefined,address)
    target_compile_options(jpeg2000_ut PRIVATE
        "$<$<CONFIG:Debug>:${GOS_SANITIZER_OPTIONS}>"
    )
    target_link_options(jpeg2000_ut PRIVATE
        "$<$<CONFIG:Debug>:${GOS_SANITIZER_OPTIONS}>"
    )
endif()

target_include_directories(jpeg2000_ut PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(jpeg2000_ut PRIVATE dwt)
