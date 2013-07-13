cmake_minimum_required(VERSION 2.8.8)

include_directories(.)

# Get the current SVN revision number
include(FindSubversion)
Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} MY)
add_definitions(-DSVN_REVISION=${MY_WC_REVISION})

add_library(pteapp
    #clipboard.cpp
    command.cpp
    #documentmanager.cpp
    options.cpp
    #powertabeditor.cpp
    recentfiles.cpp
    #scorearea.cpp
    settings.cpp
    #skinmanager.cpp
    #tuningdictionary.cpp
)

qt5_use_modules(pteapp Widgets Concurrent)
