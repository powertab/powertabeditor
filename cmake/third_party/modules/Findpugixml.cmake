include( FindPackageHandleStandardArgs )

find_path( PUGIXML_INCLUDE_DIR NAMES pugixml.hpp )
find_library( PUGIXML_LIBRARY pugixml )

set( pugixml_INCLUDE_DIRS ${PUGIXML_INCLUDE_DIR} )
set( pugixml_LIBRARIES ${PUGIXML_LIBRARY} )

find_package_handle_standard_args( pugixml DEFAULT_MSG
    PUGIXML_INCLUDE_DIR
    PUGIXML_LIBRARY
)

mark_as_advanced( PUGIXML_INCLUDE_DIR PUGIXML_LIBRARY )
