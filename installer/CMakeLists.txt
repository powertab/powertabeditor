project( installer )

set( CPACK_PACKAGE_NAME "Power Tab Editor" )
set( CPACK_PACKAGE_VENDOR "Power Tab" )

set( CPACK_PACKAGE_VERSION_MAJOR "1" )
set( CPACK_PACKAGE_VERSION_MINOR "98" )
set( CPACK_PACKAGE_VERSION_PATCH "9" )
set( pretty_version 2.0.0-alpha9 )

set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/license.rtf" )

set( system_name )
if ( PLATFORM_WIN )
    if ( CMAKE_CL_64 )
        set( system_name "-win64" )
    else ()
        set( system_name "-win32" )
    endif ()
endif ()

set( CPACK_PACKAGE_FILE_NAME powertabeditor-${pretty_version}${system_name} )

if ( PLATFORM_WIN )
    set( CPACK_PACKAGE_INSTALL_DIRECTORY "Power Tab/Power Tab Editor" )
    set( CPACK_WIX_UPGRADE_GUID "6cab03ff-a31b-4c76-a4d1-20a37575896a" )
endif ()

include( CPack )