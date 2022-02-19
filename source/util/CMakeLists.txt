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
add_definitions( -DPTE_VERSION=${_version} )

set( platform_srcs )
if ( PLATFORM_OSX )
    set( platform_srcs settingstree_plist.mm )
    # This is Objective-C++, so don't merge with other files for unity builds.
    set_source_files_properties( settingstree_plist.mm PROPERTIES
        SKIP_UNITY_BUILD_INCLUSION TRUE
    )
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
        PRIVATE nlohmann_json::nlohmann_json ${platform_depends}
)
