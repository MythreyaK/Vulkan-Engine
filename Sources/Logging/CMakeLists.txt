# A simple logging library
add_library(LoggingLib STATIC "Logger.cpp" "Logger.hpp")

# Not sure if this is needed. Can't seem to be able to include
# this dir without this line in other libs.
target_include_directories(LoggingLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
