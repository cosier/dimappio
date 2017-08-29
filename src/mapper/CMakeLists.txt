cmake_minimum_required(VERSION 3.6)
project(mapper)

# This project uses C11 Standard
set( CMAKE_C_STANDARD 11 )

# Treat all warnings as intense errors.
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")

# Main executable has a single file
SET ( SRC_MAIN main.c )

# Setup 2 targets for build output.
# 1. A library for ... library reuse.
# 2. A Executable that utilises the library.
add_executable(midi-mapper ${SRC_MAIN})

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../ )

# Break into conditionals for audio driver support

# Link Library support: libmidimap
target_link_libraries( midi-mapper midimap )

install(TARGETS midi-mapper
  RUNTIME DESTINATION bin)

