project( ptedialogs )

set( srcs
    alterationofpacedialog.cpp
    alternateendingdialog.cpp
    artificialharmonicdialog.cpp
    barlinedialog.cpp
    benddialog.cpp
    bulkconverterdialog.cpp
    chordnamedialog.cpp
    crashdialog.cpp
    directiondialog.cpp
    dynamicdialog.cpp
    fileinformationdialog.cpp
    filterrulewidget.cpp
    gotobarlinedialog.cpp
    gotorehearsalsigndialog.cpp
    infodialog.cpp
    irregulargroupingdialog.cpp
    lefthandfingeringdialog.cpp
    keyboardsettingsdialog.cpp
    keysignaturedialog.cpp
    multibarrestdialog.cpp
    playerchangedialog.cpp
    preferencesdialog.cpp
    rehearsalsigndialog.cpp
    staffdialog.cpp
    tappedharmonicdialog.cpp
    tempomarkerdialog.cpp
    textitemdialog.cpp
    timesignaturedialog.cpp
    tremolobardialog.cpp
    trilldialog.cpp
    tuningdialog.cpp
    tuningdictionarydialog.cpp
    viewfilterdialog.cpp
    viewfilterpresenter.cpp
    volumeswelldialog.cpp
)

set( headers
    alterationofpacedialog.h
    alternateendingdialog.h
    artificialharmonicdialog.h
    barlinedialog.h
    benddialog.h
    bulkconverterdialog.h
    chordnamedialog.h
    crashdialog.h
    directiondialog.h
    dynamicdialog.h
    fileinformationdialog.h
    filterrulewidget.h
    gotobarlinedialog.h
    gotorehearsalsigndialog.h
    infodialog.h
    irregulargroupingdialog.h
    keyboardsettingsdialog.h
    keysignaturedialog.h
    lefthandfingeringdialog.h
    multibarrestdialog.h
    playerchangedialog.h
    preferencesdialog.h
    rehearsalsigndialog.h
    staffdialog.h
    tappedharmonicdialog.h
    tempomarkerdialog.h
    textitemdialog.h
    timesignaturedialog.h
    tremolobardialog.h
    trilldialog.h
    tuningdialog.h
    tuningdictionarydialog.h
    viewfilterdialog.h
    viewfilterpresenter.h
    volumeswelldialog.h
)

set( moc_headers
    alterationofpacedialog.h
    alternateendingdialog.h
    artificialharmonicdialog.h
    barlinedialog.h
    benddialog.h
    bulkconverterdialog.h
    chordnamedialog.h
    crashdialog.h
    directiondialog.h
    fileinformationdialog.h
    filterrulewidget.h
    gotorehearsalsigndialog.h
    infodialog.h
    keyboardsettingsdialog.h
    keysignaturedialog.h
    lefthandfingeringdialog.h
    multibarrestdialog.h
    playerchangedialog.h
    preferencesdialog.h
    rehearsalsigndialog.h
    tappedharmonicdialog.h
    tempomarkerdialog.h
    textitemdialog.h
    timesignaturedialog.h
    tremolobardialog.h
    trilldialog.h
    tuningdialog.h
    tuningdictionarydialog.h
    viewfilterdialog.h
)

set( forms
    alterationofpacedialog.ui
    artificialharmonicdialog.ui
    barlinedialog.ui
    benddialog.ui
    bulkconverterdialog.ui
    chordnamedialog.ui
    crashdialog.ui
    directiondialog.ui
    dynamicdialog.ui
    fileinformationdialog.ui
    filterrulewidget.ui
    gotobarlinedialog.ui
    gotorehearsalsigndialog.ui
    infodialog.ui
    irregulargroupingdialog.ui
    keyboardsettingsdialog.ui
    keysignaturedialog.ui
    lefthandfingeringdialog.ui
    multibarrestdialog.ui
    playerchangedialog.ui
    preferencesdialog.ui
    rehearsalsigndialog.ui
    staffdialog.ui
    tappedharmonicdialog.ui
    tempomarkerdialog.ui
    textitemdialog.ui
    timesignaturedialog.ui
    tremolobardialog.ui
    trilldialog.ui
    tuningdialog.ui
    tuningdictionarydialog.ui
    viewfilterdialog.ui
    volumeswelldialog.ui
)

pte_library(
    NAME ptedialogs
    SOURCES ${srcs}
    HEADERS ${headers}
    MOC_HEADERS ${moc_headers}
    FORMS ${forms}
    DEPENDS
        ptescore
        Qt5::Widgets
)
