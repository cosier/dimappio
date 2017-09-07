cmake_minimum_required(VERSION 3.6)
project(dimappio)

set (DM_VERSION_MAJOR 0)
set (DM_VERSION_MINOR 2)
set (DM_VERSION_PATCH 0)

if ("$ENV{BUILD_REVISION}" STREQUAL "")
  message (FATAL_ERROR "BUILD_REVISION not specified in ENV")
endif()

if ("$ENV{BUILD_TIMESTAMP}" STREQUAL "")
  message (FATAL_ERROR "BUILD_TIMESTAMP not specified in ENV")
endif()


if ("$ENV{BUILD_STATIC}" STREQUAL "true")
  set (BUILD_SHARED_LIBS OFF)
  set (CMAKE_EXE_LINKER_FLAGS "-static")
endif()


set (DM_VERSION_BUILD $ENV{BUILD_REVISION})
set (DM_VERSION_DATE $ENV{BUILD_TIMESTAMP})

set (DM_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# This project uses C11 Standard
set( CMAKE_C_STANDARD 11 )

# Treat all warnings as intense errors.
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wno-unused-function")

add_subdirectory("tests")
add_subdirectory("src/dimappio")
add_subdirectory("src/mapper")

enable_testing ()

add_test(NAME test-utils COMMAND test-utils)
add_test(NAME test-key-maps COMMAND test-key-maps)
add_test(NAME test-key-nodes COMMAND test-key-nodes)
add_test(NAME test-note-parser COMMAND test-note-parser)


configure_file (
  "${PROJECT_SOURCE_DIR}/src/version.h.in"
  "${PROJECT_BINARY_DIR}/version.h"
)

include_directories(
  "${CMAKE_SOURCE_DIR}/src"
  "${PROJECT_BINARY_DIR}"
)
