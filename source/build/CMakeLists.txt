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
        pteapp
        ptedialogs
        Qt::Network
        Qt::Widgets
        ${platform_deps}
    PLUGINS
        ${QT_PLUGINS}
)

if ( PLATFORM_OSX )
    # Configure the .app bundle for OSX.
    set_target_properties( powertabeditor PROPERTIES
        OUTPUT_NAME "Power Tab Editor"
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/osx/Info.plist.in
    )

    set( icon_file ${CMAKE_CURRENT_SOURCE_DIR}/osx/powertabeditor.icns )
    set( tunings_file ${CMAKE_CURRENT_SOURCE_DIR}//data/tunings.json )

    target_sources( powertabeditor PRIVATE
        ${icon_file}
        ${tunings_file}
    )
    set_source_files_properties( ${icon_file} ${tunings_file} PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources
    )
else ()
    # Copy the tuning dictionary into the build directory, and set its install
    # path.
    set( files
        data/tunings.json
    )

    pte_copyfiles(
        NAME ptedata
        DESTINATION ${PTE_DATA_DIR}
        INSTALL ${PTE_DATA_INSTALL_DIR}
        FILES ${files}
    )

    add_dependencies(powertabeditor ptedata)
endif ()
