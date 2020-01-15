# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.14)

project("VulkanEngine" VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Dependencies
# Add GLFW options 
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HYBRID_HPG OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE     OFF CACHE BOOL "" FORCE)

find_package(Vulkan REQUIRED FATAL_ERROR)

# This project stuff
# Include sub-projects.
set(DEPS_DIR        "${PROJECT_SOURCE_DIR}/Deps")
set(SOURCES_SUB_DIR "${PROJECT_SOURCE_DIR}/Sources")

add_subdirectory("${DEPS_DIR}/glfw")
add_subdirectory("${DEPS_DIR}/glm")

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


add_subdirectory ("Sources")
