cmake_minimum_required(VERSION 3.6)
project(midi-mapper)

set(CMAKE_INSTALL_MESSAGE LAZY)

# Main executable has a single file
SET ( SRC_MAIN src/main.c )

file(GLOB_RECURSE SRC_LIB      src/*.c src/*.h)

# Remove tmp files from source tree
foreach ( PATH ${SRC_LIB} )
  string (FIND ${PATH} "#" FOUND)
  if (NOT ${FOUND} EQUAL -1)
    list ( REMOVE_ITEM SRC_LIB ${PATH} )
  endif()
endforeach( PATH )


add_library(midimap ${SRC_LIB})
add_executable(midi-mapper ${SRC_MAIN})

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

# Link up our two targets (main exe and library support)
target_include_directories( midimap PUBLIC src)
target_compile_definitions( midimap PUBLIC ${_midi_defs} )
target_link_libraries( midimap ${_midi_libs} ${ncurses_lib} )

target_include_directories( midi-mapper PUBLIC src )
target_link_libraries( midi-mapper midimap )


unset( _midi_libs )
unset( _midi_defs )


