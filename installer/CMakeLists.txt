project( installer )

set( CPACK_PACKAGE_NAME "Power Tab Editor" )
set( CPACK_PACKAGE_VENDOR "Power Tab" )

set( CPACK_PACKAGE_VERSION_MAJOR "1" )
set( CPACK_PACKAGE_VERSION_MINOR "98" )
set( CPACK_PACKAGE_VERSION_PATCH "11" )

if ( PLATFORM_WIN )
    set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/license.rtf" )
else ()
    set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/license.txt" )
endif ()

set( system_name )
if ( PLATFORM_WIN )
    set( system_name "-win64" )
elseif ( PLATFORM_OSX )
    set( system_name "-osx" )
endif ()

set( CPACK_PACKAGE_FILE_NAME powertabeditor-${PTE_VERSION}${system_name} )

if ( PLATFORM_WIN )
    set( CPACK_PACKAGE_INSTALL_DIRECTORY "Power Tab/Power Tab Editor" )
    set( CPACK_WIX_UPGRADE_GUID "6cab03ff-a31b-4c76-a4d1-20a37575896a" )
elseif ( PLATFORM_OSX )
    set( CPACK_GENERATOR DragNDrop )

    # Locate macdeployqt
    get_target_property( uic_path Qt5::uic IMPORTED_LOCATION )
    get_filename_component( qt_bin_dir ${uic_path} DIRECTORY)
    set( macdeployqt_path "${qt_bin_dir}/macdeployqt" )
    if( NOT EXISTS ${macdeployqt_path} )
        message( FATAL_ERROR "macdeployqt not found (${macdeployqt_path})" )
    endif()

    # Run macdeployqt on the bundle at install time.
    install( CODE
        "execute_process(
            COMMAND ${macdeployqt_path} \"\${CMAKE_INSTALL_PREFIX}/Power Tab Editor.app\"
            RESULT_VARIABLE retval
        )
        if ( NOT retval EQUAL 0 )
            message( FATAL_ERROR \"Failed to run macdeployqt\" )
        endif ()
        "
    )
endif ()

include( CPack )
