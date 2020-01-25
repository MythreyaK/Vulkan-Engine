cmake_minimum_required (VERSION 3.14)

file(GLOB_RECURSE PRIMITIVES_HPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.hpp)
file(GLOB_RECURSE PRIMITIVES_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.cpp)


add_library(PrimitivesLib STATIC ${PRIMITIVES_CPP} ${PRIMITIVES_HPP})

target_include_directories(PrimitivesLib
                    PUBLIC glm::glm
                    PUBLIC "${PROJECT_BINARY_DIR}/Sources/Version"
                    PUBLIC "${SOURCES_DIR}/Render/VKinclude"
                    PUBLIC "${Vulkan_INCLUDE_DIR}"
)

target_link_libraries(PrimitivesLib PUBLIC glm::glm)
