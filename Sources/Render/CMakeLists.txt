cmake_minimum_required (VERSION 3.14)

file(GLOB_RECURSE RENDER_HPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.hpp)
file(GLOB_RECURSE RENDER_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.cpp)


add_library(RenderLib STATIC ${RENDER_CPP} ${RENDER_HPP} ${VERSION_SOURCE})

target_include_directories(RenderLib
                        PUBLIC  "${Vulkan_INCLUDE_DIR}"
                        PUBLIC  "${PROJECT_BINARY_DIR}/Sources/Version"
                        PUBLIC  "${SOURCES_SUB_DIR}/Primitives"
                        PUBLIC  "${DEPS_DIR}/glm"
                        PRIVATE  "${CMAKE_CURRENT_SOURCE_DIR}"
                        PRIVATE "${SOURCES_SUB_DIR}/Logging"
)

target_link_libraries(RenderLib PUBLIC Vulkan::Vulkan)
