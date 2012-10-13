include (../../common.pri)

SOURCES += \
    powertabeditor.cpp \
    scorearea.cpp \
    documentmanager.cpp \
    settings.cpp \
    skinmanager.cpp \
    command.cpp \
    clipboard.cpp \
    recentfiles.cpp

HEADERS += common.h \
    documentmanager.h \
    powertabeditor.h \
    scorearea.h \
    settings.h \
    skinmanager.h \
    command.h \
    clipboard.h \
    recentfiles.h \
    pubsub/pubsub.h \
    pubsub/systemlocationpubsub.h \
    pubsub/settingspubsub.h
