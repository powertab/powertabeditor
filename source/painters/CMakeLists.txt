cmake_minimum_required(VERSION 2.8.9)

add_library(ptepainters
    barlinepainter.cpp
    beamgroup.cpp
    caretpainter.cpp
    clickablegroup.cpp
    directions.cpp
    #irregularnotegroup.cpp
    keysignaturepainter.cpp
    layoutinfo.cpp
    musicfont.cpp
    notestem.cpp
    #rhythmslashpainter.cpp
    simpletextitem.cpp
    staffpainter.cpp
    stdnotationnote.cpp
    systemrenderer.cpp
    timesignaturepainter.cpp
    #tremolobarpainter.cpp
    verticallayout.cpp

    barlinepainter.h
    beamgroup.h
    caretpainter.h
    clickablegroup.h
    #irregularnotegroup.h
    keysignaturepainter.h
    layoutinfo.h
    musicfont.h
    notestem.h
    #rhythmslashpainter.h
    simpletextitem.h
    staffpainter.h
    stdnotationnote.h
    systemrenderer.h
    timesignaturepainter.h
    #tremolobarpainter.h
    verticallayout.h
)

qt5_use_modules(ptepainters Widgets)
cotire(ptepainters)
