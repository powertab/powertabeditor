cmake_minimum_required( VERSION 3.1 )
project( powertabeditor )

set( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake )

include( PTE_Platform )
include( PTE_CompilerFlags )
include( PTE_Paths )
include( PTE_ThirdParty )
include( PTE_Executable )
include( PTE_Library )

add_subdirectory( source )
# TODO
#add_subdirectory( test )
