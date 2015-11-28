project( xdg )

install(
    FILES powertabeditor.desktop
    DESTINATION ${PTE_XDG_APPS_INSTALL_DIR}
)

install(
    FILES ../source/icons/app_icon.png
    RENAME powertabeditor.png
    DESTINATION ${PTE_XDG_ICONS_INSTALL_DIR}/128x128/apps
)
