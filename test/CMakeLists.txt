cmake_minimum_required(VERSION 2.8.8)

add_executable(pte_tests
    test_main.cpp

    actions/test_addalternateending.cpp
    actions/test_addartificialharmonic.cpp
    actions/test_addbarline.cpp
    actions/test_addchordtext.cpp
    actions/test_adddirection.cpp
    actions/test_adddynamic.cpp
    #actions/test_addirregulargrouping.cpp
    actions/test_addnote.cpp
    actions/test_addnoteproperty.cpp
    actions/test_addplayerchange.cpp
    actions/test_addpositionproperty.cpp
    actions/test_addrehearsalsign.cpp
    actions/test_addrest.cpp
    actions/test_addsystem.cpp
    actions/test_addtappedharmonic.cpp
    actions/test_addtempomarker.cpp
    actions/test_addtrill.cpp
    #actions/test_addvolumeswell.cpp
    actions/test_adjustlinespacing.cpp
    actions/test_editbarline.cpp
    actions/test_editclef.cpp
    actions/test_editfileinformation.cpp
    actions/test_editinstrument.cpp
    actions/test_editkeysignature.cpp
    actions/test_editnoteduration.cpp
    actions/test_editplayer.cpp
    actions/test_edittabnumber.cpp
    actions/test_edittimesignature.cpp
    actions/test_removealternateending.cpp
    actions/test_removeartificialharmonic.cpp
    actions/test_removebarline.cpp
    actions/test_removechordtext.cpp
    actions/test_removedirection.cpp
    actions/test_removedynamic.cpp
    #actions/test_removeirregulargrouping.cpp
    actions/test_removenote.cpp
    actions/test_removenoteproperty.cpp
    actions/test_removeplayerchange.cpp
    actions/test_removeposition.cpp
    actions/test_removepositionproperty.cpp
    actions/test_removerehearsalsign.cpp
    actions/test_removesystem.cpp
    actions/test_removetappedharmonic.cpp
    actions/test_removetempomarker.cpp
    actions/test_removetrill.cpp
    #actions/test_removevolumeswell.cpp
    #actions/test_shifttabnumber.cpp

    app/test_documentmanager.cpp

    audio/test_midievent.cpp

    formats/test_fileformat.cpp
    formats/guitar_pro/test_gp4.cpp
    formats/powertab_old/test_powertabold.cpp

    score/test_alternateending.cpp
    score/test_barline.cpp
    score/test_chordname.cpp
    score/test_chordtext.cpp
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

    # Header-only files.
    catch.hpp
    actions/actionfixture.h
    score/test_serialization.h
)

qt5_use_modules(pte_tests Widgets)

target_link_libraries(pte_tests
    pteapp
    pteactions
    ptewidgets
    pteaudio
    rtmidi
    pteformats
    ptescore
    ${Boost_LIBRARIES}
)

# Copy the test files to the build directory.
add_custom_command(TARGET pte_tests POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/formats/powertab_old/data
    ${CMAKE_CURRENT_BINARY_DIR}/data)

add_custom_command(TARGET pte_tests POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/formats/guitar_pro/data
    ${CMAKE_CURRENT_BINARY_DIR}/data)
