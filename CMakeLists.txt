cmake_minimum_required( VERSION 3.12 )



# Target OS X 14.0 and above. This must be set before the first project() call.
if(APPLE)
    # Detect macOS version
    execute_process(
    COMMAND sw_vers -productVersion
    OUTPUT_VARIABLE MACOS_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    string(REGEX MATCH "^[0-9]+" MACOS_MAJOR "${MACOS_VERSION}")
    message(STATUS "macOS major version: ${MACOS_MAJOR}")

    if(MACOS_MAJOR VERSION_LESS 15.0)
        set(CMAKE_OSX_DEPLOYMENT_TARGET 14.0)
    else()
        set(CMAKE_OSX_DEPLOYMENT_TARGET 15.0)
    endif()
endif()

message(CMAKE_OSX_DEPLOYMENT_TARGET="${CMAKE_OSX_DEPLOYMENT_TARGET}")




project( powertabeditor )
include( CTest )

# Allow using BOOST_ROOT.
if ( POLICY CMP0074 )
	cmake_policy( SET CMP0074 NEW )
endif ()

set( CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/cmake
    ${CMAKE_SOURCE_DIR}/cmake/third_party/modules
)

set( PTE_VERSION 2.0.22 )

include( PTE_Platform )
include( PTE_CompilerFlags )
include( PTE_Paths )
include( PTE_ThirdParty )
include( PTE_CopyFiles )
include( PTE_PrecompiledHeader )
include( PTE_Executable )
include( PTE_Library )

add_subdirectory( source )
add_subdirectory( installer )
add_subdirectory( translations )
if ( PLATFORM_LINUX )
    add_subdirectory(xdg)
endif ()

if ( BUILD_TESTING )
    add_subdirectory( test )
endif ()
