cmake_minimum_required(VERSION 2.8.8)

add_library(ptepainters
    barlinepainter.cpp
    #beamgroup.cpp
    caretpainter.cpp
    #chordtextpainter.cpp
    clefpainter.cpp
    directionpainter.cpp
    #irregularnotegroup.cpp
    keysignaturepainter.cpp
    layoutinfo.cpp
    musicfont.cpp
    #rhythmslashpainter.cpp
    #staffdata.cpp
    staffpainter.cpp
    stdnotationnote.cpp
    #stdnotationpainter.cpp
    systemrenderer.cpp
    tempomarkerpainter.cpp
    timesignaturepainter.cpp
    #tremolobarpainter.cpp
)

qt5_use_modules(ptepainters Widgets) 
