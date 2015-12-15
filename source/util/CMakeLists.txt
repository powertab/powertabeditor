project( pteutil )

set( platform_srcs )
if ( PLATFORM_OSX )
    set( platform_srcs settingstree_plist.mm )
endif ()

set( srcs
    rapidjson_iostreams.cpp
    settingstree.cpp

    ${platform_srcs}
)

set( headers
    rapidjson_iostreams.h
    settingstree.h
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
        boost
        rapidjson
        ${platform_depends}
)
