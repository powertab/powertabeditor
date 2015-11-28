find_package( withershins )

if ( WITHERSHINS_FOUND )
    add_library( withershins IMPORTED SHARED )

    set_target_properties(
        withershins PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${withershins_INCLUDE_DIRS}
        IMPORTED_LOCATION ${withershins_LIBRARIES}
    )
else ()
    # TODO - improve withershin's build system and consider using CMake's external
    # project module.

    set( BUILD_TESTS OFF CACHE BOOL "Build withershin's unit tests" )
    add_subdirectory( ${PTE_EXTERNAL_DIR}/withershins )

    message( STATUS "Using withershins library from ${PTE_EXTERNAL_DIR}/withershins" )

    set_target_properties(
        withershins PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${PTE_EXTERNAL_DIR}/withershins/src
    )

    # Set folder name for Visual Studio projects.
    set_target_properties(
        withershins PROPERTIES
        FOLDER ${PTE_EXTERNAL_FOLDER_NAME}
    )
endif ()
