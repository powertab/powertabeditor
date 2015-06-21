cmake_minimum_required(VERSION 2.8.9)

add_library(ptemidi
    midievent.cpp
    midieventlist.cpp
	midifile.cpp

    midievent.h
    midieventlist.h
	midifile.h
)

cotire(ptemidi)
