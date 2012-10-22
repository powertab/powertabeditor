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
    timesignaturedialog.h \
    volumeswelldialog.h \
    irregulargroupingdialog.h \
    fileinformationdialog.h \
    gotorehearsalsigndialog.h \
    tempomarkerdialog.h

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
    timesignaturedialog.cpp \
    volumeswelldialog.cpp \
    irregulargroupingdialog.cpp \
    fileinformationdialog.cpp \
    gotorehearsalsigndialog.cpp \
    tempomarkerdialog.cpp

FORMS += \
    keyboardsettingsdialog.ui \
    dynamicdialog.ui \
    timesignaturedialog.ui \
    rehearsalsigndialog.ui \
    preferencesdialog.ui \
    tuningdialog.ui \
    volumeswelldialog.ui \
    irregulargroupingdialog.ui \
    keysignaturedialog.ui \
    fileinformationdialog.ui \
    gotorehearsalsigndialog.ui \
    tempomarkerdialog.ui
