cmake_minimum_required(VERSION 3.6)
project(dimappio-tests)

# This project uses C11 Standard
set( CMAKE_C_STANDARD 11 )

# Treat all warnings as intense errors.
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")

file(GLOB_RECURSE HELPER_LIB helpers/*.c helpers/*.h)

# message ( FATAL_ERROR "HELPER_SRC: ${HELPER_LIB}" )
include_directories(
  "${DM_ROOT}/tests"
  "${DM_ROOT}/src")

add_library(test_helper STATIC ${HELPER_LIB})
# Test Executable that utilises the library.

add_executable(test-utils test_utils.c)
add_executable(test-key-maps test_key_maps.c)
add_executable(test-key-nodes test_key_nodes.c)
add_executable(test-note-parser test_note_parser.c)

# Link up main executable with our Library: libdimappio
target_link_libraries( test-utils dimappio test_helper)
target_link_libraries( test-key-maps dimappio test_helper)
target_link_libraries( test-key-nodes dimappio test_helper)
target_link_libraries( test-note-parser dimappio test_helper)

