include( ${CMAKE_CURRENT_LIST_DIR}/sign.cmake )

set( dmg_file ${CPACK_PACKAGE_FILES} )

# First, sign the .dmg file.
pte_sign( ${dmg_file} )

# Notarize and staple the installer.
message( "Running notarytool for ${dmg_file} ..." )
execute_process(
    COMMAND xcrun notarytool submit --wait --apple-id=cameronwhite91@gmail.com --password "$ENV{MAC_DEV_PASSWORD}" --team-id D5G6C56TBH ${dmg_file}
    RESULT_VARIABLE retval
)
if ( NOT retval EQUAL 0 )
    message( FATAL_ERROR "Failed to run notarytool" )
endif ()

message( "Running stapler ..." )
execute_process(
    COMMAND xcrun stapler staple ${dmg_file}
    RESULT_VARIABLE retval
)
if ( NOT retval EQUAL 0 )
    message( FATAL_ERROR "Failed to run stapler" )
endif ()
