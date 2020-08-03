# The RtMidi vcpkg is not compiled with winmm support
# (https://github.com/microsoft/vcpkg/issues/6993), so for now just default
# to downloading and building RtMidi directly on Windows.
option( FETCH_RTMIDI "Fetch and build RtMidi." ${PLATFORM_WIN} )

if ( FETCH_RTMIDI )
    include( FetchContent )

    FetchContent_Declare( rtmidi_src
        GIT_REPOSITORY https://github.com/thestk/rtmidi.git
        GIT_TAG 4.0.0
    )
    FetchContent_MakeAvailable( rtmidi_src )

    add_library( rtmidi::rtmidi INTERFACE IMPORTED )
    target_link_libraries( rtmidi::rtmidi INTERFACE rtmidi )
    target_include_directories( rtmidi::rtmidi INTERFACE ${rtmidi_src_SOURCE_DIR} )

else ()
    find_package( rtmidi REQUIRED )

    add_library( rtmidi::rtmidi IMPORTED SHARED )
    target_include_directories( rtmidi::rtmidi INTERFACE ${rtmidi_INCLUDE_DIRS} )

    if ( PLATFORM_LINUX )
        target_link_libraries( rtmidi::rtmidi INTERFACE pthread )
    endif ()

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
endif ()
