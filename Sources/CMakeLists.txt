cmake_minimum_required (VERSION 3.14)


set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR})
configure_file("Version.hpp.in" "Version.hpp" @ONLY NEWLINE_STYLE LF)

add_subdirectory("Render")
add_subdirectory("Window")
add_subdirectory("Logging")

add_executable(Game "game.cpp" "game.hpp")

target_link_libraries(Game
                    PUBLIC WindowLib
                    PUBLIC RenderLib
                    PRIVATE LoggingLib
)

# Enable debug output window in debug builds only
if (NOT BUILD_TYPE_DEBUG AND MSVC)
    set_target_properties(Game PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
endif()
