set( _boost_libs
    date_time
    filesystem
    iostreams
    program_options
    regex
    system
)

if ( PLATFORM_WIN )
    list (APPEND _boost_libs zlib )
endif ()

find_package(
    Boost 1.55.0 REQUIRED
    COMPONENTS ${_boost_libs}
)

# Set up imported targets.
add_library( boost INTERFACE IMPORTED )
set_target_properties(
    boost PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
    INTERFACE_COMPILE_DEFINITIONS "BOOST_ALL_DYN_LINK;BOOST_ALL_NO_LIB"
)

function ( pte_boost_library libname )
    string( TOUPPER ${libname} libname_upper )
    
    # Figure out the .dll paths.
    
    set( implib_debug ${Boost_${libname_upper}_LIBRARY_DEBUG} )
    set( implib_release ${Boost_${libname_upper}_LIBRARY_RELEASE} )
    
    get_filename_component( boost_dir ${implib_debug} DIRECTORY )
    get_filename_component( basename_debug ${implib_debug} NAME_WE )
    get_filename_component( basename_release ${implib_release} NAME_WE )

    set( lib_debug ${boost_dir}/${basename_debug}${CMAKE_SHARED_LIBRARY_SUFFIX} )
    set( lib_release ${boost_dir}/${basename_release}${CMAKE_SHARED_LIBRARY_SUFFIX} )

    add_library( boost_${libname} SHARED IMPORTED )
    set_target_properties(
        boost_${libname} 
        PROPERTIES 
        INTERFACE_LINK_LIBRARIES boost
        IMPORTED_LOCATION_DEBUG ${lib_debug} 
        IMPORTED_LOCATION_RELEASE ${lib_release} 
        IMPORTED_IMPLIB_DEBUG ${implib_debug} 
        IMPORTED_IMPLIB_RELEASE ${implib_release}
    )

    if ( NOT PLATFORM_WIN )
        set_target_properties(
            boost_${libname}
            PROPERTIES
            IMPORTED_LOCATION ${Boost_${libname_upper}_LIBRARY}
        )
    endif ()
endfunction ()

foreach ( lib ${_boost_libs} )
    pte_boost_library( ${lib} )
endforeach ()

# boost_filesystem depends on boost_system
set_target_properties(
    boost_filesystem
    PROPERTIES 
    INTERFACE_LINK_LIBRARIES boost_system
)

unset( _boost_libs )
