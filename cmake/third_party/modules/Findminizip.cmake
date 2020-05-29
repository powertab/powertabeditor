include( FindPackageHandleStandardArgs )

find_path(
    MINIZIP_INCLUDE_DIR
    NAMES zip.h
    PATH_SUFFIXES minizip
)
find_library( MINIZIP_LIBRARY minizip )

set( minizip_INCLUDE_DIRS ${MINIZIP_INCLUDE_DIR} )
set( minizip_LIBRARIES ${MINIZIP_LIBRARY} )

find_package_handle_standard_args( minizip DEFAULT_MSG
    MINIZIP_INCLUDE_DIR
    MINIZIP_LIBRARY
)

mark_as_advanced( MINIZIP_INCLUDE_DIR MINIZIP_LIBRARY )
