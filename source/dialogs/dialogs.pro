include (../../common.pri)

CONFIG += precompile_header

PRECOMPILED_HEADER = dialogs_pch.h

HEADERS += \
    tuningdialog.h \
    rehearsalsigndialog.h \
    preferencesdialog.h \
    chordnamedialog.h \
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
    tempomarkerdialog.h \
    barlinedialog.h \
    gotobarlinedialog.h \
    trilldialog.h

SOURCES += \
    tuningdialog.cpp \
    rehearsalsigndialog.cpp \
    preferencesdialog.cpp \
    chordnamedialog.cpp \
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
    tempomarkerdialog.cpp \
    barlinedialog.cpp \
    gotobarlinedialog.cpp \
    trilldialog.cpp

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
    tempomarkerdialog.ui \
    barlinedialog.ui \
    gotobarlinedialog.ui \
    trilldialog.ui
