cmake_minimum_required( VERSION 3.1 )
project( powertabeditor )

set( CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/cmake
    ${CMAKE_SOURCE_DIR}/cmake/third_party/modules
)

include( PTE_Platform )
include( PTE_CompilerFlags )
include( PTE_Paths )
include( PTE_ThirdParty )
include( PTE_CopyFiles )
include( PTE_Executable )
include( PTE_Library )

add_subdirectory( source )
add_subdirectory( test )
add_subdirectory( installer )
if ( PLATFORM_LINUX )
    add_subdirectory(xdg)
endif ()
