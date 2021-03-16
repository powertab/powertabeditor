project( pteutil )

set( _version ${PTE_VERSION} )

# Get a version number for development builds.
include( FindGit )
if ( GIT_FOUND )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --long --always
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE git_revision
        ERROR_VARIABLE error_var
        RESULT_VARIABLE result_var
    )

    if ( result_var EQUAL 0 )
        string( STRIP "${git_revision}" git_revision )
        set( _version ${git_revision} )
    endif ()
endif ()

message( STATUS "Version number: ${_version}" )
add_definitions( -DVERSION=${_version} )

set( platform_srcs )
if ( PLATFORM_OSX )
    set( platform_srcs settingstree_plist.mm )
endif ()

set( srcs
    settingstree.cpp
    version.cpp

    ${platform_srcs}
)

set( headers
    date.h
    settingstree.h
    tostring.h
    scopeexit.h
    version.h
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
