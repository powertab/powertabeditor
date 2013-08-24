cmake_minimum_required(VERSION 2.8.8)

add_library(pteformats
    fileformat.cpp
    fileformatmanager.cpp
    #scorearranger.cpp

    gpx/bitstream.cpp
    gpx/documentreader.cpp
    gpx/filesystem.cpp
    gpx/gpximporter.cpp
    gpx/util.cpp
    gpx/pugixml/pugixml.cpp

    #guitar_pro/gp_channel.cpp
    #guitar_pro/guitarproimporter.cpp
    #guitar_pro/inputstream.cpp

    powertab/powertabexporter.cpp
    powertab/powertabimporter.cpp

    powertab_old/powertaboldimporter.cpp
    powertab_old/powertabdocument/alternateending.cpp
    powertab_old/powertabdocument/barline.cpp
    powertab_old/powertabdocument/chorddiagram.cpp
    powertab_old/powertabdocument/chordname.cpp
    powertab_old/powertabdocument/chordtext.cpp
    powertab_old/powertabdocument/colour.cpp
    powertab_old/powertabdocument/direction.cpp
    powertab_old/powertabdocument/dynamic.cpp
    powertab_old/powertabdocument/floatingtext.cpp
    powertab_old/powertabdocument/fontsetting.cpp
    powertab_old/powertabdocument/generalmidi.cpp
    powertab_old/powertabdocument/guitar.cpp
    powertab_old/powertabdocument/guitarin.cpp
    powertab_old/powertabdocument/harmonics.cpp
    powertab_old/powertabdocument/keysignature.cpp
    powertab_old/powertabdocument/macros.cpp
    powertab_old/powertabdocument/note.cpp
    powertab_old/powertabdocument/oldrehearsalsign.cpp
    powertab_old/powertabdocument/oldtimesignature.cpp
    powertab_old/powertabdocument/position.cpp
    powertab_old/powertabdocument/powertabdocument.cpp
    powertab_old/powertabdocument/powertabfileheader.cpp
    powertab_old/powertabdocument/powertabinputstream.cpp
    powertab_old/powertabdocument/powertaboutputstream.cpp
    powertab_old/powertabdocument/rect.cpp
    powertab_old/powertabdocument/rehearsalsign.cpp
    powertab_old/powertabdocument/rhythmslash.cpp
    powertab_old/powertabdocument/score.cpp
    powertab_old/powertabdocument/staff.cpp
    powertab_old/powertabdocument/system.cpp
    powertab_old/powertabdocument/systemsymbol.cpp
    powertab_old/powertabdocument/tempomarker.cpp
    powertab_old/powertabdocument/timesignature.cpp
    powertab_old/powertabdocument/tuning.cpp
)

qt5_use_modules(pteformats Widgets) 
