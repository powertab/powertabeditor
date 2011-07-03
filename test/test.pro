include (../common.pri)

SOURCE_DIR=../source
include (../linking.pri)

TARGET = pte_tests
TEMPLATE = app

unix:LIBS += -lboost_unit_test_framework

# replace with the path to your boost installation
win32:INCLUDEPATH += 'C:/Program Files (x86)/boost/boost_1_44/'
win32:LIBS += -L'C:/Program Files (x86)/boost/boost_1_44/lib' -lboost_unit_test_framework

SOURCES += \
    test_main.cpp \
    powertabdocument/staff_test.cpp \    
    powertabdocument/system_test.cpp \
    powertabdocument/position_test.cpp \
    audio/test_midievent.cpp \
    powertabdocument/tuning_test.cpp \
    powertabdocument/generalmidi_test.cpp \
    powertabdocument/alternateending_test.cpp \
    powertabdocument/chorddiagram_test.cpp \
    powertabdocument/direction_test.cpp \
    powertabdocument/note_test.cpp \
    powertabdocument/complexsymbolarray_test.cpp \
    actions/test_editslideinto.cpp \
    actions/test_editslideout.cpp \
    app/test_documentmanager.cpp \
    powertabdocument/harmonics_test.cpp \
    actions/test_removetappedharmonic.cpp \
    actions/test_addtappedharmonic.cpp \
    powertabdocument/score_test.cpp \
    powertabdocument/common_test.cpp \
    actions/test_editrest.cpp \
    actions/test_editkeysignature.cpp \
    powertabdocument/keysignature_test.cpp \
    actions/test_edittimesignature.cpp \
    powertabdocument/timesignature_test.cpp \
    formats/guitar_pro/test_gp4.cpp

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
    ../source/powertabdocument/systemlocation.h \
    ../source/audio/midievent.h \
    powertabdocument/tuning_fixtures.h \
    powertabdocument/serialization_test.h \
    powertabdocument/score_fixture.h

# copy test files to output directory
QMAKE_POST_LINK += $$COPY_DIR $${PWD}/formats/guitar_pro/data $$OUT_PWD/$${BUILDTYPE}
