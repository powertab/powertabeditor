# TODO - improve withershin's build system and consider using CMake's external
# project module.

set( BUILD_TESTS OFF CACHE BOOL "Build withershin's unit tests" )
add_subdirectory( ${PTE_EXTERNAL_DIR}/withershins )

set_target_properties(
    withershins PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${PTE_EXTERNAL_DIR}/withershins/src
)

# Set folder name for Visual Studio projects.
set_target_properties(
    withershins PROPERTIES
    FOLDER ${PTE_EXTERNAL_FOLDER_NAME}
)
