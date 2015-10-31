include( CMakeParseArguments )

function( pte_executable )
    cmake_parse_arguments( PTE_EXE "" "NAME" "SOURCES;RESOURCES;DEPENDS" ${ARGN} )

    set( generated_files )
    if ( PTE_EXE_RESOURCES )
        qt5_add_resources( generated_files ${PTE_EXE_RESOURCES} )
    endif ()

    add_executable( ${PTE_EXE_NAME}
        WIN32
        ${PTE_EXE_SOURCES}
        ${generated_files}
    )

    target_link_libraries( ${PTE_EXE_NAME} ${PTE_EXE_DEPENDS} )

    # Set output directory for executables.
    set_target_properties( powertabeditor PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PTE_BIN_DIR}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PTE_BIN_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PTE_BIN_DIR}
    )

    # Copy dlls to the bin directory.
    if ( PLATFORM_WIN )
        pte_find_dlls( ${PTE_EXE_NAME} shared_libs )

        foreach( lib ${shared_libs} )
            add_custom_command(
                TARGET ${PTE_EXE_NAME}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${lib} ${PTE_BIN_DIR}
            )
        endforeach ()
    endif ()
endfunction ()

# Recursively find shared libraries that a target depends on.
function ( pte_find_dlls target output_list )
    if ( NOT TARGET ${target} )
        return ()
    endif ()

    get_target_property( child_targets ${target} INTERFACE_LINK_LIBRARIES )
    set( all_libs )
    if ( child_targets )
        foreach ( child_target ${child_targets} )
            pte_find_dlls( ${child_target} child_libs )
            list( APPEND all_libs ${child_libs} )
        endforeach ()
    endif ()

    get_target_property( target_type ${target} TYPE )
    if ( ${target_type} MATCHES SHARED_LIBRARY )
        list( APPEND all_libs "$<TARGET_FILE:${target}>" )
    endif ()

    if ( all_libs )
        list( REMOVE_DUPLICATES all_libs )
    endif ()
    set( ${output_list} ${all_libs} PARENT_SCOPE )
endfunction ()
