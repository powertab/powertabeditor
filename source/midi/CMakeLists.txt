cmake_minimum_required(VERSION 2.8.9)

add_library(ptemidi
    midievent.cpp
    midieventlist.cpp
	midifile.cpp

    midievent.h
    midieventlist.h
	midifile.h
)

qt5_use_modules(ptemidi Core)
add_precompiled_header(ptemidi)
