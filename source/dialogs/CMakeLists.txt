cmake_minimum_required(VERSION 2.8.8)

set(FORMS
    artificialharmonicdialog.ui
    barlinedialog.ui
    directiondialog.ui
    dynamicdialog.ui
    fileinformationdialog.ui
    gotobarlinedialog.ui
    gotorehearsalsigndialog.ui
    irregulargroupingdialog.ui
    keyboardsettingsdialog.ui
    keysignaturedialog.ui
    preferencesdialog.ui
    rehearsalsigndialog.ui
    tempomarkerdialog.ui
    timesignaturedialog.ui
    trilldialog.ui
    tuningdialog.ui
    tuningdictionarydialog.ui
    volumeswelldialog.ui
)

qt5_wrap_ui(FORMS_HEADERS ${FORMS})

add_library(ptedialogs
    ${FORMS_HEADERS}
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
    preferencesdialog.cpp
    rehearsalsigndialog.cpp
    tappedharmonicdialog.cpp
    tempomarkerdialog.cpp
    timesignaturedialog.cpp
    trilldialog.cpp
    tuningdialog.cpp
    tuningdictionarydialog.cpp
    volumeswelldialog.cpp
)

qt5_use_modules(ptedialogs Widgets) 
