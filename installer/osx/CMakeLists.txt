project( installer )

# TODO - investigate whether it makes sense to build the Windows installer with
# CPack.

set( CPACK_PACKAGE_NAME "Power Tab Editor" )
set( CPACK_PACKAGE_VENDOR "Power Tab" )

set( CPACK_PACKAGE_VERSION_MAJOR "1" )
set( CPACK_PACKAGE_VERSION_MINOR "98" )
set( CPACK_PACKAGE_VERSION_PATCH "12" )
set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/license.txt" )

set( system_name "osx" )
set( CPACK_PACKAGE_FILE_NAME powertabeditor-${system_name} )

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

include( CPack )
