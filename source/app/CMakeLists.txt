cmake_minimum_required(VERSION 2.8.9)

include_directories(.)

# Get the current SVN revision number, and also handle git-svn repositories.
include(FindSubversion)
include(FindGit)
if (EXISTS ${CMAKE_SOURCE_DIR}/.svn/)
    Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} MY)
else()
    execute_process(COMMAND ${GIT_EXECUTABLE} --no-pager svn log --oneline --limit 1
        OUTPUT_VARIABLE GIT_REVISION_LOG)
    string(REGEX MATCH "[0-9]+" MY_WC_REVISION "${GIT_REVISION_LOG}")
endif()

add_definitions(-DSVN_REVISION=${MY_WC_REVISION})

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
