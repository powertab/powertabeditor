include (../../common.pri)

SOURCE_DIR = ..
include (../../linking.pri)

TARGET = ../powertabeditor
TEMPLATE = app
CONFIG -= staticlib

# Link to appropriate libraries for RtMidi
win32:LIBS += -lwinmm
unix:LIBS += -lasound -lpthread
macx:LIBS += -framework CoreMidi -framework CoreAudio -framework CoreFoundation

SOURCES += main.cpp

RESOURCES += resources.qrc

# if shadow building is enabled
!equals($${PWD}, $${OUT_PWD}) {
    # copy skins folder to output directory
    unix|macx {
        QMAKE_POST_LINK += cp -rf "$${PWD}/../skins" "$${OUT_PWD}"
    }
    win32 {
        QMAKE_POST_LINK += xcopy /E /Y /I \"$${PWD}/../skins\" \"$${OUT_PWD}/skins\"
    }
}
