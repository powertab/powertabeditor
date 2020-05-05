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
    Boost 1.54.0 REQUIRED
    COMPONENTS ${_boost_libs}
)

# Set up imported targets if not already provided.
if ( NOT TARGET Boost::headers )
    add_library( Boost::headers INTERFACE IMPORTED )
    set_target_properties(
        Boost::headers PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
        INTERFACE_COMPILE_DEFINITIONS "BOOST_ALL_DYN_LINK;BOOST_ALL_NO_LIB"
    )
endif ()

function ( pte_boost_library libname )
    string( TOUPPER ${libname} libname_upper )

    # Newer Boost versions already provide an imported target.
    if ( NOT TARGET Boost::${libname} )
        # Figure out the .dll paths.
        set( implib_release ${Boost_${libname_upper}_LIBRARY_RELEASE} )
        set( implib_debug ${Boost_${libname_upper}_LIBRARY_DEBUG} )
        if ( NOT implib_debug )
            set( implib_debug ${implib_release} )
        endif ()

        get_filename_component( boost_dir ${implib_debug} DIRECTORY )
        get_filename_component( basename_debug ${implib_debug} NAME_WE )
        get_filename_component( basename_release ${implib_release} NAME_WE )

        set( lib_debug ${boost_dir}/${basename_debug}${CMAKE_SHARED_LIBRARY_SUFFIX} )
        set( lib_release ${boost_dir}/${basename_release}${CMAKE_SHARED_LIBRARY_SUFFIX} )

        add_library( Boost::${libname} SHARED IMPORTED )
        set_target_properties(
            Boost::${libname} 
            PROPERTIES 
            INTERFACE_LINK_LIBRARIES Boost::headers
            IMPORTED_LOCATION_DEBUG ${lib_debug} 
            IMPORTED_LOCATION_RELEASE ${lib_release} 
            IMPORTED_IMPLIB_DEBUG ${implib_debug} 
            IMPORTED_IMPLIB_RELEASE ${implib_release}
        )

        if ( NOT PLATFORM_WIN )
            set_target_properties(
                Boost::${libname}
                PROPERTIES
                IMPORTED_LOCATION ${Boost_${libname_upper}_LIBRARY}
            )
        endif ()

        if ( ${libname} STREQUAL "filesystem" )
            # boost_filesystem depends on boost_system
            set_target_properties(
                Boost::filesystem
                PROPERTIES
                INTERFACE_LINK_LIBRARIES Boost::system
            )
        endif ()
    endif ()
endfunction ()

foreach ( lib ${_boost_libs} )
    pte_boost_library( ${lib} )
endforeach ()

unset( _boost_libs )
