project( ptedata )

set( files
    tunings.json
)

set( output_files )
foreach( file ${files} )
    list( APPEND output_files ${PTE_DATA_DIR}/${file} )
endforeach ()

add_custom_command(
    OUTPUT ${output_files}
    DEPENDS ${files}
    COMMAND ${CMAKE_COMMAND} -E copy ${files} ${PTE_DATA_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    ptedata ALL
    DEPENDS ${output_files}
)

set_target_properties( ptedata PROPERTIES
    FOLDER data
)
