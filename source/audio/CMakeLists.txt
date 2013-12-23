cmake_minimum_required(VERSION 2.8.8)

add_library(pteaudio
    bendevent.cpp
    letringevent.cpp
    metronomeevent.cpp
    midievent.cpp
    midioutputdevice.cpp
    midiplayer.cpp
    playnoteevent.cpp
    repeatcontroller.cpp
    repeat.cpp
    restevent.cpp
    stopnoteevent.cpp
    vibratoevent.cpp
    volumechangeevent.cpp

    bendevent.h
    letringevent.h
    metronomeevent.h
    midievent.h
    midioutputdevice.h
    midiplayer.h
    playnoteevent.h
    repeatcontroller.h
    repeat.h
    restevent.h
    stopnoteevent.h
    vibratoevent.h
    volumechangeevent.h
)

qt5_use_modules(pteaudio Widgets) 

add_subdirectory(rtmidi)
