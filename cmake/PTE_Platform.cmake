set( PLATFORM_WIN )
set( PLATFORM_OSX )
set( PLATFORM_LINUX )

if ( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
    set( PLATFORM_WIN TRUE )
elseif ( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
    set( PLATFORM_OSX TRUE )
elseif ( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
    set( PLATFORM_LINUX TRUE )
else ()
    message( FATAL_ERROR "Unknown platform." )
endif ()

if ( PLATFORM_WIN )
    # Enable folders in Visual Studio projects.
    set_property ( GLOBAL PROPERTY USE_FOLDERS ON )
endif ()
