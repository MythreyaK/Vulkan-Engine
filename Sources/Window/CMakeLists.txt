# A simple glfw wrapper for convenience
add_library(WindowLib STATIC "GLFW.cpp" "GLFW.hpp")

target_include_directories(WindowLib
                        PRIVATE "${DEPS_DIR}/glfw/include"
)

target_link_libraries(WindowLib
                    PRIVATE glfw
)
