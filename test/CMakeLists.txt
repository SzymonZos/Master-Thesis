cmake_minimum_required(VERSION 3.17)
project(jpeg2000_test)

pybind11_add_module(jpeg2000_test main.cpp)
target_link_libraries(jpeg2000_test PRIVATE dwt)
install(TARGETS jpeg2000_test
    LIBRARY
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}
)
