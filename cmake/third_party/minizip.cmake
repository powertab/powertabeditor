find_package( minizip REQUIRED )

add_library( minizip::minizip INTERFACE IMPORTED )
target_include_directories( minizip::minizip INTERFACE ${minizip_INCLUDE_DIRS} )
target_link_libraries( minizip::minizip
    INTERFACE ${minizip_LIBRARIES}
)

if ( PLATFORM_WIN )
    # On Windows, linking against zlib also seems to be required.
    find_package( ZLIB REQUIRED )
    target_link_libraries( minizip::minizip
        INTERFACE ${ZLIB_LIBRARIES}
    )
endif ()
