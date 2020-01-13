cmake_minimum_required (VERSION 3.14)


set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR})
configure_file("Version.hpp.in" "Version.hpp")

add_subdirectory("Render")
add_subdirectory("Window")

add_executable(Game "game.cpp" "game.hpp")

target_link_libraries(Game PUBLIC WindowLib PUBLIC RenderLib)


