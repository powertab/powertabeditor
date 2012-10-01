include (../../../common.pri)

QT -= core gui

# RtMidi configuration
win32:DEFINES +=__WINDOWS_MM__
unix:DEFINES += __LINUX_ALSA__
macx:DEFINES += __MACOSX_CORE__

HEADERS += \
    RtMidi.h \
    RtError.h

SOURCES += \
    RtMidi.cpp
