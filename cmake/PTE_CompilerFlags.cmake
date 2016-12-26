function ( pte_add_compile_flags target )
    # Always warn about using deprecated Qt functions.
    target_compile_definitions( ${target} PRIVATE -DQT_DEPRECATED_WARNINGS )

    # Use C++11.
    set_target_properties( ${target} PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
    )

    if ( PLATFORM_WIN )
        target_compile_definitions( ${target} PRIVATE -DNOMINMAX )
    else ()
        target_compile_options( ${target} PRIVATE -Wall -Wnon-virtual-dtor -Wextra )

        if ( PLATFORM_OSX )
            target_compile_options( ${target} PRIVATE -stdlib=libc++ )
        endif ()
    endif ()

    # Print coloured error messages when compiling with Clang and Ninja.
    if ( COMPILER_CLANG AND CMAKE_GENERATOR STREQUAL "Ninja" )
        target_compile_options( ${target} PRIVATE -fcolor-diagnostics )
    endif ()
endfunction ()
