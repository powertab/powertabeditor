include( FindPackageHandleStandardArgs )

find_path( WITHERSHINS_INCLUDE_DIR NAMES withershins.hpp )
find_library( WITHERSHINS_LIBRARY withershins )

set( withershins_INCLUDE_DIRS ${WITHERSHINS_INCLUDE_DIR} )
set( withershins_LIBRARIES ${WITHERSHINS_LIBRARY} )

find_package_handle_standard_args( withershins DEFAULT_MSG
    WITHERSHINS_INCLUDE_DIR
    WITHERSHINS_LIBRARY
)

mark_as_advanced( WITHERSHINS_INCLUDE_DIR WITHERSHINS_LIBRARY )
