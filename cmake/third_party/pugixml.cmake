set( _src_dir ${PTE_EXTERNAL_DIR}/pugixml/src )

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
