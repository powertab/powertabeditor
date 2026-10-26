include( FindPackageHandleStandardArgs )

find_path(
    RTMIDI_INCLUDE_DIR
    NAMES RtMidi.h
    PATH_SUFFIXES rtmidi
)
find_library( RTMIDI_LIBRARY rtmidi )

set( rtmidi_INCLUDE_DIRS ${RTMIDI_INCLUDE_DIR} )
set( rtmidi_LIBRARIES ${RTMIDI_LIBRARY} )
set( rtmidi_VERSION "0.0.0" )

if ( RTMIDI_INCLUDE_DIR AND EXISTS "${RTMIDI_INCLUDE_DIR}/RtMidi.h" )
    foreach ( component MAJOR MINOR PATCH )
        file( STRINGS "${RTMIDI_INCLUDE_DIR}/RtMidi.h" version_line
            REGEX "^#define RTMIDI_VERSION_${component}[ \t]+[0-9]+" )
        string( REGEX MATCH "[0-9]+$" version_component "${version_line}" )
        set( RTMIDI_VERSION_${component} "${version_component}" )
    endforeach ()

    if ( NOT "${RTMIDI_VERSION_MAJOR}" STREQUAL "" AND
         NOT "${RTMIDI_VERSION_MINOR}" STREQUAL "" AND
         NOT "${RTMIDI_VERSION_PATCH}" STREQUAL "" )
        set( rtmidi_VERSION
            "${RTMIDI_VERSION_MAJOR}.${RTMIDI_VERSION_MINOR}.${RTMIDI_VERSION_PATCH}" )
    endif ()
endif ()

find_package_handle_standard_args( rtmidi
    REQUIRED_VARS RTMIDI_INCLUDE_DIR RTMIDI_LIBRARY
    VERSION_VAR rtmidi_VERSION
)

mark_as_advanced( RTMIDI_INCLUDE_DIR RTMIDI_LIBRARY )
