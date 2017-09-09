cmake_minimum_required(VERSION 3.6)
set (APP_LIB_NAME dimappio)
set (APP_EXE_NAME dmappio)

set (APP_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

project(${APP_LIB_NAME})

set (DM_VERSION_BUILD $ENV{BUILD_REVISION})
set (DM_VERSION_DATE $ENV{BUILD_TIMESTAMP})

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

# This project uses C11 Standard
set( CMAKE_C_STANDARD 11 )

# Treat all warnings as intense errors.
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wno-unused-function")

set( APP_DEFS _DEBUG_ )

set( VENDOR ${APP_ROOT}/vendor)
set( VENDOR_INCLUDES
  ${VENDOR}/ubelt/src)

include_directories(${APP_ROOT}/src ${VENDOR_INCLUDES})
enable_testing ()

add_subdirectory("tests")
add_subdirectory("src/dimappio")
add_subdirectory("src/main")

add_subdirectory("vendor/ubelt")

configure_file (
  "${APP_ROOT}/src/version.h.in"
  "${APP_ROOT}/src/version.h"
)
