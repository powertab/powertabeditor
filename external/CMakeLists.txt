cmake_minimum_required(VERSION 2.8.9)

add_library(rtmidi
    rtmidi/RtMidi.cpp

    rtmidi/RtMidi.h
)

add_library(pugixml
    pugixml/src/pugixml.cpp

    pugixml/src/pugixml.hpp
)

# Don't bother building unit tests for this library.
set(WITHERSHINS_BUILD_TESTS OFF CACHE BOOL "Build unit tests.")
# Propagate our -Werror setting downwards.
set(WITHERSHINS_ENABLE_WERROR ${ENABLE_WERROR} CACHE BOOL "Fail and stop if a warning is triggered.")
add_subdirectory(withershins)
