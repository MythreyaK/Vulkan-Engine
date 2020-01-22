cmake_minimum_required (VERSION 3.14)

# Dependencies
# Add GLFW options 
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HYBRID_HPG OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE     OFF CACHE BOOL "" FORCE)

# God need to supress warnings from these
add_subdirectory("${DEPS_DIR}/glfw")
add_subdirectory("${DEPS_DIR}/glm")
target_compile_options(glfw PRIVATE "/W0")
target_compile_options(glm INTERFACE "/W0")
# project(Deps)

# set(${PROJECT_NAME}_CXX_FLAGS ${CMAKE_CXX_FLAGS})
# string(REPLACE "/W3" "/W0" ${PROJECT_NAME}_CXX_FLAGS ${${PROJECT_NAME}_CXX_FLAGS})
# string(REPLACE "-Wall" "" ${PROJECT_NAME}_CXX_FLAGS  ${${PROJECT_NAME}_CXX_FLAGS})

# Add this later -analyzer-disable-all-checks
# message("Deps Compile flags: " ${${PROJECT_NAME}_CXX_FLAGS})


