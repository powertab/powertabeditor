# Determine the platform-specific libraries to link against.
if ( PLATFORM_WIN )
    set( _midi_libs winmm )
    set( _midi_defs __WINDOWS_MM__ )
elseif ( PLATFORM_OSX )
    find_library( coreaudio_lib CoreAudio )
    find_library( coremidi_lib CoreMIDI )
    find_library( corefoundation_lib CoreFoundation )
    find_library( audiotoolbox_lib AudioToolbox )
    find_library( audiounit_lib AudioUnit )

    set( _midi_libs
        ${coreaudio_lib}
        ${coremidi_lib}
        ${corefoundation_lib}
        ${audiotoolbox_lib}
        ${audiounit_lib}
    )
    set( _midi_defs __MACOSX_AU__ __MACOSX_CORE__ )
elseif ( PLATFORM_LINUX )
    find_package( ALSA REQUIRED )

    set( _midi_libs ${ALSA_LIBRARY} pthread )
    set( _midi_defs __LINUX_ALSA__ )
endif ()

find_package( rtmidi REQUIRED )

add_library( rtmidi::rtmidi IMPORTED SHARED )
target_link_libraries( rtmidi::rtmidi INTERFACE ${_midi_libs} )
target_compile_definitions( rtmidi::rtmidi INTERFACE ${_midi_defs} )
target_include_directories( rtmidi::rtmidi INTERFACE ${rtmidi_INCLUDE_DIRS} )

if ( PLATFORM_WIN )
    set_target_properties(
        rtmidi::rtmidi PROPERTIES
        IMPORTED_IMPLIB ${rtmidi_LIBRARIES}
        IMPORTED_LOCATION ${rtmidi_LIBRARIES}
    )
else ()
    set_target_properties(
        rtmidi::rtmidi PROPERTIES
        IMPORTED_LOCATION ${rtmidi_LIBRARIES}
    )
endif ()

unset( _midi_libs )
unset( _midi_defs )
