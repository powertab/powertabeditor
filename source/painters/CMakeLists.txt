project( ptepainters )

set( srcs
    antialiasedpathitem.cpp
    barlinepainter.cpp
    beamgroup.cpp
    caretpainter.cpp
    chorddiagrampainter.cpp
    clickableitem.cpp
    directions.cpp
    keysignaturepainter.cpp
    layoutinfo.cpp
    musicfont.cpp
    notestem.cpp
    scoreinforenderer.cpp
    simpletextitem.cpp
    staffpainter.cpp
    stdnotationnote.cpp
    styles.cpp
    systemrenderer.cpp
    timesignaturepainter.cpp
    verticallayout.cpp
)

set( headers
    antialiasedpathitem.h
    barlinepainter.h
    beamgroup.h
    caretpainter.h
    chorddiagrampainter.h
    clickableitem.h
    keysignaturepainter.h
    layoutinfo.h
    musicfont.h
    notestem.h
    scoreclickevent.h
    scoreinforenderer.h
    simpletextitem.h
    staffpainter.h
    stdnotationnote.h
    styles.h
    systemrenderer.h
    timesignaturepainter.h
    verticallayout.h
)

pte_library(
    NAME ptepainters
    SOURCES ${srcs}
    HEADERS ${headers}
    DEPENDS
        ptescore
        Qt5::Widgets
)
