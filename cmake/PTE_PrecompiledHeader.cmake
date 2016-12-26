include( CMakeParseArguments )

option( ENABLE_PCH "Enable precompiled headers." OFF )

macro( pte_join_flags flags prefix )
    set( ${flags} "$<$<BOOL:${${flags}}>:${prefix}$<JOIN:${${flags}},\t${prefix}>>" )
endmacro ()

function( pte_precompiled_header )
    cmake_parse_arguments( PTE_PCH "" "TARGET;HEADER" "EXCLUDE_FILES" ${ARGN} )

    if ( NOT ENABLE_PCH )
        return ()
    endif ()

    get_filename_component( pch_basename ${PTE_PCH_HEADER} NAME_WE )
        
    # Find the other .cpp files in the project that will use the precompiled header.
    get_target_property( srcs ${PTE_PCH_TARGET} SOURCES )
    set( filtered_srcs )
    foreach ( src ${srcs} )
        list( FIND PTE_PCH_EXCLUDE_FILES ${src} idx )
        if ( ${idx} EQUAL -1 )
            list( APPEND filtered_srcs ${src} )
        endif ()
    endforeach ()
    
    if ( PLATFORM_WIN )
        set( pch_src ${CMAKE_CURRENT_BINARY_DIR}/${pch_basename}.cpp )
        set( pch_output ${CMAKE_CURRENT_BINARY_DIR}/${pch_basename}.pch )
        
        # Generate the stub .cpp file.
        file( WRITE ${pch_src} "#include \"${PTE_PCH_HEADER}\"" )
        source_group( "Precompiled Header" FILES ${pch_src} ${PTE_PCH_HEADER} )
        
        set_source_files_properties( ${pch_src} PROPERTIES
            COMPILE_FLAGS "/Yc\"${PTE_PCH_HEADER}\" /Fp\"${pch_output}\" /I\"${CMAKE_CURRENT_SOURCE_DIR}\""
        )
        
        set_source_files_properties( ${filtered_srcs} PROPERTIES
            COMPILE_FLAGS "/Yu\"${PTE_PCH_HEADER}\" /FI\"${PTE_PCH_HEADER}\" /Fp\"${pch_output}\""
            OBJECT_DEPENDS ${pch_output}
        )
        
        # Add the precompiled header to the project.
        list( APPEND srcs ${PTE_PCH_HEADER} ${pch_src} )
        set_target_properties( ${PTE_PCH_TARGET} PROPERTIES SOURCES "${srcs}" )
    elseif ( PLATFORM_OSX )
        # TODO - support gcc on Linux.
        set( pch_output ${CMAKE_CURRENT_BINARY_DIR}/${pch_basename}.h.pch )

        # Figure out the compiler flags used for this target.
        set( compile_options "$<TARGET_PROPERTY:${PTE_PCH_TARGET},COMPILE_OPTIONS>" )
        pte_join_flags( compile_options "" )

        set( include_dirs "$<TARGET_PROPERTY:${PTE_PCH_TARGET},INCLUDE_DIRECTORIES>" )
        pte_join_flags( include_dirs "-I" )

        set( definitions "$<TARGET_PROPERTY:${PTE_PCH_TARGET},COMPILE_DEFINITIONS>" )
        pte_join_flags( definitions "-D" )

        # Generate the precompiled header.
        add_custom_command(
            OUTPUT ${pch_output}
            DEPENDS ${PTE_PCH_HEADER}
            COMMAND ${CMAKE_CXX_COMPILER}
                    ${definitions} ${include_dirs} ${compile_options}
                    -isysroot ${CMAKE_OSX_SYSROOT} -std=c++11
                    -x c++-header
                    -o ${pch_output}
                    ${PTE_PCH_HEADER}
            COMMENT "Building precompiled header for ${PTE_PCH_TARGET}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )

        # Force include the precompiled header.
        set_source_files_properties( ${filtered_srcs} PROPERTIES
            COMPILE_FLAGS "-Winvalid-pch -include-pch ${pch_output}"
            OBJECT_DEPENDS ${pch_output}
        )
    endif ()
endfunction ()
