cmake_minimum_required(VERSION 2.8.9)

# Run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

add_subdirectory(actions)
add_subdirectory(app)
add_subdirectory(audio)
add_subdirectory(dialogs)
add_subdirectory(formats)
add_subdirectory(midi)
add_subdirectory(painters)
add_subdirectory(score)
add_subdirectory(widgets)

qt5_add_resources(RESOURCES build/resources.qrc)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(RC_FILE build/build.rc)
endif()

add_executable(powertabeditor WIN32
    build/main.cpp
    ${RC_FILE}
    ${RESOURCES}
)

qt5_use_modules(powertabeditor Widgets Network)

target_link_libraries(powertabeditor
    pteapp
    ptedialogs
    ptewidgets
    pteaudio
    ptemidi
    rtmidi
    ptepainters
    pteformats
    pteactions
    ptescore
    pugixml
    withershins
    ${Boost_LIBRARIES}
    ${ZLIB_LIBRARIES}
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Link against the ALSA library.
    target_link_libraries(powertabeditor ${ALSA_LIBRARY} pthread)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Link against winmm and QtMain
    target_link_libraries(powertabeditor winmm Qt5::WinMain)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(COREAUDIO CoreAudio)
    find_library(COREMIDI CoreMIDI)
    find_library(COREFOUNDATION CoreFoundation)
    find_library(AUDIOTOOLBOX AudioToolbox)
    find_library(AUDIOUNIT AudioUnit)
    target_link_libraries(powertabeditor ${COREAUDIO} ${COREMIDI} ${COREFOUNDATION} ${AUDIOTOOLBOX} ${AUDIOUNIT})
endif()

# Copy the tuning database to the build directory.
file(COPY data DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Copy the Qt and Boost dlls to the build folder on Windows.
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(QT_BIN_DIR ${Qt5Core_DIR}/../../../bin)
    set(QT_LIB_DIR ${Qt5Core_DIR}/../../../lib)
    set(QT_PLATFORM_PLUGINS_DIR ${Qt5Core_DIR}/../../../plugins/platforms)
    get_filename_component(BOOST_LIB_DIR ${Boost_DATE_TIME_LIBRARY_DEBUG} PATH)
    get_filename_component(DT_NAME_DEBUG ${Boost_DATE_TIME_LIBRARY_DEBUG} NAME_WE)
    get_filename_component(PO_NAME_DEBUG ${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG} NAME_WE)
    get_filename_component(IO_NAME_DEBUG ${Boost_IOSTREAMS_LIBRARY_DEBUG} NAME_WE)
    get_filename_component(ZLIB_NAME_DEBUG ${Boost_ZLIB_LIBRARY_DEBUG} NAME_WE)
    get_filename_component(REGEX_NAME_DEBUG ${Boost_REGEX_LIBRARY_DEBUG} NAME_WE)
    get_filename_component(DT_NAME_RELEASE ${Boost_DATE_TIME_LIBRARY_RELEASE} NAME_WE)
    get_filename_component(PO_NAME_RELEASE ${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE} NAME_WE)
    get_filename_component(IO_NAME_RELEASE ${Boost_IOSTREAMS_LIBRARY_RELEASE} NAME_WE)
    get_filename_component(ZLIB_NAME_RELEASE ${Boost_ZLIB_LIBRARY_RELEASE} NAME_WE)
    get_filename_component(REGEX_NAME_RELEASE ${Boost_REGEX_LIBRARY_RELEASE} NAME_WE)

    file(GLOB files ${QT_LIB_DIR}/Qt*.pdb)
    foreach(file ${files})
        file(COPY ${file} DESTINATION ${QT_BIN_DIR})
    endforeach()
    
    add_custom_command(TARGET powertabeditor POST_BUILD
	    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/platforms

	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5Widgetsd.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5Guid.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5Cored.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5Networkd.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5PrintSupportd.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5Widgetsd.pdb ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5Guid.pdb ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5Cored.pdb ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5Networkd.pdb ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5PrintSupportd.pdb ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_PLATFORM_PLUGINS_DIR}/qwindowsd.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/platforms

	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5Widgets.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5Gui.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5Core.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5Network.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5PrintSupport.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_PLATFORM_PLUGINS_DIR}/qwindows.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/platforms

	    COMMAND ${CMAKE_COMMAND} -E copy ${BOOST_LIB_DIR}/${DT_NAME_DEBUG}.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${BOOST_LIB_DIR}/${PO_NAME_DEBUG}.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${BOOST_LIB_DIR}/${IO_NAME_DEBUG}.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${BOOST_LIB_DIR}/${ZLIB_NAME_DEBUG}.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${BOOST_LIB_DIR}/${REGEX_NAME_DEBUG}.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}

	    COMMAND ${CMAKE_COMMAND} -E copy ${BOOST_LIB_DIR}/${DT_NAME_RELEASE}.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${BOOST_LIB_DIR}/${PO_NAME_RELEASE}.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${BOOST_LIB_DIR}/${IO_NAME_RELEASE}.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${BOOST_LIB_DIR}/${ZLIB_NAME_RELEASE}.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND ${CMAKE_COMMAND} -E copy ${BOOST_LIB_DIR}/${REGEX_NAME_RELEASE}.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )

    if (EXISTS ${QT_BIN_DIR}/libEGL.dll)
        add_custom_command(TARGET powertabeditor POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/libEGLd.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/libEGL.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
    endif ()

    if (EXISTS ${QT_BIN_DIR}/libGLESv2.dll)
        add_custom_command(TARGET powertabeditor POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/libGLESv2d.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/libGLESv2.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
    endif ()
    
    file(GLOB files ${QT_BIN_DIR}/icu*.dll)
    foreach(file ${files})
        add_custom_command(TARGET powertabeditor POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${file} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
    endforeach()
endif()
