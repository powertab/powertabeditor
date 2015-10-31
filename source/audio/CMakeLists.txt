project( pteaudio )

set( srcs
    midioutputdevice.cpp
    midiplayer.cpp
)

set( headers
    midioutputdevice.h
    midiplayer.h
)

set( moc_headers
    midiplayer.h
)

pte_library(
    NAME pteaudio
    SOURCES ${srcs}
    HEADERS ${headers}
    MOC_HEADERS ${moc_headers}
    DEPENDS
        ptescore
        Qt5::Core
        rtmidi
)
