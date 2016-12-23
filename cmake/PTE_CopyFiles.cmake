include( CMakeParseArguments )

function ( pte_copyfiles )
    cmake_parse_arguments( PTE_DATA "" "NAME;DESTINATION;INSTALL" "FILES" ${ARGN} )

    set( dest_dir ${PTE_DATA_DESTINATION} )

    set( output_files )
    foreach( src_file ${PTE_DATA_FILES} )
        get_filename_component( filename ${src_file} NAME )
        set( output_file ${dest_dir}/${filename} )
        list( APPEND output_files ${output_file} )

        add_custom_command(
            OUTPUT ${output_file}
            DEPENDS ${src_file}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${dest_dir}
            COMMAND ${CMAKE_COMMAND} -E copy ${src_file} ${dest_dir}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endforeach ()

    add_custom_target(
        ${PTE_DATA_NAME} ALL
        DEPENDS ${output_files}
    )

    set_target_properties( ${PTE_DATA_NAME} PROPERTIES
        FOLDER data
    )

    if ( PTE_DATA_INSTALL )
        install(
            FILES ${PTE_DATA_FILES}
            DESTINATION ${PTE_DATA_INSTALL}
        )
    endif ()
endfunction ()
