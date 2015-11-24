project( ptedata )

set( files
    tunings.json
)

pte_copyfiles(
    NAME ptedata
    DESTINATION ${PTE_DATA_DIR}
    INSTALL ${PTE_DATA_INSTALL_DIR}
    FILES ${files}
)
