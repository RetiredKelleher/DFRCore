cmake_minimum_required(VERSION 3.10)
project(DFRCoreLib VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing and add the tests subdirectory
enable_testing()
add_subdirectory(tests)

###############################################################################
###############################################################################
# Test the Package Creation Steps
###############################################################################
###############################################################################

set(CPACK_PACKAGE_DESCRIPTION "Simple base simulation framework C++ library packaged with CMake and CPack.")
set(CPACK_PACKAGE_VENDOR "Thomas Kelleher")
set(CPACK_DEBIAN_PACKAGE_SECTION "libs")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})

#########
# Step 1: Define Install Rules
#########

include(GNUInstallDirs)

add_library(DFRCore
   src/core/Application.cpp src/core/Logger.cpp
   src/core/EventManager.cpp src/core/RandomNumber.cpp
   src/core/SimClock.cpp src/core/RealTimeSimClock.cpp
   src/core/PriorityQueueEventManager.cpp
   src/core/SimEngine.cpp)

target_include_directories(DFRCore
   PUBLIC
       $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
       $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Optional: Set install permissions
set(CPACK_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
)

install(TARGETS DFRCore
   EXPORT DFRCoreTargets
   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
   RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#   PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
   FILES_MATCHING PATTERN "*.hpp"
)

## Build the examples for testing
add_subdirectory(src/examples/simpleRealTimeEvents)

#########
# Step 2: Export Targets and Create Config Files
#########

install(EXPORT DFRCoreTargets
   FILE DFRCoreTargets.cmake
   NAMESPACE DFRCore::
   DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DFRCore
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
   cmake/DFRCoreConfig.cmake.in
   DFRCoreConfig.cmake
   INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DFRCore
)
write_basic_package_version_file(
   DFRCoreConfigVersion.cmake
   VERSION ${PROJECT_VERSION}
   COMPATIBILITY SameMajorVersion
)

install(FILES
   "${CMAKE_CURRENT_BINARY_DIR}/DFRCoreConfig.cmake"
   "${CMAKE_CURRENT_BINARY_DIR}/DFRCoreConfigVersion.cmake"
   DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DFRCore
)

#########
# Step 3: Enable CPack for Bins and Source packages
#########

set(CPACK_PACKAGE_CONTACT "Thomas Kelleher <you@example.com>")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Thomas Kelleher") # Required
set(CPACK_PACKAGE_NAME "DFRCore")
set(CPACK_PACKAGE_VERSION "0.0.1")
#set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "DEB")

include(CPack)

# old cpack var sets

# Dependencies (optional)
#set(CPACK_DEBIAN_PACKAGE_DEPENDS "libstdc++6 (>= 9)")

# License file (optional)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

# Ensure versioned package filename
#set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${CMAKE_SYSTEM_PROCESSOR}")

# Optionally generate Doxygen documentation
find_package(Doxygen)
include(cmake/doxygen.cmake)

###############################################################################
###############################################################################
# Final Hand Steps in the Make process
###############################################################################
###############################################################################

# Step 1: Building the Docs (from build directory)
#  cmake --build . --target doxygen
#
# NOTE: First time only (at DFRCore directory)
#  doxygen -g Doxyfile

# Step 2: Build a package to deploy (from top directory DFRCore)
#  cmake -B build -DCMAKE_INSTALL_PREFIX=install_dir
#  cmake --build build --target install
#  fakeroot cpack --config build/CPackConfig.cmake