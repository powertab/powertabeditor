cmake_minimum_required(VERSION 2.8.9)

add_library(rtmidi
    rtmidi/RtMidi.cpp

    rtmidi/RtMidi.h
)

add_library(pugixml
    pugixml/src/pugixml.cpp

    pugixml/src/pugixml.hpp
)
