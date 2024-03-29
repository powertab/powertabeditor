project( pte_tests )

set( srcs
    test_main.cpp

    actions/test_addalternateending.cpp
    actions/test_addartificialharmonic.cpp
    actions/test_addbarline.cpp
    actions/test_addbend.cpp
    actions/test_addchordtext.cpp
    actions/test_adddirection.cpp
    actions/test_adddynamic.cpp
    actions/test_addinstrument.cpp
    actions/test_addirregulargrouping.cpp
    actions/test_addlefthandfingering.cpp
    actions/test_addmultibarrest.cpp
    actions/test_addnote.cpp
    actions/test_addnoteproperty.cpp
    actions/test_addplayer.cpp
    actions/test_addpositionproperty.cpp
    actions/test_addrest.cpp
    actions/test_addstaff.cpp
    actions/test_addsystem.cpp
    actions/test_addtappedharmonic.cpp
    actions/test_addtrill.cpp
    actions/test_chorddiagram.cpp
    actions/test_adjustlinespacing.cpp
    actions/test_editbarline.cpp
    actions/test_editclef.cpp
    actions/test_editdynamic.cpp
    actions/test_editfileinformation.cpp
    actions/test_editinstrument.cpp
    actions/test_editkeysignature.cpp
    actions/test_editnoteduration.cpp
    actions/test_editplayer.cpp
    actions/test_editplayerchange.cpp
    actions/test_editrehearsalsign.cpp
    actions/test_editstaff.cpp
    actions/test_edittabnumber.cpp
    actions/test_edittempomarker.cpp
    actions/test_edittextitem.cpp
    actions/test_edittimesignature.cpp
    actions/test_editviewfilters.cpp
    actions/test_removealternateending.cpp
    actions/test_removeartificialharmonic.cpp
    actions/test_removebarline.cpp
    actions/test_removebend.cpp
    actions/test_removechordtext.cpp
    actions/test_removedirection.cpp
    actions/test_removedynamic.cpp
    actions/test_removeinstrument.cpp
    actions/test_removeirregulargrouping.cpp
    actions/test_removelefthandfingering.cpp
    actions/test_removenote.cpp
    actions/test_removenoteproperty.cpp
    actions/test_removeplayer.cpp
    actions/test_removeposition.cpp
    actions/test_removepositionproperty.cpp
    actions/test_removestaff.cpp
    actions/test_removesystem.cpp
    actions/test_removetappedharmonic.cpp
    actions/test_removetrill.cpp
    actions/test_shiftstring.cpp
    actions/test_tremolobar.cpp
    actions/test_volumeswell.cpp

    audio/test_midioutputdevice.cpp

    app/test_documentmanager.cpp
    app/test_settingsmanager.cpp

    dialogs/test_viewfilterdialog.cpp

    formats/test_fileformat.cpp
    formats/gp7/test_gp7.cpp
    formats/gpx/test_gpx.cpp
    formats/guitar_pro/test_gp.cpp
    formats/powertab_old/test_powertabold.cpp

    score/test_alternateending.cpp
    score/test_barline.cpp
    score/test_chorddiagram.cpp
    score/test_chordname.cpp
    score/test_chordtext.cpp
    score/test_direction.cpp
    score/test_dynamic.cpp
    score/test_instrument.cpp
    score/test_irregulargrouping.cpp
    score/test_keysignature.cpp
    score/test_note.cpp
    score/test_player.cpp
    score/test_playerchange.cpp
    score/test_position.cpp
    score/test_rehearsalsign.cpp
    score/test_score.cpp
    score/test_scoreinfo.cpp
    score/test_staff.cpp
    score/test_system.cpp
    score/test_tempomarker.cpp
    score/test_textitem.cpp
    score/test_timesignature.cpp
    score/test_tuning.cpp
    score/test_utils.cpp
    score/test_viewfilter.cpp
    score/test_voiceutils.cpp

    util/test_enumtostring.cpp
    util/test_scopeexit.cpp
    util/test_settingstree.cpp
)

set( headers
    actions/actionfixture.h
    score/test_serialization.h
)

set( data_files
    actions/data/test_editstaff.pt2
    actions/data/test_shiftstring.pt2

    formats/powertab_old/data/alternate_endings.ptb
    formats/powertab_old/data/barlines.ptb
    formats/powertab_old/data/bends.ptb
    formats/powertab_old/data/chord_diagrams.ptb
    formats/powertab_old/data/chordtext.ptb
    formats/powertab_old/data/directions.ptb
    formats/powertab_old/data/floating_text.ptb
    formats/powertab_old/data/guitar_ins.ptb
    formats/powertab_old/data/guitars.ptb
    formats/powertab_old/data/merge_multibar_rests_correct.pt2
    formats/powertab_old/data/merge_multibar_rests.ptb
    formats/powertab_old/data/notes.ptb
    formats/powertab_old/data/positions.ptb
    formats/powertab_old/data/song_header.ptb
    formats/powertab_old/data/staves.ptb
    formats/powertab_old/data/tempo_markers.ptb
    formats/powertab_old/data/tremolo_bars.ptb
    formats/powertab_old/data/volume_swells.ptb

    formats/guitar_pro/data/alt_endings.gp5
    formats/guitar_pro/data/barlines.gp5
    formats/guitar_pro/data/bends.gp5
    formats/guitar_pro/data/directions.gp5
    formats/guitar_pro/data/gracenote.gp5
    formats/guitar_pro/data/harmonics.gp5
    formats/guitar_pro/data/irregular.gp5
    formats/guitar_pro/data/keys.gp5
    formats/guitar_pro/data/notes.gp5
    formats/guitar_pro/data/positions.gp5
    formats/guitar_pro/data/rehearsal_signs.gp5
    formats/guitar_pro/data/tempos.gp5
    formats/guitar_pro/data/text.gp5
    formats/guitar_pro/data/time_signatures.gp5
    formats/guitar_pro/data/tremolo_bars.gp5

    formats/gp7/data/alternate_endings.gp
    formats/gp7/data/bars.gp
    formats/gp7/data/bends.gp
    formats/gp7/data/chord_diagrams.gp
    formats/gp7/data/directions.gp
    formats/gp7/data/fermatas.gp
    formats/gp7/data/harmonics.gp
    formats/gp7/data/irregular_groups.gp
    formats/gp7/data/notes.gp
    formats/gp7/data/score_info.gp
    formats/gp7/data/text.gp
    formats/gp7/data/tracks.gp
    formats/gp7/data/tremolo_bars.gp
    formats/gp7/data/words_and_music.gp

    formats/gpx/data/chord_diagrams.gpx
    formats/gpx/data/chord_names.gpx
    formats/gpx/data/text.gpx
    formats/gpx/data/tremolo_bars.gpx

    score/data/reordered.pt2
    score/data/test_viewfilter.pt2

    util/test_settingstree_expected.json
)

pte_executable(
    CONSOLE
    NAME pte_tests
    SOURCES ${srcs}
    HEADERS ${headers}
    PCH precompiled.h
    PCH_EXCLUDE test_main.cpp
    DEPENDS
        doctest::doctest
        pteapp
        rtmidi::rtmidi
)

# Workaround for https://github.com/onqtam/doctest/issues/316
if ( PLATFORM_OSX )
    target_compile_definitions( pte_tests PRIVATE
        DOCTEST_CONFIG_USE_STD_HEADERS
    )
endif ()

add_test(
    NAME all_tests
    COMMAND pte_tests exclude:Formats/PowerTabOldImport/Directions
)

pte_copyfiles(
    NAME pte_tests_data
    DESTINATION ${PTE_DATA_DIR}
    FILES ${data_files}
)
add_dependencies( pte_tests pte_tests_data )

add_custom_target( check
    ${CMAKE_COMMAND} -E env CTEST_OUTPUT_ON_FAILURE=1
    ${CMAKE_CTEST_COMMAND} --verbose
)
