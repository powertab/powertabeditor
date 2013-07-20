include (../../common.pri)

SOURCE_DIR = ..
include (../../linking.pri)

TARGET = ../powertabeditor
TEMPLATE = app
CONFIG -= staticlib

# Link to appropriate libraries for RtMidi
win32:LIBS += -lwinmm
unix:!macx:LIBS += -lasound -lpthread
macx:LIBS += -framework CoreMidi -framework CoreAudio -framework CoreFoundation\
             -framework AudioToolbox -framework AudioUnit

SOURCES += main.cpp

RESOURCES += resources.qrc

win32:RC_FILE = build.rc

# if shadow building is enabled
!equals($${PWD}, $${OUT_PWD}) {
    # copy skins folder to output directory
    unix:!macx: {
        QMAKE_POST_LINK += cp -rf "$${PWD}/../skins" "$${PWD}/../data" "$${OUT_PWD}"
    }
    # ignore hidden folders (i.e. .svn)
    macx {
        QMAKE_POST_LINK += rsync -av --exclude='.*'\
            "$${PWD}/../skins" "$${PWD}/../data" "$${OUT_PWD}/powertabeditor.app/Contents/MacOS/"
    }
    win32 {
        QMAKE_POST_LINK += xcopy /E /Y /I \"$${PWD}/../skins\" \"$${OUT_PWD}/skins\" & xcopy /E /Y /I \"$${PWD}/../data\" \"$${OUT_PWD}/data\"
    }
}
