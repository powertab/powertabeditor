# common build settings for all libraries/executables

QT += core gui

QMAKE_CXXFLAGS+=-std=c++0x -U__STRICT_ANSI__ -Wall -Werror -Wnon-virtual-dtor -Wextra

INCLUDEPATH += $${PWD}/source

# store the subdirectory used for the active build type
CONFIG(debug, debug|release) {
BUILDTYPE = debug
}
else {
BUILDTYPE = release
}

# useful commands for QMAKE_POST_LINK (i.e. copying data files, skins, etc to output directory)
win {
    COPY_DIR = xcopy /E /Y
    COPY_FILE = xcopy /Y
}
unix|macx {
    COPY_DIR = cp -rf
    COPY_FILE = cp -f
}

# keep the generated files somewhat separate
DESTDIR = $${BUILDTYPE}
OBJECTS_DIR = $${BUILDTYPE}/obj
MOC_DIR = $${BUILDTYPE}/moc
RCC_DIR = $${BUILDTYPE}/rcc

# Replace this with the path to your boost installation if you're compiling on Windows
# There shouldn't be any need for a unix:INCLUDEPATH, since boost should already be
# in the include path (/usr/include) if you installed the package from your Linux distribution's repo
win32:INCLUDEPATH += 'C:/Program Files (x86)/boost/boost_1_44/'

# build all subprojects as static libraries unless overridden
TEMPLATE = lib
CONFIG += staticlib
