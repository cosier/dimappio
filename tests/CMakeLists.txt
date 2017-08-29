cmake_minimum_required(VERSION 3.6)
project(midimap-tests)

# This project uses C11 Standard
set( CMAKE_C_STANDARD 11 )

# Treat all warnings as intense errors.
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")

file(GLOB_RECURSE HELPER_LIB helpers/*.c helpers/*.h)

# message ( FATAL_ERROR "HELPER_SRC: ${HELPER_LIB}" )
include_directories(
  "${CMAKE_SOURCE_DIR}/tests"
  "${CMAKE_SOURCE_DIR}/src"
  "${CMAKE_SOURCE_DIR}/src/midimap")

add_library(test_helper STATIC ${HELPER_LIB})
# Test Executable that utilises the library.

add_executable(test-key-nodes  key_nodes.c)
add_executable(test-map-parser map_parser.c)
add_executable(test-note-parser note_parser.c)


# Link up main executable with our Library: libmidimap
target_link_libraries( test-key-nodes midimap test_helper)
target_link_libraries( test-map-parser midimap )
target_link_libraries( test-note-parser midimap )

