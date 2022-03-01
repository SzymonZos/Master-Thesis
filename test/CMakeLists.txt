cmake_minimum_required(VERSION 3.17)
project(jpeg2000_test)

add_subdirectory(ut)

pybind11_add_module(jpeg2000_test
    dwt.cpp
    dwt_2d_f.cpp
    dwt_2d_d.cpp
    utils.cpp
)

if(USE_SANITIZERS)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(LIB_ASAN libclang_rt.asan-x86_64.so)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(LIB_ASAN libasan.so)
    endif()
    execute_process(
        COMMAND bash -c "${CMAKE_C_COMPILER} -print-file-name=${LIB_ASAN}"
        OUTPUT_VARIABLE LIB_ASAN
    )
    # TODO: Rewrite it in clean way
    set(ASAN_CONFIG ${CMAKE_BINARY_DIR}/export_asan_config.sh)
    set(LSAN_CONFIG ${CMAKE_BINARY_DIR}/lsan_suppresions.txt)
    # It looks like a false positive in python interpreter
    # __interceptor_malloc is used in the gcc build while clang uses malloc
    # I think suppressing just malloc is bad idea so instead python3.8 is chosen
    # Some real errors can be silenced as LeakSanitizer does not seem to support regex
    file(WRITE ${LSAN_CONFIG} "leak:__interceptor_malloc\n")
    file(APPEND ${LSAN_CONFIG} "leak:python3.8")
    file(WRITE ${ASAN_CONFIG} "#!/usr/bin/env bash\n")
    file(APPEND ${ASAN_CONFIG} "set -Eeuo pipefail\n")
    file(APPEND ${ASAN_CONFIG} "export LD_PRELOAD=${LIB_ASAN}")
    file(APPEND ${ASAN_CONFIG} "export PYTHONMALLOC=malloc\n")
    file(APPEND ${ASAN_CONFIG} "export LSAN_OPTIONS=suppressions=${LSAN_CONFIG}")
endif()

target_link_options(jpeg2000_test PRIVATE
    "$<$<AND:$<BOOL:USE_SANITIZERS>,$<CXX_COMPILER_ID:Clang>>:-shared-libasan>"
)

target_include_directories(jpeg2000_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(jpeg2000_test PRIVATE dwt)
install(TARGETS jpeg2000_test
    LIBRARY
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}
)
