find_package( rtmidi REQUIRED )

add_library( rtmidi::rtmidi IMPORTED SHARED )
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
