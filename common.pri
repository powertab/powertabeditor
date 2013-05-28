# common build settings for all libraries/executables

QT += core gui

# Add QtWidgets when compiling against Qt5.
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS+= -Wall

# Prevents the compiler from linking against boost::date_time.
win32-msvc*:DEFINES += BOOST_ALL_NO_LIB

win32-g++ {
    QMAKE_CXXFLAGS += -U__STRICT_ANSI__ -fno-strict-aliasing
}

win32-g++ | linux-g++ | macx-g++ | linux-clang {
    QMAKE_CXXFLAGS+= -Werror -Wnon-virtual-dtor -Wextra
}

DEPENDPATH += $${PWD}/source
INCLUDEPATH += $${PWD}/source

# store the subdirectory used for the active build type
CONFIG(debug, debug|release) {
BUILDTYPE = debug
}
else {
BUILDTYPE = release
}

# keep the generated files somewhat separate
DESTDIR = $${BUILDTYPE}
OBJECTS_DIR = $${BUILDTYPE}/obj
MOC_DIR = $${BUILDTYPE}/moc
RCC_DIR = $${BUILDTYPE}/rcc

# Replace this with the path to your boost installation if you're compiling on Windows
# There shouldn't be any need for a unix:INCLUDEPATH, since boost should already be
# in the include path (/usr/include) if you installed the package from your Linux distribution's repo
win32:INCLUDEPATH += 'C:/Program Files/boost/boost_1_49/'

# Using macports for boost, the default local is below
macx:INCLUDEPATH += /opt/local/include

# build all subprojects as static libraries unless overridden
TEMPLATE = lib
CONFIG += staticlib
