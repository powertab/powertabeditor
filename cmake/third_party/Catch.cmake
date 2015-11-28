find_package( Catch )

if ( CATCH_FOUND )
    set( _catch_dir ${Catch_INCLUDE_DIRS} )
else ()
    set( _catch_dir ${PTE_EXTERNAL_DIR}/Catch/single_include )
    message( STATUS "Using Catch library from ${_catch_dir}" )
endif ()

add_library( Catch INTERFACE IMPORTED )

set_target_properties(
    Catch PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${_catch_dir}
)

unset( _catch_dir )
