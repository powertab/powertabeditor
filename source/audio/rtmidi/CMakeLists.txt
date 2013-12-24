cmake_minimum_required(VERSION 2.8.8)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-D__LINUX_ALSA__)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(-D__MACOSX_AU__)
    add_definitions(-D__MACOSX_CORE__)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_definitions(-D__WINDOWS_MM__)
endif()

add_library(rtmidi
    RtMidi.cpp

    RtError.h
    RtMidi.h
)
