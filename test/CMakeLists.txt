cmake_minimum_required(VERSION 2.8.8)

add_executable(pte_tests
    test_main.cpp

    actions/test_addartificialharmonic.cpp
    actions/test_addirregulargrouping.cpp
    actions/test_addmusicaldirection.cpp
    actions/test_addtappedharmonic.cpp
    actions/test_addvolumeswell.cpp
    actions/test_deletenote.cpp
    actions/test_edithammerpull.cpp
    actions/test_editkeysignature.cpp
    actions/test_editrest.cpp
    actions/test_editslideinto.cpp
    actions/test_editslideout.cpp
    actions/test_edittimesignature.cpp
    actions/test_removeartificialharmonic.cpp
    actions/test_removeirregulargrouping.cpp
    actions/test_removemusicaldirection.cpp
    actions/test_removesystem.cpp
    actions/test_removetappedharmonic.cpp
    actions/test_removevolumeswell.cpp
    actions/test_shifttabnumber.cpp
    actions/test_updatenoteduration.cpp
    actions/test_updatetabnumber.cpp

    app/test_documentmanager.cpp

    audio/test_midievent.cpp

    formats/test_fileformat.cpp
    formats/guitar_pro/test_gp4.cpp

    powertabdocument/alternateending_test.cpp
    powertabdocument/chorddiagram_test.cpp
    powertabdocument/common_test.cpp
    powertabdocument/complexsymbolarray_test.cpp
    powertabdocument/direction_test.cpp
    powertabdocument/generalmidi_test.cpp
    powertabdocument/harmonics_test.cpp
    powertabdocument/keysignature_test.cpp
    powertabdocument/note_test.cpp
    powertabdocument/position_test.cpp
    powertabdocument/powertabfileheader_test.cpp
    powertabdocument/score_test.cpp
    powertabdocument/staff_test.cpp
    powertabdocument/system_test.cpp
    powertabdocument/timesignature_test.cpp
    powertabdocument/tuning_test.cpp

    score/test_tuning.cpp
)

qt5_use_modules(pte_tests Widgets)

target_link_libraries(pte_tests
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
    ptescore
)

# Copy the test files to the build directory.
file(COPY formats/guitar_pro/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
