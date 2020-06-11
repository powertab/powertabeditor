project ( pteformats )

set( srcs
    fileformat.cpp
    fileformatmanager.cpp

    gp7/converter.cpp
    gp7/gp7importer.cpp
    gp7/parser.cpp

    gpx/bitstream.cpp
    gpx/filesystem.cpp
    gpx/gpximporter.cpp
    gpx/util.cpp

    guitar_pro/document.cpp
    guitar_pro/guitarproimporter.cpp
    guitar_pro/inputstream.cpp

    midi/midiexporter.cpp

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
    powertab_old/powertabdocument/guitar.cpp
    powertab_old/powertabdocument/guitarin.cpp
    powertab_old/powertabdocument/harmonics.cpp
    powertab_old/powertabdocument/keysignature.cpp
    powertab_old/powertabdocument/macros.cpp
    powertab_old/powertabdocument/note.cpp
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

set( headers
    fileformat.h
    fileformatmanager.h

    gp7/converter.h
    gp7/gp7importer.h
    gp7/parser.h

    gpx/bitstream.h
    gpx/filesystem.h
    gpx/gpximporter.h
    gpx/util.h

    guitar_pro/document.h
    guitar_pro/guitarproimporter.h
    guitar_pro/inputstream.h

    midi/midiexporter.h

    powertab/common.h
    powertab/powertabexporter.h
    powertab/powertabimporter.h

    powertab_old/powertaboldimporter.h
    powertab_old/powertabdocument/alternateending.h
    powertab_old/powertabdocument/barline.h
    powertab_old/powertabdocument/chorddiagram.h
    powertab_old/powertabdocument/chordname.h
    powertab_old/powertabdocument/chordtext.h
    powertab_old/powertabdocument/colour.h
    powertab_old/powertabdocument/direction.h
    powertab_old/powertabdocument/dynamic.h
    powertab_old/powertabdocument/floatingtext.h
    powertab_old/powertabdocument/fontsetting.h
    powertab_old/powertabdocument/guitar.h
    powertab_old/powertabdocument/guitarin.h
    powertab_old/powertabdocument/harmonics.h
    powertab_old/powertabdocument/keysignature.h
    powertab_old/powertabdocument/macros.h
    powertab_old/powertabdocument/note.h
    powertab_old/powertabdocument/position.h
    powertab_old/powertabdocument/powertabdocument.h
    powertab_old/powertabdocument/powertabfileheader.h
    powertab_old/powertabdocument/powertabinputstream.h
    powertab_old/powertabdocument/powertaboutputstream.h
    powertab_old/powertabdocument/rect.h
    powertab_old/powertabdocument/rehearsalsign.h
    powertab_old/powertabdocument/rhythmslash.h
    powertab_old/powertabdocument/score.h
    powertab_old/powertabdocument/staff.h
    powertab_old/powertabdocument/system.h
    powertab_old/powertabdocument/systemsymbol.h
    powertab_old/powertabdocument/tempomarker.h
    powertab_old/powertabdocument/timesignature.h
    powertab_old/powertabdocument/tuning.h
)

pte_library(
    NAME pteformats
    SOURCES ${srcs}
    HEADERS ${headers}
    DEPENDS
        PUBLIC
            ptescore
            pteutil
        PRIVATE
            Boost::date_time
            Boost::iostreams
            minizip::minizip
            ptemidi
            pugixml::pugixml
)
