add_library( Catch INTERFACE IMPORTED )

set_target_properties(
    Catch PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${PTE_EXTERNAL_DIR}/Catch/single_include
)
