TARGET = pte_tests
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x -U__STRICT_ANSI__

# don't need qt dependencies
QT     -= gui core
LIBS   -= -lQtGui -lQtCore

INCLUDEPATH += "../source"

unix:LIBS += -lboost_unit_test_framework

# replace with the path to your boost installation
win32:INCLUDEPATH += 'C:/Program Files (x86)/boost/boost_1_44/'
win32:LIBS += -L'C:/Program Files (x86)/boost/boost_1_44/lib' -lboost_unit_test_framework

SOURCES += \
    test_main.cpp \
    powertabdocument/staff_test.cpp \
    ../source/powertabdocument/tuning.cpp \
    ../source/powertabdocument/timesignature.cpp \
    ../source/powertabdocument/tempomarker.cpp \
    ../source/powertabdocument/systemsymbol.cpp \
    ../source/powertabdocument/system.cpp \
    ../source/powertabdocument/staff.cpp \
    ../source/powertabdocument/score.cpp \
    ../source/powertabdocument/rhythmslash.cpp \
    ../source/powertabdocument/rehearsalsign.cpp \
    ../source/powertabdocument/rect.cpp \
    ../source/powertabdocument/powertaboutputstream.cpp \
    ../source/powertabdocument/powertabinputstream.cpp \
    ../source/powertabdocument/powertabfileheader.cpp \
    ../source/powertabdocument/powertabdocument.cpp \
    ../source/powertabdocument/position.cpp \
    ../source/powertabdocument/oldtimesignature.cpp \
    ../source/powertabdocument/oldrehearsalsign.cpp \
    ../source/powertabdocument/note.cpp \
    ../source/powertabdocument/macros.cpp \
    ../source/powertabdocument/keysignature.cpp \
    ../source/powertabdocument/guitarin.cpp \
    ../source/powertabdocument/guitar.cpp \
    ../source/powertabdocument/generalmidi.cpp \
    ../source/powertabdocument/fontsetting.cpp \
    ../source/powertabdocument/floatingtext.cpp \
    ../source/powertabdocument/dynamic.cpp \
    ../source/powertabdocument/direction.cpp \
    ../source/powertabdocument/colour.cpp \
    ../source/powertabdocument/chordtext.cpp \
    ../source/powertabdocument/chordname.cpp \
    ../source/powertabdocument/chorddiagram.cpp \
    ../source/powertabdocument/barline.cpp \
    ../source/powertabdocument/alternateending.cpp \
    ../source/audio/midievent.cpp \
    powertabdocument/system_test.cpp \
    powertabdocument/position_test.cpp \
    audio/test_midievent.cpp \
    powertabdocument/tuning_test.cpp \
    powertabdocument/generalmidi_test.cpp \
    powertabdocument/alternateending_test.cpp

HEADERS += \
    ../source/powertabdocument/tuning.h \
    ../source/powertabdocument/timesignature.h \
    ../source/powertabdocument/tempomarker.h \
    ../source/powertabdocument/systemsymbol.h \
    ../source/powertabdocument/system.h \
    ../source/powertabdocument/staff.h \
    ../source/powertabdocument/score.h \
    ../source/powertabdocument/rhythmslash.h \
    ../source/powertabdocument/rehearsalsign.h \
    ../source/powertabdocument/rect.h \
    ../source/powertabdocument/powertabstream.h \
    ../source/powertabdocument/powertaboutputstream.h \
    ../source/powertabdocument/powertabobject.h \
    ../source/powertabdocument/powertabinputstream.h \
    ../source/powertabdocument/powertabfileheader.h \
    ../source/powertabdocument/powertabdocument.h \
    ../source/powertabdocument/position.h \
    ../source/powertabdocument/oldtimesignature.h \
    ../source/powertabdocument/oldrehearsalsign.h \
    ../source/powertabdocument/note.h \
    ../source/powertabdocument/macros.h \
    ../source/powertabdocument/keysignature.h \
    ../source/powertabdocument/guitarin.h \
    ../source/powertabdocument/guitar.h \
    ../source/powertabdocument/generalmidi.h \
    ../source/powertabdocument/fontsetting.h \
    ../source/powertabdocument/floatingtext.h \
    ../source/powertabdocument/dynamic.h \
    ../source/powertabdocument/direction.h \
    ../source/powertabdocument/colour.h \
    ../source/powertabdocument/chordtext.h \
    ../source/powertabdocument/chordname.h \
    ../source/powertabdocument/chorddiagram.h \
    ../source/powertabdocument/barline.h \
    ../source/powertabdocument/alternateending.h \
    ../source/audio/midievent.h \
    powertabdocument/tuning_fixtures.h \
    powertabdocument/serialization_test.h
