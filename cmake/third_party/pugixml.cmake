find_package( pugixml )

if ( PUGIXML_FOUND )
    add_library( pugixml IMPORTED SHARED )

    set_target_properties(
        pugixml PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${pugixml_INCLUDE_DIRS}
        IMPORTED_LOCATION ${pugixml_LIBRARIES}
    )
else ()
    set( _src_dir ${PTE_EXTERNAL_DIR}/pugixml/src )

    message( STATUS "Using pugixml library from ${_src_dir}" )

    add_library( pugixml
        ${_src_dir}/pugixml.cpp
        ${_src_dir}/pugixml.hpp
    )

    set_target_properties(
        pugixml PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${_src_dir}
    )

    # Set folder name for Visual Studio projects.
    set_target_properties(
        pugixml PROPERTIES
        FOLDER ${PTE_EXTERNAL_FOLDER_NAME}
    )

    unset( _src_dir )
endif ()
