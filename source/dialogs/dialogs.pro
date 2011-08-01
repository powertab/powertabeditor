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
    keyboardsettingsdialog.h \
    dynamicdialog.h \
    dialogs_pch.h \
    timesignaturedialog.h

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
    keyboardsettingsdialog.cpp \
    dynamicdialog.cpp \
    timesignaturedialog.cpp

FORMS += \
    keyboardsettingsdialog.ui \
    dynamicdialog.ui \
    timesignaturedialog.ui









