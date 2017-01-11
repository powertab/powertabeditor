cmake_minimum_required( VERSION 3.1 )

# Target OS X 10.9 and above. This must be set before the first project() call.
set( CMAKE_OSX_DEPLOYMENT_TARGET "10.9"
     CACHE STRING "Minimum OS X deployment version"
)

project( powertabeditor )

enable_testing()

set( CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/cmake
    ${CMAKE_SOURCE_DIR}/cmake/third_party/modules
)

set( PTE_VERSION 2.0.0-alpha10 )

include( PTE_Platform )
include( PTE_CompilerFlags )
include( PTE_Paths )
include( PTE_ThirdParty )
include( PTE_CopyFiles )
include( PTE_PrecompiledHeader )
include( PTE_Executable )
include( PTE_Library )

add_subdirectory( source )
add_subdirectory( test )
add_subdirectory( installer )
if ( PLATFORM_LINUX )
    add_subdirectory(xdg)
endif ()
