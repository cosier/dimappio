cmake_minimum_required(VERSION 3.6)
project(midi-mapper)

# This project uses C11 Standard
set( CMAKE_C_STANDARD 11 )

# Treat all warnings as intense errors.
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")

add_subdirectory("tests")
add_subdirectory("src/midimap")
add_subdirectory("src/mapper")

enable_testing ()

add_test(NAME test-key-nodes COMMAND test-key-nodes)
add_test(NAME test-map-parser COMMAND test-map-parser)
