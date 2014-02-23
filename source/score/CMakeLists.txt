cmake_minimum_required(VERSION 2.8.9)

add_library(ptescore
    alternateending.cpp
    barline.cpp
    chordname.cpp
    chordtext.cpp
    direction.cpp
    dynamic.cpp
    generalmidi.cpp
    instrument.cpp
    irregulargrouping.cpp
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
    system.cpp
    systemlocation.cpp
    tempomarker.cpp
    timesignature.cpp
    tuning.cpp
    voice.cpp
    voiceutils.cpp

    utils/repeatindexer.cpp
    utils/scoremerger.cpp

    # Add header files here so that they show up in the generated projects for
    # Visual Studio, QtCreator, etc.
    alternateending.h
    barline.h
    chordname.h
    chordtext.h
    direction.h
    dynamic.h
    fileversion.h
    generalmidi.h
    instrument.h
    irregulargrouping.h
    keysignature.h
    note.h
    player.h
    playerchange.h
    position.h
    rehearsalsign.h
    score.h
    scoreinfo.h
    scorelocation.h
    serialization.h
    staff.h
    system.h
    systemlocation.h
    tempomarker.h
    timesignature.h
    tuning.h
    utils.h
    voice.h
    voiceutils.h

    utils/repeatindexer.h
    utils/scoremerger.h
)

cotire(ptescore)
