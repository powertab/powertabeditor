cmake_minimum_required(VERSION 2.8.8)

add_library(ptepainters
    barlinepainter.cpp
    #beamgroup.cpp
    #caret.cpp
    #chordtextpainter.cpp
    clefpainter.cpp
    #directionpainter.cpp
    #irregularnotegroup.cpp
    keysignaturepainter.cpp
    layoutinfo.cpp
    musicfont.cpp
    #restpainter.cpp
    #rhythmslashpainter.cpp
    #staffdata.cpp
    staffpainter.cpp
    #stdnotationpainter.cpp
    systemrenderer.cpp
    tabnotepainter.cpp
    tempomarkerpainter.cpp
    timesignaturepainter.cpp
    #tremolobarpainter.cpp
)

qt5_use_modules(ptepainters Widgets) 
