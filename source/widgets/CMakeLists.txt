project( ptewidgets )

set( srcs
    clickablelabel.cpp
    common.cpp
    instruments/instrumentpanel.cpp
    instruments/instrumentpanelitem.cpp
    mixer/mixer.cpp
    mixer/mixeritem.cpp
    playback/playbackwidget.cpp
    toolbox/toolbox.cpp
)

set( headers
    clickablelabel.h
    instruments/instrumentpanel.h
    instruments/instrumentpanelitem.h
    mixer/mixer.h
    mixer/mixeritem.h
    playback/playbackwidget.h
    toolbox/toolbox.h
)

set( moc_headers
    clickablelabel.h
    instruments/instrumentpanel.h
    instruments/instrumentpanelitem.h
    mixer/mixer.h
    mixer/mixeritem.h
    playback/playbackwidget.h
    toolbox/toolbox.h
)

set( forms
    instruments/instrumentpanelitem.ui
    mixer/mixeritem.ui
    playback/playbackwidget.ui
    toolbox/toolbox.ui
)

pte_library(
    NAME ptewidgets
    SOURCES ${srcs}
    HEADERS ${headers}
    MOC_HEADERS ${moc_headers}
    FORMS ${forms}
    DEPENDS
        ptescore
        Qt::Widgets
)
