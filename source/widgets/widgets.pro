include (../../common.pri)

HEADERS += \
    toolbox/toolbox.h \
    toolbox/scorepage.h \
    toolbox/notepage.h \
    playback/playbackwidget.h \
    mixer/mixerinstrument.h \
    mixer/mixer.h \
    clickablelabel.h \
    common.h

SOURCES += \
    common.cpp \
    toolbox/toolbox.cpp \
    toolbox/scorepage.cpp \
    toolbox/notepage.cpp \
    playback/playbackwidget.cpp \
    mixer/mixerinstrument.cpp \
    mixer/mixer.cpp \
    clickablelabel.cpp

FORMS += \
    playback/playbackwidget.ui
