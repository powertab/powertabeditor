include( CMakeParseArguments )

option( PTE_ENABLE_PCH "Enable precompiled headers (requires CMake 3.16)." OFF )

function( pte_precompiled_header )
    cmake_parse_arguments( PTE_PCH "" "TARGET;HEADER" "EXCLUDE_FILES" ${ARGN} )

    if ( NOT PTE_ENABLE_PCH )
        return ()
    endif ()

    target_precompile_headers( ${PTE_PCH_TARGET}
        PUBLIC ${PTE_PCH_HEADER}
    )

    if ( PTE_PCH_EXCLUDE_FILES )
        set_source_files_properties( ${PTE_PCH_EXCLUDE_FILES} PROPERTIES
            SKIP_PRECOMPILE_HEADERS TRUE
        )
    endif ()

endfunction ()
