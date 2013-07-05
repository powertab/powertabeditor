cmake_minimum_required(VERSION 2.8.8)

add_subdirectory(actions)
add_subdirectory(app)
add_subdirectory(audio)
add_subdirectory(dialogs)
add_subdirectory(formats)
add_subdirectory(painters)
add_subdirectory(powertabdocument)
add_subdirectory(sigfwd)
add_subdirectory(widgets)

qt5_add_resources(RESOURCES build/resources.qrc)

add_executable(powertabeditor
    build/main.cpp
    ${RESOURCES}
)

qt5_use_modules(powertabeditor Widgets)

target_link_libraries(powertabeditor
    pteapp
    ptewidgets
    ptedialogs
    pteactions
    pteaudio
    rtmidi
    ptepainters
    sigfwd
    pteformats
    ptedocument
)

# Link against the ALSA library.
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(powertabeditor ${ALSA_LIBRARY} pthread)
endif()

# Copy the tuning database and the skins to the build directory.
file(COPY data skins DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
