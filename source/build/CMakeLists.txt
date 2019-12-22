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

set( platform_deps )
if ( PLATFORM_LINUX )
    # For boost::stacktrace
    set( platform_deps dl )
endif ()

pte_executable(
    NAME powertabeditor
    INSTALL
    SOURCES ${srcs} ${rc_file}
    RESOURCES ${resources}
    DEPENDS
        Boost::program_options
        pteapp
        ptedialogs
        Qt5::Network
        Qt5::Widgets
        ${platform_deps}
    PLUGINS
        ${QT5_PLUGINS}
)
