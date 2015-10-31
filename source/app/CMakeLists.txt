project( pteapp )

# Get a version number.
include( FindGit )
execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --long --always
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_VARIABLE MY_WC_REVISION
)
string( STRIP "${MY_WC_REVISION}" MY_WC_REVISION )

add_definitions( -DVERSION=${MY_WC_REVISION} )

set( srcs
    appinfo.cpp
    caret.cpp
    clipboard.cpp
    command.cpp
    documentmanager.cpp
    powertabeditor.cpp
    recentfiles.cpp
    scorearea.cpp
    settings.cpp
    tuningdictionary.cpp
    viewoptions.cpp
)

set( headers
    appinfo.h
    caret.h
    clipboard.h
    command.h
    documentmanager.h
    powertabeditor.h
    recentfiles.h
    scorearea.h
    settings.h
    tuningdictionary.h
    viewoptions.h

    pubsub/playerpubsub.h
    pubsub/pubsub.h
    pubsub/clickpubsub.h
    pubsub/settingspubsub.h
)


set( moc_headers
    command.h
    powertabeditor.h
    recentfiles.h
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
        Qt5::Widgets
        Qt5::PrintSupport
        Qt5::Concurrent
)
