cmake_minimum_required(VERSION 3.6)
project(midimapper)

file(GLOB_RECURSE sources      src/*.c src/*.h)
file(GLOB_RECURSE data resources/*)

add_executable(midimapper ${sources} ${data})

target_include_directories(midimapper PUBLIC src)

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
  # TODO:

elseif (UNIX)
  # TODO:

endif ()



target_link_libraries( midimapper ${_midi_libs} )

unset( _midi_libs )
unset( _midi_defs )

