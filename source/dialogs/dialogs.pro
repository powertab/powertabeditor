include (../../common.pri)

CONFIG += precompile_header

PRECOMPILED_HEADER = dialogs_pch.h

HEADERS += \
    tuningdialog.h \
    trilldialog.h \
    rehearsalsigndialog.h \
    preferencesdialog.h \
    chordnamedialog.h \
    barlinedialog.h \
    alternateendingdialog.h \
    tappedharmonicdialog.h \
    keysignaturedialog.h \
    timesignaturedialog.h \
    keyboardsettingsdialog.h \
    dynamicdialog.h \
    dialogs_pch.h

SOURCES += \
    tuningdialog.cpp \
    trilldialog.cpp \
    rehearsalsigndialog.cpp \
    preferencesdialog.cpp \
    chordnamedialog.cpp \
    barlinedialog.cpp \
    alternateendingdialog.cpp \
    tappedharmonicdialog.cpp \
    keysignaturedialog.cpp \
    timesignaturedialog.cpp \
    keyboardsettingsdialog.cpp \
    dynamicdialog.cpp

FORMS += \
    keyboardsettingsdialog.ui \
    dynamicdialog.ui




