cmake_minimum_required(VERSION 2.8.8)

add_library(pteformats
    fileformat.cpp
    fileformatmanager.cpp
    #scorearranger.cpp

    #gpx/bitstream.cpp
    #gpx/documentreader.cpp
    #gpx/filesystem.cpp
    #gpx/gpximporter.cpp
    #gpx/util.cpp
    #gpx/pugixml/pugixml.cpp

    #guitar_pro/gp_channel.cpp
    #guitar_pro/guitarproimporter.cpp
    #guitar_pro/inputstream.cpp

    powertab/powertabexporter.cpp
    powertab/powertabimporter.cpp
)

qt5_use_modules(pteformats Widgets) 
