cmake_minimum_required(VERSION 3.6)
project(midimap-tests)

# This project uses C11 Standard
set( CMAKE_C_STANDARD 11 )

# Treat all warnings as intense errors.
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")

# Test Executable that utilises the library.
add_executable(test-key-nodes key_nodes.c)
add_executable(test-map-parser map_parser.c)

include_directories("${CMAKE_SOURCE_DIR}/src")

# Link up main executable with our Library: libmidimap
target_link_libraries( test-key-nodes midimap )
target_link_libraries( test-map-parser midimap )

