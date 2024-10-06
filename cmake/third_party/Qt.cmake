find_package( Qt6 REQUIRED COMPONENTS
	Widgets
	Network
	PrintSupport
	LinguistTools
)

set( QT_PLUGINS )
if ( PLATFORM_WIN )
	set( QT_PLUGINS 
        Qt6::QWindowsIntegrationPlugin
        Qt6::QModernWindowsStylePlugin
    )
endif ()

# Find some standard paths to the Qt installation.
get_target_property( _uic_path Qt::lconvert IMPORTED_LOCATION )
get_filename_component( PTE_QT_BIN_DIR ${_uic_path} DIRECTORY )
get_filename_component( PTE_QT_ROOT_DIR ${PTE_QT_BIN_DIR} DIRECTORY )

# Find the Qt translations, which we need to copy for Windows / Mac installers.
set( PTE_QT_TRANSLATIONS_DIR "${PTE_QT_ROOT_DIR}/translations" )
file( GLOB PTE_QT_TRANSLATION_FILES
    ${PTE_QT_TRANSLATIONS_DIR}/qtbase_*.qm
)
