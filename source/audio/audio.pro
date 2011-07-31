include (../../common.pri)

# can't use the default name of 'libaudio' since
# it conflicts with a system library on Ubuntu
TARGET = pteaudio

CONFIG += precompile_header
PRECOMPILED_HEADER = audio_pch.h

# uncomment to enable logging for midi events
#DEFINES += LOG_MIDI_EVENTS

HEADERS += \
    vibratoevent.h \
    stopnoteevent.h \
    rtmidiwrapper.h \
    repeatcontroller.h \
    repeat.h \
    playnoteevent.h \
    midiplayer.h \
    midievent.h \
    metronomeevent.h \
    letringevent.h \
    directionsymbol.h \
    bendevent.h \
    audio_pch.h \
    volumechangeevent.h

SOURCES += \
    vibratoevent.cpp \
    stopnoteevent.cpp \
    rtmidiwrapper.cpp \
    repeatcontroller.cpp \
    repeat.cpp \
    playnoteevent.cpp \
    midiplayer.cpp \
    midievent.cpp \
    metronomeevent.cpp \
    letringevent.cpp \
    directionsymbol.cpp \
    bendevent.cpp \
    volumechangeevent.cpp



