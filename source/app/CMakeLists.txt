project( pteapp )

set( srcs
    appinfo.cpp
    autobackup.cpp
    caret.cpp
    clipboard.cpp
    command.cpp
    documentmanager.cpp
    paths.cpp
    powertabeditor.cpp
    recentfiles.cpp
    scorearea.cpp
    settings.cpp
    settingsmanager.cpp
    tuningdictionary.cpp
    viewoptions.cpp
)

set( headers
    appinfo.h
    autobackup.h
    caret.h
    clipboard.h
    command.h
    documentmanager.h
    paths.h
    powertabeditor.h
    recentfiles.h
    scorearea.h
    settings.h
    settingsmanager.h
    tuningdictionary.h
    viewoptions.h
)


set( moc_headers
    command.h
    powertabeditor.h
    recentfiles.h
    scorearea.h
)

pte_library(
    NAME pteapp
    SOURCES ${srcs}
    HEADERS ${headers}
    MOC_HEADERS ${moc_headers}
    DEPENDS
        pteactions
        pteaudio
        ptedialogs
        pteformats
        ptepainters
        ptewidgets
        pteutil
        Qt::Widgets
        Qt::PrintSupport
)
