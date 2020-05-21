find_package( rapidjson REQUIRED )

add_library( rapidjson::rapidjson INTERFACE IMPORTED )
target_include_directories( rapidjson::rapidjson
    INTERFACE ${RAPIDJSON_INCLUDE_DIRS}
)
