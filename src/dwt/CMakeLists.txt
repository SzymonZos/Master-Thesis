add_library(dwt STATIC
    dwt.cpp
    dwt_2d.cpp
)

target_compile_options(dwt PUBLIC
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

if((NOT CMAKE_SYSTEM_NAME STREQUAL "Windows") AND BUILD_TESTS)
    target_compile_options(dwt PRIVATE
        -fPIC
    )
endif()

target_include_directories(dwt PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
