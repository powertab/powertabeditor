include( CMakeParseArguments )

function ( pte_copyfiles )
    cmake_parse_arguments( PTE_DATA "" "NAME;DESTINATION" "FILES" ${ARGN} )

    set( output_files )
    foreach( src_file ${PTE_DATA_FILES} )
        get_filename_component( filename ${src_file} NAME )
        set( output_file ${PTE_DATA_DESTINATION}/${filename} )
        list( APPEND output_files ${output_file} )

        add_custom_command(
            OUTPUT ${output_file}
            DEPENDS ${src_file}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${PTE_DATA_DESTINATION}
            COMMAND ${CMAKE_COMMAND} -E copy ${src_file} ${PTE_DATA_DESTINATION}
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
endfunction ()
