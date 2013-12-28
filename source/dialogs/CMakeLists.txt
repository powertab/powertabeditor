cmake_minimum_required(VERSION 2.8.8)

set(FORMS
    alterationofpacedialog.ui
    artificialharmonicdialog.ui
    barlinedialog.ui
    chordnamedialog.ui
    directiondialog.ui
    dynamicdialog.ui
    fileinformationdialog.ui
    gotobarlinedialog.ui
    gotorehearsalsigndialog.ui
    irregulargroupingdialog.ui
    keyboardsettingsdialog.ui
    keysignaturedialog.ui
    multibarrestdialog.ui
    playerchangedialog.ui
    preferencesdialog.ui
    rehearsalsigndialog.ui
    staffdialog.ui
    tappedharmonicdialog.ui
    tempomarkerdialog.ui
    timesignaturedialog.ui
    trilldialog.ui
    tuningdialog.ui
    tuningdictionarydialog.ui
    #volumeswelldialog.ui
)

qt5_wrap_ui(FORMS_HEADERS ${FORMS})

add_library(ptedialogs
    ${FORMS_HEADERS}
    alterationofpacedialog.cpp
    alternateendingdialog.cpp
    artificialharmonicdialog.cpp
    barlinedialog.cpp
    chordnamedialog.cpp
    directiondialog.cpp
    dynamicdialog.cpp
    fileinformationdialog.cpp
    gotobarlinedialog.cpp
    gotorehearsalsigndialog.cpp
    irregulargroupingdialog.cpp
    keyboardsettingsdialog.cpp
    keysignaturedialog.cpp
    multibarrestdialog.cpp
    playerchangedialog.cpp
    preferencesdialog.cpp
    rehearsalsigndialog.cpp
    staffdialog.cpp
    tappedharmonicdialog.cpp
    tempomarkerdialog.cpp
    timesignaturedialog.cpp
    trilldialog.cpp
    tuningdialog.cpp
    tuningdictionarydialog.cpp
    #volumeswelldialog.cpp

    alterationofpacedialog.h
    alternateendingdialog.h
    artificialharmonicdialog.h
    barlinedialog.h
    chordnamedialog.h
    directiondialog.h
    dynamicdialog.h
    fileinformationdialog.h
    gotobarlinedialog.h
    gotorehearsalsigndialog.h
    irregulargroupingdialog.h
    keyboardsettingsdialog.h
    keysignaturedialog.h
    multibarrestdialog.h
    playerchangedialog.h
    preferencesdialog.h
    rehearsalsigndialog.h
    staffdialog.h
    tappedharmonicdialog.h
    tempomarkerdialog.h
    timesignaturedialog.h
    trilldialog.h
    tuningdialog.h
    tuningdictionarydialog.h
    #volumeswelldialog.h

    # Precompiled header.
    precompiled.h
    precompiled.cpp
)

qt5_use_modules(ptedialogs Widgets) 
add_precompiled_header(ptedialogs precompiled.h FORCEINCLUDE)
