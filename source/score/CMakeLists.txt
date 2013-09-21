cmake_minimum_required(VERSION 2.8.8)

add_library(ptescore
    alternateending.cpp
    barline.cpp
    chordname.cpp
    chordtext.cpp
    direction.cpp
    dynamic.cpp
    generalmidi.cpp
    instrument.cpp
    keysignature.cpp
    note.cpp
    player.cpp
    playerchange.cpp
    position.cpp
    rehearsalsign.cpp
    score.cpp
    scoreinfo.cpp
    scorelocation.cpp
    serialization.cpp
    staff.cpp
    staffutils.cpp
    system.cpp
    systemlocation.cpp
    tempomarker.cpp
    timesignature.cpp
    tuning.cpp

    # Add header-only files here so that they show up in the generated projects
    # for Visual Studio, QtCreator, etc.
    utils.h
)
