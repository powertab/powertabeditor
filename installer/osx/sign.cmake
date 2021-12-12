include_guard()

set( sign_identity "Developer ID Application: Cameron White (D5G6C56TBH)" )

function( pte_sign filename )
    message( "Running codesign for ${filename} ..." )
    execute_process(
        COMMAND codesign --deep --force --timestamp --options=runtime --sign "${sign_identity}" "${filename}"
        RESULT_VARIABLE retval
    )
    if ( NOT retval EQUAL 0 )
        message( FATAL_ERROR "Failed to run codesign" )
    endif ()
endfunction()
