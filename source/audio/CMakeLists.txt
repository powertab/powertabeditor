cmake_minimum_required(VERSION 2.8.8)

add_library(pteaudio
    bendevent.cpp
    #directionsymbol.cpp
    letringevent.cpp
    metronomeevent.cpp
    midievent.cpp
    midioutputdevice.cpp
    #midiplayer.cpp
    playnoteevent.cpp
    #repeatcontroller.cpp
    #repeat.cpp
    restevent.cpp
    stopnoteevent.cpp
    vibratoevent.cpp
    volumechangeevent.cpp
)

qt5_use_modules(pteaudio Widgets) 

add_subdirectory(rtmidi)
