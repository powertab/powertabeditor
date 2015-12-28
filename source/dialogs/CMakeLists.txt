project( ptedialogs )

set( srcs
    alterationofpacedialog.cpp
    alternateendingdialog.cpp
    artificialharmonicdialog.cpp
    barlinedialog.cpp
    benddialog.cpp
    chordnamedialog.cpp
    crashdialog.cpp
    directiondialog.cpp
    dynamicdialog.cpp
    fileinformationdialog.cpp
    filterrulewidget.cpp
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
    textitemdialog.cpp
    timesignaturedialog.cpp
    trilldialog.cpp
    tuningdialog.cpp
    tuningdictionarydialog.cpp
    viewfilterdialog.cpp
    viewfilterpresenter.cpp
)

set( headers
    alterationofpacedialog.h
    alternateendingdialog.h
    artificialharmonicdialog.h
    barlinedialog.h
    benddialog.h
    chordnamedialog.h
    crashdialog.h
    directiondialog.h
    dynamicdialog.h
    fileinformationdialog.h
    filterrulewidget.h
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
    textitemdialog.h
    timesignaturedialog.h
    trilldialog.h
    tuningdialog.h
    tuningdictionarydialog.h
    viewfilterdialog.h
    viewfilterpresenter.h
)

set( moc_headers
    alterationofpacedialog.h
    alternateendingdialog.h
    artificialharmonicdialog.h
    barlinedialog.h
    benddialog.h
    chordnamedialog.h
    crashdialog.h
    directiondialog.h
    fileinformationdialog.h
    filterrulewidget.h
    gotorehearsalsigndialog.h
    keyboardsettingsdialog.h
    keysignaturedialog.h
    multibarrestdialog.h
    playerchangedialog.h
    preferencesdialog.h
    rehearsalsigndialog.h
    tappedharmonicdialog.h
    tempomarkerdialog.h
    textitemdialog.h
    timesignaturedialog.h
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
    chordnamedialog.ui
    crashdialog.ui
    directiondialog.ui
    dynamicdialog.ui
    fileinformationdialog.ui
    filterrulewidget.ui
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
    textitemdialog.ui
    timesignaturedialog.ui
    trilldialog.ui
    tuningdialog.ui
    tuningdictionarydialog.ui
    viewfilterdialog.ui
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
