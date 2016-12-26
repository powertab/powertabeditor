include( CMakeParseArguments )

function( pte_library )
    cmake_parse_arguments( PTE_LIB ""
        "NAME;PCH"
        "SOURCES;HEADERS;MOC_HEADERS;FORMS;DEPENDS;PCH_EXCLUDE"
        ${ARGN}
    )

    set( generated_headers )
    set( moc_files )
    if ( PTE_LIB_FORMS )
        qt5_wrap_ui( generated_headers ${PTE_LIB_FORMS} )
    endif ()
    if ( PTE_LIB_MOC_HEADERS )
        qt5_wrap_cpp( moc_files ${PTE_LIB_MOC_HEADERS} )
    endif ()

    add_library( ${PTE_LIB_NAME} STATIC
        ${PTE_LIB_SOURCES}
        ${PTE_LIB_HEADERS}
        ${generated_headers}
        ${moc_files}
    )

    pte_add_compile_flags( ${PTE_LIB_NAME} )

    target_include_directories( ${PTE_LIB_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    )

    target_link_libraries( ${PTE_LIB_NAME} ${PTE_LIB_DEPENDS} )

    # Set folder name for Visual Studio projects.
    set_target_properties( ${PTE_LIB_NAME} PROPERTIES
        FOLDER libraries
    )
    
    # Set up precompiled header.
    if ( PTE_LIB_PCH )
        pte_precompiled_header(
            TARGET ${PTE_LIB_NAME}
            HEADER ${PTE_LIB_PCH}
            EXCLUDE_FILES ${PTE_LIB_PCH_EXCLUDE}
        )
    endif ()
endfunction ()
