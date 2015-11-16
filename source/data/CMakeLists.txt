project( ptedata )

set( files
    tunings.json
)

pte_copyfiles(
    NAME ptedata
    INSTALL
    DESTINATION ${PTE_DATA_DIR}
    FILES ${files}
)
