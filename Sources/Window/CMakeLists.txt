# A simple glfw wrapper for convenience
add_library(WindowLib STATIC "GLFW.cpp" "GLFW.hpp")

target_include_directories(WindowLib
                        PRIVATE "${DEPS_DIR}/glfw/include"
                        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(WindowLib
                    PRIVATE glfw
)
