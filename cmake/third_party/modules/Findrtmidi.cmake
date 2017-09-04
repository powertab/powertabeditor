include( FindPackageHandleStandardArgs )

find_path(
    RTMIDI_INCLUDE_DIR
    NAMES RtMidi.h
    PATH_SUFFIXES rtmidi
)
find_library( RTMIDI_LIBRARY rtmidi )

set( rtmidi_INCLUDE_DIRS ${RTMIDI_INCLUDE_DIR} )
set( rtmidi_LIBRARIES ${RTMIDI_LIBRARY} )

find_package_handle_standard_args( rtmidi DEFAULT_MSG
    RTMIDI_INCLUDE_DIR
    RTMIDI_LIBRARY
)

mark_as_advanced( RTMIDI_INCLUDE_DIR RTMIDI_LIBRARY )
