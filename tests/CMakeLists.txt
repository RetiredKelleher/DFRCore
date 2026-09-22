cmake_minimum_required(VERSION 3.10)

# Add the source directory to the include path so that tests can locate headers.
#include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include)

# -----------------------------------------------------------------------------
# DFRApplication Test
# -----------------------------------------------------------------------------
add_executable(test_DFRApplication testDFRApplication.cpp)
target_link_libraries(test_DFRApplication PRIVATE DFRCore)
add_test(NAME test_DFRApplication COMMAND test_DFRApplication)



