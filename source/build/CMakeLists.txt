project( powertabeditor )

set( srcs
    main.cpp
)

set( resources
    resources.qrc
)

set( rc_file )
if ( PLATFORM_WIN )
    set( rc_file build.rc )
endif ()

pte_executable(
    NAME powertabeditor
    INSTALL
    SOURCES ${srcs} ${rc_file}
    RESOURCES ${resources}
    DEPENDS
        boost_program_options
        pteapp
        ptedialogs
        Qt5::Network
        Qt5::Widgets
        withershins
    PLUGINS
        ${QT5_PLUGINS}
)
