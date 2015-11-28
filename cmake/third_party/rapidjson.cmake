find_package( rapidjson )
if ( RAPIDJSON_FOUND )
    set( _rapidjson_includedirs ${RAPIDJSON_INCLUDE_DIRS} )
else ()
    set( _rapidjson_includedirs ${PTE_EXTERNAL_DIR}/rapidjson/include )
    message( STATUS "Using rapidjson headers from ${_rapidjson_includedirs}" )
endif ()

add_library( rapidjson INTERFACE IMPORTED )

set_target_properties(
    rapidjson PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${_rapidjson_includedirs}
)

unset( _rapidjson_includedirs )
