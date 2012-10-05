include (../../../common.pri)

QT -= core gui

# RtMidi configuration
win32:DEFINES +=__WINDOWS_MM__
unix:!macx:DEFINES += __LINUX_ALSASEQ__
macx:DEFINES += __MACOSX_AU__ __MACOSX_CORE__

HEADERS += \
    RtMidi.h \
    RtError.h

SOURCES += \
    RtMidi.cpp
