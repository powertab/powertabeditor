project( ptepainters )

set( srcs
    antialiasedpathitem.cpp
    barlinepainter.cpp
    beamgroup.cpp
    caretpainter.cpp
    clickablegroup.cpp
    directions.cpp
    keysignaturepainter.cpp
    layoutinfo.cpp
    musicfont.cpp
    notestem.cpp
    scoreinforenderer.cpp
    simpletextitem.cpp
    staffpainter.cpp
    stdnotationnote.cpp
    systemrenderer.cpp
    timesignaturepainter.cpp
    verticallayout.cpp
)

set( headers
    antialiasedpathitem.h
    barlinepainter.h
    beamgroup.h
    caretpainter.h
    clickablegroup.h
    keysignaturepainter.h
    layoutinfo.h
    musicfont.h
    notestem.h
    scoreinforenderer.h
    simpletextitem.h
    staffpainter.h
    stdnotationnote.h
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
