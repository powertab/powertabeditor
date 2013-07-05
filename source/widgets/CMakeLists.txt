cmake_minimum_required(VERSION 2.8.8)

set(FORMS
    playback/playbackwidget.ui
)

qt5_wrap_ui(FORMS_HEADERS ${FORMS})

add_library(ptewidgets
    ${FORMS_HEADERS}
    clickablelabel.cpp
    common.cpp

    mixer/mixer.cpp
    mixer/mixerinstrument.cpp

    playback/playbackwidget.cpp

    toolbox/notepage.cpp
    toolbox/scorepage.cpp
    toolbox/toolbox.cpp
)

qt5_use_modules(ptewidgets Widgets) 
