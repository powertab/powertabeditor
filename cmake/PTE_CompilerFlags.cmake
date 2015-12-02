# Always warn about using deprecated Qt functions.
add_definitions( -DQT_DEPRECATED_WARNINGS )

# Use C++11.
set( CMAKE_CXX_STANDARD 11 )

if ( PLATFORM_WIN )
    add_definitions( -DNOMINMAX )
else ()
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wnon-virtual-dtor -Wextra")

    if ( PLATFORM_OSX )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++" )
    endif ()
endif ()

# Print coloured error messages when compiling with Clang and Ninja.
if ( COMPILER_CLANG AND CMAKE_GENERATOR STREQUAL "Ninja" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics" )
endif ()
