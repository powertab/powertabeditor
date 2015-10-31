add_library( rapidjson INTERFACE IMPORTED )

set_target_properties(
    rapidjson PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${PTE_EXTERNAL_DIR}/rapidjson/include
)
