include (../../common.pri)

# Find the SVN revision number. The 'tr' command is used to remove any extra
# letters in the string provided by svnversion (e.g. "875M" => "875").
unix:DEFINES += "SVN_REVISION=$$system(svnversion $${PWD} -n | tr -cd [:digit:])"

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
