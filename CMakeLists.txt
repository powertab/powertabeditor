cmake_minimum_required( VERSION 3.12 )

# Target OS X 10.15 and above. This must be set before the first project() call.
set( CMAKE_OSX_DEPLOYMENT_TARGET "10.15"
     CACHE STRING "Minimum OS X deployment version"
)

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
