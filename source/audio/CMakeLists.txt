project( pteaudio )

set( srcs
    midioutputdevice.cpp
    midiplayer.cpp
    settings.cpp
)

set( headers
    midioutputdevice.h
    midiplayer.h
    settings.h
)

set( moc_headers
    midiplayer.h
)

set( platform_deps )
if ( PLATFORM_OSX )
    find_library( coremidi_lib CoreMIDI )
    find_library( audiotoolbox_lib AudioToolbox REQUIRED )
    find_library( audiounit_lib AudioUnit REQUIRED )
    set( platform_deps
        ${coremidi_lib}
        ${audiotoolbox_lib}
        ${audiounit_lib}
    )

    list( APPEND srcs
        midisoftwaresynth.cpp
    )
    list( APPEND headers
        midisoftwaresynth.h
    )
endif ()

pte_library(
    NAME pteaudio
    SOURCES ${srcs}
    HEADERS ${headers}
    MOC_HEADERS ${moc_headers}
    DEPENDS
        PUBLIC
            ptescore
            Qt5::Core
        PRIVATE
            rtmidi::rtmidi
            ${platform_deps}
)

# For the snap package, enable info about resolving MIDI device access issues.
if ( PTE_BUILD_SNAP )
    target_compile_definitions( pteaudio PRIVATE
        PTE_BUILDING_FOR_SNAP
    )
endif ()
