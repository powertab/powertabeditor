cmake_minimum_required(VERSION 2.8.8)

set(FORMS
    instruments/instrumentpanelitem.ui
    mixer/mixeritem.ui
    playback/playbackwidget.ui
)

qt5_wrap_ui(FORMS_HEADERS ${FORMS})

add_library(ptewidgets
    ${FORMS_HEADERS}

    clickablelabel.h
    clickablelabel.cpp
    #common.cpp

    instruments/instrumentpanel.h
    instruments/instrumentpanel.cpp
    instruments/instrumentpanelitem.h
    instruments/instrumentpanelitem.cpp

    mixer/mixer.h
    mixer/mixer.cpp
    mixer/mixeritem.h
    mixer/mixeritem.cpp

    playback/playbackwidget.h
    playback/playbackwidget.cpp

    #toolbox/notepage.cpp
    #toolbox/scorepage.cpp
    #toolbox/toolbox.cpp
)

qt5_use_modules(ptewidgets Widgets) 
