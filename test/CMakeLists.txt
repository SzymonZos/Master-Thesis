cmake_minimum_required(VERSION 3.17)
project(jpeg2000_test)

pybind11_add_module(jpeg2000_test
    dwt.cpp
    dwt_2d.cpp
    utils.cpp
)

target_include_directories(jpeg2000_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_options(jpeg2000_test PRIVATE
        -fPIC
    )
endif()

target_link_libraries(jpeg2000_test PRIVATE dwt)
install(TARGETS jpeg2000_test
    LIBRARY
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}
)
