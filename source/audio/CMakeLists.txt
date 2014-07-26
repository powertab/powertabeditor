cmake_minimum_required(VERSION 2.8.9)

include_directories(${PROJECT_SOURCE_DIR}/external/rtmidi)

add_library(pteaudio
    bendevent.cpp
    letringevent.cpp
    metronomeevent.cpp
    midievent.cpp
    midioutputdevice.cpp
    midiplayer.cpp
    playnoteevent.cpp
    repeatcontroller.cpp
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
    restevent.h
    stopnoteevent.h
    vibratoevent.h
    volumechangeevent.h
)

qt5_use_modules(pteaudio Widgets) 
cotire(pteaudio)
