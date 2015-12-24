project( pteapp )

set( _version ${PTE_VERSION} )

# Get a version number for development builds.
include( FindGit )
if ( GIT_FOUND )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --long --always
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE git_revision
        ERROR_VARIABLE error_var
        RESULT_VARIABLE result_var
    )

    if ( result_var EQUAL 0 )
        string( STRIP "${git_revision}" git_revision )
        set( _version ${git_revision} )
    endif ()
endif ()

message( STATUS "Version number: ${_version}" )
add_definitions( -DVERSION=${_version} )

set( srcs
    appinfo.cpp
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

    pubsub/playerpubsub.h
    pubsub/pubsub.h
    pubsub/clickpubsub.h
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
        pteutil
        boost_filesystem
        Qt5::Widgets
        Qt5::PrintSupport
)
