# NOTE: this must be synchronized with source/build/osx/Info.plist.in for the macOS app bundle.
set( languages
    de
    en
    en_CA
    es
    fr
    it
    ja
    nl
    ru
    tr
    zh_Hans
)

# Generate the list of translation files.
set( translation_files ${languages} )
list( TRANSFORM translation_files PREPEND "powertabeditor_" )
list( TRANSFORM translation_files APPEND ".ts" )

# For macOS, install the translations into the bundle.
if ( PLATFORM_OSX )
    set( output_dir "${PTE_DEV_BIN_DIR}/Power Tab Editor.app/Contents/Resources/translations")
else ()
    set( output_dir ${PTE_TRANSLATIONS_DIR} )
endif ()

# Set the output location for the compiled translations.
set_source_files_properties( ${translation_files} PROPERTIES
    OUTPUT_LOCATION ${output_dir}
)

# Add a target to generate the translations.
# Optionally, the translations can be updated with new source strings.
option( PTE_UPDATE_TRANSLATIONS "Scan source files for new translatable strings." OFF )
if ( PTE_UPDATE_TRANSLATIONS )
    qt_create_translation( qm_files ${CMAKE_SOURCE_DIR} ${translation_files} )
    add_custom_target( create_translations DEPENDS ${qm_files} )
else ()
    qt_add_translation( qm_files
        ${translation_files}
        OPTIONS -silent
    )
    add_custom_target( compile_translations ALL DEPENDS ${qm_files} )
    set_target_properties( compile_translations PROPERTIES
        FOLDER data
    )

    # For Windows, also copy the Qt translations into our local folder since
    # the dlls are also copied there.
    if ( PLATFORM_WIN )
        pte_copyfiles( NAME copy_qt_translations
            DESTINATION ${PTE_TRANSLATIONS_DIR}
            FILES ${PTE_QT_TRANSLATION_FILES}
        )
        add_dependencies( compile_translations copy_qt_translations )
    endif ()
endif ()

# Install the compiled translations. This doesn't seem to be required for
# macOS, where CPack picks up everything in the build folder's app bundle.
if ( NOT PLATFORM_OSX )
    install(
        FILES ${qm_files}
        DESTINATION ${PTE_TRANSLATIONS_INSTALL_DIR}
    )
endif ()
