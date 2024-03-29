set( PTE_DEV_BIN_DIR ${CMAKE_BINARY_DIR}/bin )
set( PTE_DATA_DIR ${PTE_DEV_BIN_DIR}/data )

if ( PLATFORM_LINUX )
    set( PTE_DATA_INSTALL_DIR share/powertab/powertabeditor )
else ()
    set( PTE_DATA_INSTALL_DIR data )
endif ()

set( PTE_TRANSLATIONS_DIR ${PTE_DATA_DIR}/translations )
set( PTE_TRANSLATIONS_INSTALL_DIR ${PTE_DATA_INSTALL_DIR}/translations )

set( PTE_XDG_APPS_INSTALL_DIR share/applications )
set( PTE_XDG_ICONS_INSTALL_DIR share/icons/hicolor )
set( PTE_XDG_MIME_INSTALL_DIR share/mime/packages )
set( PTE_XDG_METAINFO_INSTALL_DIR share/metainfo )
