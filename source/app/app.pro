include (../../common.pri)

# Find the SVN revision number.
DEFINES += "SVN_REVISION=$$system(svnversion $${PWD} -n)"

SOURCES += \
    powertabeditor.cpp \
    scorearea.cpp \
    documentmanager.cpp \
    settings.cpp \
    skinmanager.cpp \
    command.cpp \
    clipboard.cpp \
    recentfiles.cpp \
    tuningdictionary.cpp

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
    pubsub/settingspubsub.h \
    tuningdictionary.h
