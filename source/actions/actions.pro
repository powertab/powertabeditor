include (../../common.pri)

INCLUDEPATH += $${PWD} # needed for precompiled headers in MSVC

CONFIG += precompile_header

PRECOMPILED_HEADER = actions_pch.h

HEADERS += \
    updatetabnumber.h \
    updatenoteduration.h \
    undomanager.h \
    toggleproperty.h \
    shifttabnumber.h \
    removetrill.h \
    removesystem.h \
    removechordtext.h \
    removealternateending.h \
    positionshift.h \
    edittuning.h \
    editslideout.h \
    editslideinto.h \
    editrehearsalsign.h \
    deleteposition.h \
    deletebarline.h \
    changepositionspacing.h \
    changebarlinetype.h \
    addtrill.h \
    addsystem.h \
    addnote.h \
    addguitar.h \
    addchordtext.h \
    addbarline.h \
    addalternateending.h \
    removetappedharmonic.h \
    addtappedharmonic.h \
    editrest.h \
    editkeysignature.h \
    edittimesignature.h \
    adddynamic.h \
    removedynamic.h \
    actions_pch.h \
    insertnotes.h \
    removevolumeswell.h \
    addvolumeswell.h \
    addirregulargrouping.h \
    removeirregulargrouping.h \
    edittrackshown.h \
    deletenote.h \
    editfileinformation.h \
    removetempomarker.h

SOURCES += \
    updatetabnumber.cpp \
    updatenoteduration.cpp \
    undomanager.cpp \
    shifttabnumber.cpp \
    removetrill.cpp \
    removesystem.cpp \
    removechordtext.cpp \
    removealternateending.cpp \
    positionshift.cpp \
    edittuning.cpp \
    editslideout.cpp \
    editslideinto.cpp \
    editrehearsalsign.cpp \
    deleteposition.cpp \
    deletebarline.cpp \
    changepositionspacing.cpp \
    changebarlinetype.cpp \
    addtrill.cpp \
    addsystem.cpp \
    addnote.cpp \
    addguitar.cpp \
    addchordtext.cpp \
    addbarline.cpp \
    addalternateending.cpp \
    removetappedharmonic.cpp \
    addtappedharmonic.cpp \
    editrest.cpp \
    editkeysignature.cpp \
    edittimesignature.cpp \
    adddynamic.cpp \
    removedynamic.cpp \
    insertnotes.cpp \
    removevolumeswell.cpp \
    addvolumeswell.cpp \
    addirregulargrouping.cpp \
    removeirregulargrouping.cpp \
    edittrackshown.cpp \
    deletenote.cpp \
    editfileinformation.cpp \
    removetempomarker.cpp
