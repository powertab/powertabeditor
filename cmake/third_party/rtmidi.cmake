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

find_package( rtmidi )

if ( RTMIDI_FOUND )
    add_library( rtmidi IMPORTED SHARED )

    set_target_properties(
        rtmidi PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${rtmidi_INCLUDE_DIRS}
        INTERFACE_COMPILE_DEFINITIONS ${_midi_defs}
        IMPORTED_LOCATION ${rtmidi_LIBRARIES}
        INTERFACE_LINK_LIBRARIES "${_midi_libs}"
    )
else ()
    set( _src_dir ${PTE_EXTERNAL_DIR}/rtmidi )
    message( STATUS "Using RtMidi library from ${_src_dir}" )

    add_library( rtmidi
        ${_src_dir}/RtMidi.cpp
        ${_src_dir}/RtMidi.h
    )

    target_link_libraries( rtmidi ${_midi_libs} )
    target_compile_definitions( rtmidi PUBLIC ${_midi_defs} )
    target_include_directories( rtmidi INTERFACE ${_src_dir} )

    # Set folder name for Visual Studio projects.
    set_target_properties(
        rtmidi PROPERTIES
        FOLDER ${PTE_EXTERNAL_FOLDER_NAME}
    )

    unset( _src_dir )
endif ()

unset( _midi_libs )
unset( _midi_defs )
