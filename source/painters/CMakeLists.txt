cmake_minimum_required(VERSION 2.8.9)

add_library(ptepainters
    barlinepainter.cpp
    beamgroup.cpp
    caretpainter.cpp
    clefpainter.cpp
    directionpainter.cpp
    #irregularnotegroup.cpp
    keysignaturepainter.cpp
    layoutinfo.cpp
    musicfont.cpp
    notestem.cpp
    #rhythmslashpainter.cpp
    staffpainter.cpp
    stdnotationnote.cpp
    systemrenderer.cpp
    tempomarkerpainter.cpp
    timesignaturepainter.cpp
    #tremolobarpainter.cpp

    barlinepainter.h
    beamgroup.h
    caretpainter.h
    clefpainter.h
    directionpainter.h
    #irregularnotegroup.h
    keysignaturepainter.h
    layoutinfo.h
    musicfont.h
    notestem.h
    #rhythmslashpainter.h
    staffpainter.h
    stdnotationnote.h
    systemrenderer.h
    tempomarkerpainter.h
    timesignaturepainter.h
    #tremolobarpainter.h
)

qt5_use_modules(ptepainters Widgets)
cotire(ptepainters)
