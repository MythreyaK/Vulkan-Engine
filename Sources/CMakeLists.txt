cmake_minimum_required (VERSION 3.14)

set(SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
configure_file("Version/Version.hpp.in" "Version/Version.hpp" @ONLY NEWLINE_STYLE LF)

add_subdirectory("Render")
add_subdirectory("Window")
add_subdirectory("Logging")
add_subdirectory("Primitives")
add_subdirectory("TestGame")