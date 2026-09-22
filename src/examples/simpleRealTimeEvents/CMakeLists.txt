cmake_minimum_required(VERSION 3.10)

# Add the source directory to the include path so that tests can locate headers.
#include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${CMAKE_PROJECT_INCLUDE}/include)

# -----------------------------------------------------------------------------
# DFRApplication Test
# -----------------------------------------------------------------------------
add_executable(simpleRealTimeEvents simpleRealTimeEvents.cpp)
target_link_libraries(simpleRealTimeEvents PRIVATE DFRCore)
##add_test(NAME test_DFRApplication COMMAND test_DFRApplication)
