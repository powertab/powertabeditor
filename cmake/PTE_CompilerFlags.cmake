option( ENABLE_ASAN "Enable address sanitizer." OFF )
option( ENABLE_TSAN "Enable thread sanitizer." OFF )

function ( pte_add_compile_flags target )
    # Always warn about using deprecated Qt functions.
    target_compile_definitions( ${target} PRIVATE -DQT_DEPRECATED_WARNINGS )

    # Use C++11.
    set_target_properties( ${target} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
    )

    if ( PLATFORM_WIN )
        target_compile_definitions( ${target} PRIVATE -DNOMINMAX -D_SCL_SECURE_NO_WARNINGS )
    else ()
        target_compile_options( ${target} PRIVATE -Wall -Wnon-virtual-dtor -Wextra -Werror=switch )

        if ( PLATFORM_OSX )
            target_compile_options( ${target} PRIVATE -stdlib=libc++ )
        endif ()
    endif ()

    # Print coloured error messages when compiling with Clang and Ninja.
    if ( CMAKE_GENERATOR STREQUAL "Ninja" )
        if ( COMPILER_CLANG )
            target_compile_options( ${target} PRIVATE -fcolor-diagnostics )
        elseif ( COMPILER_GCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.8 )
            target_compile_options( ${target} PRIVATE -fdiagnostics-color=always )
        endif ()
    endif ()

    if ( ENABLE_ASAN )
        target_compile_options( ${target} PRIVATE
            -fsanitize=address
            -fno-omit-frame-pointer
        )
        target_link_options( ${target} PRIVATE
            -fsanitize=address
        )
    elseif ( ENABLE_TSAN )
        target_compile_options( ${target} PRIVATE
            -fsanitize=thread
        )
        target_link_options( ${target} PRIVATE
            -fsanitize=thread
        )
    endif ()
endfunction ()
