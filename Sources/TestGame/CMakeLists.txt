cmake_minimum_required (VERSION 3.14)

add_executable(Game "game.cpp" "game.hpp")

target_link_libraries(Game
                    PUBLIC WindowLib
                    PUBLIC RenderLib
                    PUBLIC PrimitivesLib
                    PRIVATE LoggingLib
)

# Enable debug output window in debug builds only
if (NOT BUILD_TYPE_DEBUG AND MSVC)
    set_target_properties(Game PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
endif()
