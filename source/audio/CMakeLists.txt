cmake_minimum_required(VERSION 2.8.9)

include_directories(${PROJECT_SOURCE_DIR}/external/rtmidi)

add_library(pteaudio
    midioutputdevice.cpp
    midiplayer.cpp

    midioutputdevice.h
    midiplayer.h
)

qt5_use_modules(pteaudio Widgets) 
add_precompiled_header(pteaudio)
