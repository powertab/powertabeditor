include( CMakeParseArguments )

option( ENABLE_PCH "Enable precompiled headers." OFF )

function( pte_precompiled_header )
    cmake_parse_arguments( PTE_PCH "" "TARGET;HEADER" "EXCLUDE_FILES" ${ARGN} )
    
    if ( PLATFORM_WIN AND ENABLE_PCH )
        get_filename_component( pch_basename ${PTE_PCH_HEADER} NAME_WE )
        set( pch_src ${CMAKE_CURRENT_BINARY_DIR}/${pch_basename}.cpp )
        set( pch_output ${CMAKE_CURRENT_BINARY_DIR}/${pch_basename}.pch )
        
        # Generate the stub .cpp file.
        file( WRITE ${pch_src} "#include \"${PTE_PCH_HEADER}\"" )
        source_group( "Precompiled Header" FILES ${pch_src} ${PTE_PCH_HEADER} )
        
        set_source_files_properties( ${pch_src} PROPERTIES
            COMPILE_FLAGS "/Yc\"${PTE_PCH_HEADER}\" /Fp\"${pch_output}\" /I\"${CMAKE_CURRENT_SOURCE_DIR}\""
        )
        
        # Find the other .cpp files in the project that will use the precompiled header.
        get_target_property( srcs ${PTE_PCH_TARGET} SOURCES )
        set( filtered_srcs )
        foreach ( src ${srcs} )
            list( FIND PTE_PCH_EXCLUDE_FILES ${src} idx )
            if ( ${idx} EQUAL -1 )
                list( APPEND filtered_srcs ${src} )
            endif ()
        endforeach ()
        
        set_source_files_properties( ${filtered_srcs} PROPERTIES
            COMPILE_FLAGS "/Yu\"${PTE_PCH_HEADER}\" /FI\"${PTE_PCH_HEADER}\" /Fp\"${pch_output}\""
            OBJECT_DEPENDS ${pch_output}
        )
        
        # Add the precompiled header to the project.
        list( APPEND srcs ${PTE_PCH_HEADER} ${pch_src} )
        set_target_properties( ${PTE_PCH_TARGET} PROPERTIES SOURCES "${srcs}" )
    endif ()
endfunction ()