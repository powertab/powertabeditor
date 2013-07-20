include (../../common.pri)

HEADERS += \
    fileformat.h \
    fileformatmanager.h \
    guitar_pro/guitarproimporter.h \
    guitar_pro/inputstream.h \
    guitar_pro/gp_channel.h \
    guitar_pro/gp_fileformat.h \
    gpx/util.h \
    gpx/filesystem.h \
    gpx/bitstream.h \
    gpx/gpximporter.h \
    gpx/documentreader.h \
    scorearranger.h \
    gpx/pugixml/pugixml.hpp \
    gpx/pugixml/pugiconfig.hpp \
    gpx/pugixml/foreach.hpp \
    powertab/powertabimporter.h \
    powertab/powertabexporter.h \
    powertab/common.h

SOURCES += \
    fileformat.cpp \
    fileformatmanager.cpp \
    guitar_pro/guitarproimporter.cpp \
    guitar_pro/inputstream.cpp \
    guitar_pro/gp_channel.cpp \
    gpx/util.cpp \
    gpx/filesystem.cpp \
    gpx/bitstream.cpp \
    gpx/gpximporter.cpp \
    gpx/documentreader.cpp \
    scorearranger.cpp \
    gpx/pugixml/pugixml.cpp \
    powertab/powertabimporter.cpp \
    powertab/powertabexporter.cpp
