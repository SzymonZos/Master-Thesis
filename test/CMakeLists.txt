cmake_minimum_required(VERSION 3.17)
project(jpeg2000_test)

add_subdirectory(ut)

pybind11_add_module(jpeg2000_test
    dwt.cpp
    dwt_2d_f.cpp
    dwt_2d_d.cpp
    utils.cpp
)

target_include_directories(jpeg2000_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(jpeg2000_test PRIVATE dwt)
install(TARGETS jpeg2000_test
    LIBRARY
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}
)
