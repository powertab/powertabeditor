cmake_minimum_required(VERSION 2.8.9)

# Get a version number.
include(FindGit)
execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --long OUTPUT_VARIABLE MY_WC_REVISION)
string(STRIP "${MY_WC_REVISION}" MY_WC_REVISION)

add_definitions(-DVERSION=${MY_WC_REVISION})

add_library(pteapp
    caret.cpp
    clipboard.cpp
    command.cpp
    documentmanager.cpp
    powertabeditor.cpp
    recentfiles.cpp
    scorearea.cpp
    settings.cpp
    tuningdictionary.cpp

    caret.h
    clipboard.h
    command.h
    documentmanager.h
    powertabeditor.h
    recentfiles.h
    scorearea.h
    settings.h
    tuningdictionary.h
    pubsub/playerpubsub.h
    pubsub/pubsub.h
    pubsub/scorelocationpubsub.h
    pubsub/settingspubsub.h
    pubsub/staffpubsub.h
)

qt5_use_modules(pteapp Widgets Concurrent)
cotire(pteapp)
