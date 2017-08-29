cmake_minimum_required(VERSION 3.6)
project(midimap)

# This project uses C11 Standard
set( CMAKE_C_STANDARD 11 )

# Treat all warnings as intense errors.
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wno-unused-function")
add_definitions( -D__STDC_WANT_LIB_EXT1__=1 )

# Glob'n all C source + headers under src/
#
# Below we iterate and remove any tmp files that
# Would otherwise break the Make build system.
file(GLOB_RECURSE SRC_LIB *.c *.h)

# Remove tmp files from source tree
foreach ( PATH ${SRC_LIB} )
  string (FIND ${PATH} "#" FOUND)
  if (NOT ${FOUND} EQUAL -1)
    list ( REMOVE_ITEM SRC_LIB ${PATH} )
  endif()
endforeach( PATH )

# Setup 2 targets for build output.
# 1. A library for ... library reuse.
# 2. A Executable that utilises the library.
add_library(midimap STATIC ${SRC_LIB})

# Break into conditionals for audio driver support
if (APPLE)
  find_library( coreaudio_lib CoreAudio )
  find_library( coremidi_lib CoreMIDI )
  find_library( corefoundation_lib CoreFoundation )
  find_library( audiounit_lib AudioUnit )
  find_library( audiotoolbox_lib AudioToolbox )

  set( _midi_libs
    ${coreaudio_lib}
    ${coremidi_lib}
    ${corefoundation_lib}
    ${audiounit_lib}
    ${audiotoolbox_lib}
    )

  set( _midi_defs __MACOSX_AU__ __MACOSX_CORE__ )

elseif (MSVC)
  set( _midi_libs winmm )
  set( _midi_defs __WINDOWS_MM__ )

elseif (UNIX)
  find_package( ALSA REQUIRED )

  set( _midi_libs ${ALSA_LIBRARY} pthread )
  set( _midi_defs __LINUX_ALSA__ )

endif ()

# Everybody needs curses
find_library( ncurses_lib ncurses )

# Link Library support: libmidimap
target_include_directories( midimap PUBLIC ./ )
target_compile_definitions( midimap PUBLIC ${_midi_defs} ${_app_defs} )
target_link_libraries( midimap m ${_midi_libs} ${ncurses_lib} )

# Cleanup after ourselves.
unset( _midi_libs )
unset( _midi_defs )


