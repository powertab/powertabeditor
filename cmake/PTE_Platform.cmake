set( PLATFORM_WIN )
set( PLATFORM_OSX )
set( PLATFORM_LINUX )
set( COMPILER_CLANG )

if ( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
    set( PLATFORM_WIN TRUE )
elseif ( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
    set( PLATFORM_OSX TRUE )
elseif ( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
    set( PLATFORM_LINUX TRUE )
else ()
    message( FATAL_ERROR "Unknown platform." )
endif ()

if ( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" )
    set( COMPILER_CLANG TRUE )
endif ()

if ( PLATFORM_WIN )
    # Enable folders in Visual Studio projects.
    set_property ( GLOBAL PROPERTY USE_FOLDERS ON )
endif ()
