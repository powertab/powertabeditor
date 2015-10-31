project( ptewidgets )

set( srcs
    clickablelabel.cpp
    common.cpp
    instruments/instrumentpanel.cpp
    instruments/instrumentpanelitem.cpp
    mixer/mixer.cpp
    mixer/mixeritem.cpp
    playback/playbackwidget.cpp
)

set( headers
    clickablelabel.h
    instruments/instrumentpanel.h
    instruments/instrumentpanelitem.h
    mixer/mixer.h
    mixer/mixeritem.h
    playback/playbackwidget.h
)

set( moc_headers
    clickablelabel.h
    playback/playbackwidget.h
)

set( forms
    instruments/instrumentpanelitem.ui
    mixer/mixeritem.ui
    playback/playbackwidget.ui
)

pte_library(
    NAME ptewidgets
    SOURCES ${srcs}
    HEADERS ${headers}
    MOC_HEADERS ${moc_headers}
    FORMS ${forms}
    DEPENDS
        ptescore
        Qt5::Widgets
)
