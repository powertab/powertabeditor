set( PLATFORM_WIN )
set( PLATFORM_OSX )
set( PLATFORM_LINUX )
set( COMPILER_CLANG )
set( COMPILER_GCC )

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
elseif ( CMAKE_COMPILER_IS_GNUCC )
    set( COMPILER_GCC TRUE )
endif ()

if ( PLATFORM_WIN )
    # Enable folders in Visual Studio projects.
    set_property ( GLOBAL PROPERTY USE_FOLDERS ON )
endif ()

option( PTE_BUILD_SNAP "Enable specific error messages for common runtime errors due to the snap package's sandboxing." OFF )
