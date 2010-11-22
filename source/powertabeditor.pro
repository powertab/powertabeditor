#-------------------------------------------------
#
# Project created by QtCreator 2010-10-31T00:51:40
#
#-------------------------------------------------

QT       += core gui

TARGET = powertabeditor
TEMPLATE = app

QMAKE_CXXFLAGS+=-std=c++0x -U__STRICT_ANSI__
QMAKE_CXXFLAGS_RELEASE+=-O2

SOURCES += main.cpp\
        powertabeditor.cpp \
    documentmanager.cpp \
    powertabdocument/tuning.cpp \
    powertabdocument/timesignature.cpp \
    powertabdocument/tempomarker.cpp \
    powertabdocument/systemsymbol.cpp \
    powertabdocument/system.cpp \
    powertabdocument/staff.cpp \
    powertabdocument/score.cpp \
    powertabdocument/rhythmslash.cpp \
    powertabdocument/rehearsalsign.cpp \
    powertabdocument/rect.cpp \
    powertabdocument/powertaboutputstream.cpp \
    powertabdocument/powertabinputstream.cpp \
    powertabdocument/powertabfileheader.cpp \
    powertabdocument/powertabdocument.cpp \
    powertabdocument/position.cpp \
    powertabdocument/oldtimesignature.cpp \
    powertabdocument/oldrehearsalsign.cpp \
    powertabdocument/note.cpp \
    powertabdocument/macros.cpp \
    powertabdocument/keysignature.cpp \
    powertabdocument/guitarin.cpp \
    powertabdocument/guitar.cpp \
    powertabdocument/generalmidi.cpp \
    powertabdocument/fontsetting.cpp \
    powertabdocument/floatingtext.cpp \
    powertabdocument/dynamic.cpp \
    powertabdocument/direction.cpp \
    powertabdocument/colour.cpp \
    powertabdocument/chordtext.cpp \
    powertabdocument/chordname.cpp \
    powertabdocument/chorddiagram.cpp \
    powertabdocument/barline.cpp \
    powertabdocument/alternateending.cpp \
    scorearea.cpp \
    musicfont.cpp \
    painters/barlinepainter.cpp \
    painters/staffdata.cpp \
    actions/changebarlinetype.cpp \
    dialogs/barlinedialog.cpp \
    painters/tabnotepainter.cpp \
    painters/caret.cpp \
    dialogs/preferencesdialog.cpp \
    toolbox.cpp

HEADERS  += powertabeditor.h \
    documentmanager.h \
    powertabdocument/tuning.h \
    powertabdocument/timesignature.h \
    powertabdocument/tempomarker.h \
    powertabdocument/systemsymbol.h \
    powertabdocument/system.h \
    powertabdocument/staff.h \
    powertabdocument/score.h \
    powertabdocument/rhythmslash.h \
    powertabdocument/rehearsalsign.h \
    powertabdocument/rect.h \
    powertabdocument/powertaboutputstream.h \
    powertabdocument/powertabobject.h \
    powertabdocument/powertabinputstream.h \
    powertabdocument/powertabfileheader.h \
    powertabdocument/powertabdocument.h \
    powertabdocument/position.h \
    powertabdocument/oldtimesignature.h \
    powertabdocument/oldrehearsalsign.h \
    powertabdocument/note.h \
    powertabdocument/macros.h \
    powertabdocument/keysignature.h \
    powertabdocument/guitarin.h \
    powertabdocument/guitar.h \
    powertabdocument/generalmidi.h \
    powertabdocument/fontsetting.h \
    powertabdocument/floatingtext.h \
    powertabdocument/dynamic.h \
    powertabdocument/direction.h \
    powertabdocument/colour.h \
    powertabdocument/chordtext.h \
    powertabdocument/chordname.h \
    powertabdocument/chorddiagram.h \
    powertabdocument/barline.h \
    powertabdocument/alternateending.h \
    powertabdocument/powertabstream.h \
    scorearea.h \
    musicfont.h \
    painters/barlinepainter.h \
    painters/staffdata.h \
    painters/painterbase.h \
    actions/changebarlinetype.h \
    dialogs/barlinedialog.h \
    painters/tabnotepainter.h \
    painters/caret.h \
    dialogs/preferencesdialog.h \
    toolbox.h

RESOURCES += \
    resources.qrc
