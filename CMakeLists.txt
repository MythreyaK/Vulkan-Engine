# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.14)

project("VulkanEngine" VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Vulkan REQUIRED FATAL_ERROR)

# This project stuff
# Include sub-projects.
set(SOURCES_SUB_DIR "${PROJECT_SOURCE_DIR}/Sources")

# Dependencies
set(DEPS_DIR "${PROJECT_SOURCE_DIR}/Deps")
# Remove debug console for release builds
string(TOUPPER ${CMAKE_BUILD_TYPE} UC_CMAKE_BUILD_TYPE)
set(BUILD_TYPE_STRING ${CMAKE_BUILD_TYPE})

if (UC_CMAKE_BUILD_TYPE MATCHES "DEBUG")
    set(DEBUG_WINDOW TRUE)
    set(BUILD_TYPE_DEBUG TRUE)
    message(STATUS "${PROJECT_NAME}: Debug window enabled")

elseif(UC_CMAKE_BUILD_TYPE MATCHES "RELEASE")
    set(DEBUG_WINDOW FALSE)
    set(BUILD_TYPE_RELEASE TRUE)

elseif(UC_CMAKE_BUILD_TYPE MATCHES "RELWITHDEBINFO")
    set(DEBUG_WINDOW FALSE)
    set(BUILD_TYPE_RELEASE_DEBUG_INFO TRUE)

endif()

# Deps
add_subdirectory("${DEPS_DIR}")

# This project
add_subdirectory("${SOURCES_SUB_DIR}")
