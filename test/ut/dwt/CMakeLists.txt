set(TEST_UT_DWT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/dwt_helpers.cpp
    PARENT_SCOPE
)

# Figure out how to use object library instead
# add_library(ut_dwt OBJECT ${TEST_UT_DWT_SOURCES})
