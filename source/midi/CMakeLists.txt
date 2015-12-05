project ( ptemidi )

set( srcs
    midievent.cpp
    midieventlist.cpp
    midifile.cpp
    repeatcontroller.cpp
)

set( headers
    midievent.h
    midieventlist.h
    midifile.h
    repeatcontroller.h
)

pte_library(
    NAME ptemidi
    SOURCES ${srcs}
    HEADERS ${headers}
    DEPENDS
        ptescore
)
