cmake_minimum_required(VERSION 2.8.8)

add_executable(pte_tests
    test_main.cpp

    #actions/test_addartificialharmonic.cpp
    actions/test_addbarline.cpp
    actions/test_adddynamic.cpp
    #actions/test_addirregulargrouping.cpp
    #actions/test_addmusicaldirection.cpp
    actions/test_addnoteproperty.cpp
    actions/test_addplayerchange.cpp
    actions/test_addpositionproperty.cpp
    actions/test_addrehearsalsign.cpp
    actions/test_addsystem.cpp
    actions/test_addtappedharmonic.cpp
    actions/test_addtrill.cpp
    #actions/test_addvolumeswell.cpp
    actions/test_adjustlinespacing.cpp
    #actions/test_deletenote.cpp
    actions/test_editbarline.cpp
    #actions/test_edithammerpull.cpp
    actions/test_editkeysignature.cpp
    #actions/test_editrest.cpp
    #actions/test_editslideinto.cpp
    #actions/test_editslideout.cpp
    actions/test_edittabnumber.cpp
    #actions/test_edittimesignature.cpp
    #actions/test_removeartificialharmonic.cpp
    actions/test_removedynamic.cpp
    #actions/test_removeirregulargrouping.cpp
    #actions/test_removemusicaldirection.cpp
    actions/test_removenoteproperty.cpp
    actions/test_removeplayerchange.cpp
    actions/test_removepositionproperty.cpp
    actions/test_removerehearsalsign.cpp
    actions/test_removesystem.cpp
    actions/test_removetappedharmonic.cpp
    actions/test_removetrill.cpp
    #actions/test_removevolumeswell.cpp
    #actions/test_shifttabnumber.cpp
    #actions/test_updatenoteduration.cpp

    app/test_documentmanager.cpp

    #audio/test_midievent.cpp

    formats/test_fileformat.cpp
    #formats/guitar_pro/test_gp4.cpp
    formats/powertab_old/test_powertabold.cpp

    #powertabdocument/alternateending_test.cpp
    #powertabdocument/chorddiagram_test.cpp
    #powertabdocument/common_test.cpp
    #powertabdocument/complexsymbolarray_test.cpp
    #powertabdocument/direction_test.cpp
    #powertabdocument/generalmidi_test.cpp
    #powertabdocument/harmonics_test.cpp
    #powertabdocument/keysignature_test.cpp
    #powertabdocument/note_test.cpp
    #powertabdocument/position_test.cpp
    #powertabdocument/powertabfileheader_test.cpp
    #powertabdocument/score_test.cpp
    #powertabdocument/staff_test.cpp
    #powertabdocument/system_test.cpp
    #powertabdocument/timesignature_test.cpp
    #powertabdocument/tuning_test.cpp

    score/test_alternateending.cpp
    score/test_barline.cpp
    score/test_direction.cpp
    score/test_dynamic.cpp
    score/test_instrument.cpp
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
    score/test_timesignature.cpp
    score/test_tuning.cpp
    score/test_utils.cpp
)

qt5_use_modules(pte_tests Widgets)

target_link_libraries(pte_tests
    pteapp
    #ptewidgets
    #ptedialogs
    pteactions
    #pteaudio
    #rtmidi
    #ptepainters
    #sigfwd
    pteformats
    ptescore
    ${Boost_LIBRARIES}
)

# Copy the test files to the build directory.
#file(COPY formats/guitar_pro/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_custom_command(TARGET pte_tests POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/formats/powertab_old/data
    ${CMAKE_CURRENT_BINARY_DIR}/data)
