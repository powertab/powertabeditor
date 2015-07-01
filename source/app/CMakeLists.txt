cmake_minimum_required(VERSION 2.8.9)

# Get a version number.
include(FindGit)
execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --long --always
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		OUTPUT_VARIABLE MY_WC_REVISION)
string(STRIP "${MY_WC_REVISION}" MY_WC_REVISION)

add_definitions(-DVERSION=${MY_WC_REVISION})

add_library(pteapp
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

qt5_use_modules(pteapp Widgets Concurrent PrintSupport)
add_precompiled_header(pteapp)
