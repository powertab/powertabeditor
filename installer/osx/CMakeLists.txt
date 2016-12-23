set( macdeployqt_path "/usr/local/opt/qt5/bin/macdeployqt" )
set( installer_file powertabeditor-${PTE_VERSION}-osx.app.zip )
set( bundle_dir ${CMAKE_CURRENT_BINARY_DIR}/powertabeditor.app )

set( src_files )
set( bundle_files )
macro( install_to_bundle src_path bundle_subdir )
    set( dest_folder ${bundle_dir}/${bundle_subdir} )
    get_filename_component( filename ${src_path} NAME )
    set( dest_path ${dest_folder}/${filename} )

    add_custom_command(
        OUTPUT ${dest_path}
        DEPENDS ${src_path}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${dest_folder}
        COMMAND ${CMAKE_COMMAND} -E copy ${src_path} ${dest_path}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

    list( APPEND src_files ${src_path} )
    list( APPEND bundle_files ${dest_path} )
endmacro ()

# Build directory structure for bundle.
install_to_bundle( ${PTE_DEV_BIN_DIR}/powertabeditor Contents/MacOS )
install_to_bundle( ${PTE_DATA_DIR}/tunings.json Contents/MacOS/data )
install_to_bundle( Info.plist Contents )
install_to_bundle( powertabeditor.icns Contents/Resources )

# Copy library dependencies.
set( macdeployqt_stampfile "macdeployqt_stamp" )
add_custom_command(
    OUTPUT ${macdeployqt_stampfile}
    DEPENDS powertabeditor
    COMMAND ${macdeployqt_path} ${bundle_dir}
    COMMAND ${CMAKE_COMMAND} -E touch ${macdeployqt_stampfile}
    COMMENT "Running macdeployqt"
)
list( APPEND bundle_files ${macdeployqt_stampfile} )

# Zip up the bundle.
add_custom_command(
    OUTPUT ${installer_file}
    DEPENDS ${bundle_files}
    COMMAND ${CMAKE_COMMAND} -E tar "cfv" ${installer_file} --format=zip ${bundle_dir}
    COMMENT "Compressing installer"
)

add_custom_target( osx_installer
    DEPENDS ${installer_file}
)
