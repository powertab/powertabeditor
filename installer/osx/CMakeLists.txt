project( installer )

# TODO - investigate whether it makes sense to build the Windows installer with
# CPack.

set( CPACK_PACKAGE_NAME "Power Tab Editor" )
set( CPACK_PACKAGE_VENDOR "Power Tab" )

set( CPACK_PACKAGE_VERSION_MAJOR "2" )
set( CPACK_PACKAGE_VERSION_MINOR "0" )
set( CPACK_PACKAGE_VERSION_PATCH "22" )
set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/license.txt" )

set( system_name "osx" )
set( CPACK_PACKAGE_FILE_NAME powertabeditor-${system_name} )

set( CPACK_GENERATOR DragNDrop )

# Locate macdeployqt.
set( macdeployqt_path "${PTE_QT_BIN_DIR}/macdeployqt" )
if( NOT EXISTS ${macdeployqt_path} )
    message( FATAL_ERROR "macdeployqt not found (${macdeployqt_path})" )
endif()

# Run macdeployqt on the bundle at install time.
install( CODE
    "execute_process(
        COMMAND ${macdeployqt_path} \"\${CMAKE_INSTALL_PREFIX}/Power Tab Editor.app\" -verbose=2
        RESULT_VARIABLE retval
    )
    if ( NOT retval EQUAL 0 )
        message( FATAL_ERROR \"Failed to run macdeployqt\" )
    endif ()
    "
)

# Fix up the executable's rpath, which macdeployqt6 doesn't seem to do anymore...
install( CODE
    "execute_process(
        COMMAND install_name_tool -add_rpath \"@executable_path/../Frameworks\" \"\${CMAKE_INSTALL_PREFIX}/Power Tab Editor.app/Contents/MacOS/Power Tab Editor\"
        RESULT_VARIABLE retval
    )
    if ( NOT retval EQUAL 0 )
        message( FATAL_ERROR \"Failed to run install_name_tool\" )
    endif ()
    "
)

# Install the Qt translations (macdeployqt doesn't handle this)
install( FILES ${PTE_QT_TRANSLATION_FILES}
    DESTINATION "${CPACK_PACKAGE_NAME}.app/Contents/Resources/translations"
)

# https://codingflamingo.com/blog/how-to-code-sign-and-notarize-net-console-app-for-mac is a good reference...
# Sign the .app bundle before the .dmg is created.
install( SCRIPT
    ${CMAKE_CURRENT_LIST_DIR}/sign_bundle.cmake
)

# Sign and notarize the .dmg after it is created.
set( CPACK_POST_BUILD_SCRIPTS
    ${CMAKE_CURRENT_LIST_DIR}/sign_dmg.cmake
)

include( CPack )
