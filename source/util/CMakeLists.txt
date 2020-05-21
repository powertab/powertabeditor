project( pteutil )

set( platform_srcs )
if ( PLATFORM_OSX )
    set( platform_srcs settingstree_plist.mm )
endif ()

set( srcs
    settingstree.cpp

    ${platform_srcs}
)

set( headers
    settingstree.h
    tostring.h
)

set( platform_depends )
if ( PLATFORM_OSX )
    find_library( foundation_lib Foundation )
    set( platform_depends ${foundation_lib} )
endif ()

pte_library(
    NAME pteutil
    SOURCES ${srcs}
    HEADERS ${headers}
    DEPENDS
        PUBLIC Boost::headers
        PRIVATE rapidjson::rapidjson ${platform_depends}
)
