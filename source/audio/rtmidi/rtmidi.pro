include (../../../common.pri)

QT -= core gui

# RtMidi configuration
win32:DEFINES +=__WINDOWS_MM__
unix:DEFINES += __LINUX_ALSASEQ__
macx:DEFINES += __MACOSX_CORE__

HEADERS += \
    rtmidi.h \
    rterror.h

SOURCES += \
    rtmidi.cpp
