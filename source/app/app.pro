include (../../common.pri)

# Find the SVN revision number.
DEFINES += "SVN_REVISION=$$system(svnversion $${PWD} -n)"

INCLUDEPATH += .

SOURCES += \
    powertabeditor.cpp \
    scorearea.cpp \
    documentmanager.cpp \
    settings.cpp \
    skinmanager.cpp \
    command.cpp \
    clipboard.cpp \
    recentfiles.cpp \
    tuningdictionary.cpp \
    options.cpp

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
    tuningdictionary.h \
    options.h \
    tclap/ZshCompletionOutput.h \
    tclap/XorHandler.h \
    tclap/Visitor.h \
    tclap/VersionVisitor.h \
    tclap/ValuesConstraint.h \
    tclap/ValueArg.h \
    tclap/UnlabeledValueArg.h \
    tclap/UnlabeledMultiArg.h \
    tclap/SwitchArg.h \
    tclap/StdOutput.h \
    tclap/StandardTraits.h \
    tclap/OptionalUnlabeledTracker.h \
    tclap/MultiSwitchArg.h \
    tclap/MultiArg.h \
    tclap/IgnoreRestVisitor.h \
    tclap/HelpVisitor.h \
    tclap/DocBookOutput.h \
    tclap/Constraint.h \
    tclap/CmdLineOutput.h \
    tclap/CmdLineInterface.h \
    tclap/CmdLine.h \
    tclap/ArgTraits.h \
    tclap/ArgException.h \
    tclap/Arg.h
