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

# RtMidi configuration
win32:DEFINES +=__WINDOWS_MM__
win32:LIBS += -lwinmm
unix:DEFINES += __LINUX_ALSASEQ__
unix:LIBS += -lasound -lpthread
macx:DEFINES += __MACOSX_CORE__
macx:LIBS += -framework CoreMidi -framework CoreAudio -framework CoreFoundation

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
    widgets/toolbox/toolbox.cpp \
    widgets/toolbox/scorepage.cpp \
    widgets/toolbox/notepage.cpp \
    widgets/mixer/mixerinstrument.cpp \
    widgets/mixer/mixer.cpp \
    skinmanager.cpp \
    painters/keysignaturepainter.cpp \
    rtmidiwrapper.cpp \
    rtmidi/rtmidi.cpp \
    painters/timesignaturepainter.cpp \
    painters/clefpainter.cpp \
    midiplayer.cpp \
    painters/stdnotationpainter.cpp \
    actions/undomanager.cpp \
    painters/chordtextpainter.cpp \
    actions/removechordtext.cpp \
    dialogs/chordnamedialog.cpp \
    actions/addchordtext.cpp

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
    widgets/mixer/mixer.h \
    widgets/mixer/mixerinstrument.h \
    widgets/toolbox/toolbox.h \
    widgets/toolbox/scorepage.h \
    widgets/toolbox/notepage.h \
    skinmanager.h \
    painters/keysignaturepainter.h \
    rtmidiwrapper.h \
    rtmidi/rtmidi.h \
    rtmidi/rterror.h \
    painters/timesignaturepainter.h \
    painters/clefpainter.h \
    midiplayer.h \
    painters/stdnotationpainter.h \
    actions/undomanager.h \
    painters/chordtextpainter.h \
    actions/removechordtext.h \
    dialogs/chordnamedialog.h \
    actions/addchordtext.h

RESOURCES += \
    resources.qrc

# if shadow building is enabled
!equals($${PWD}, $${OUT_PWD}) {

    # commands to copy files from the current directory to the output directory
    win32 {
    CHECK_DIR_EXIST = IF EXIST
    OR = else
    MAKE_DIR = mkdir
    COPY = xcopy /E /Y
    THEN = (
    DONE_IF = )
    }
    unix|macx {
    CHECK_DIR_EXIST = test -d
    OR = ||
    MAKE_DIR = mkdir
    COPY = cp -r -u
    THEN = &&
    DONE_IF = ''
    }


    # select the right subfolder
    CONFIG(release, debug|release) {
     BUILD_TYPE = release
    } else {
     BUILD_TYPE = debug
    }

    # specify files for copying
    SOURCE_SKINS = $${PWD}/skins
    win32:DEST_SKINS = $${OUT_PWD}/$${BUILD_TYPE}/skins
    unix|macx:DEST_SKINS = $${OUT_PWD}/

    # replace '/' with '\' in Windows paths
    win32 {
    SOURCE_SKINS = $${replace(SOURCE_SKINS, /, \\)}
    DEST_SKINS = $${replace(DEST_SKINS, /, \\)}
    }

    CHECK_DEST_SKINS_DIR_EXIST = $$CHECK_DIR_EXIST $$DEST_SKINS
    MAKE_DEST_SKINS_DIR = $$MAKE_DIR $$DEST_SKINS
    COPY_COMPILED_TRANSLATIONS = $$COPY $$SOURCE_SKINS $$DEST_SKINS

    QMAKE_POST_LINK += $$CHECK_DEST_SKINS_DIR_EXIST \
    $${THEN} $$COPY_COMPILED_TRANSLATIONS $${DONE_IF} $${OR} \
    $$MAKE_DEST_SKINS_DIR && $$COPY_COMPILED_TRANSLATIONS
}
