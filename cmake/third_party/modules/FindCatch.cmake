include( FindPackageHandleStandardArgs )

find_path( CATCH_INCLUDE_DIR NAMES catch.hpp)
set( Catch_INCLUDE_DIRS ${CATCH_INCLUDE_DIR} )

find_package_handle_standard_args( Catch DEFAULT_MSG CATCH_INCLUDE_DIR )

mark_as_advanced( CATCH_INCLUDE_DIR )
