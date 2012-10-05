#!/bin/bash
#
# This file is part of PowertabEditor.
#
# PowertabEditor is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# PowertabEditor is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with PowertabEditor.  If not, see <http://www.gnu.org/licenses/>.
#


################################################################################
##
## == Overview ==
##
## 1. Parse Args 
## 2. Setup for local system
## 3. Do the build
## 4. Package as a <app>.dmg for mac
##
################################################################################



################################################################################
# Function    : DisplayUsage
#
# Description : How to use this script.
#
# Parameters  : 
#     NONE
#
# Returns     :
#     The usage text
#
################################################################################
function DisplayUsage()
{
    echo "
USAGE

    build-macx-sh -h
    build-macx-sh [options]

OPTIONS
    -clean      Perform a make clean first

    -rmClean    Perform a rm clean, deleting the working directory

    -noPackage  Do not produce a dmg package at the end

ENVIRONMENT
    The following environment variables change the behaviour of this script.

    PROJECT_DIR This is the root directory of the project where all files are
                contained. If not defined, it is assume this script is being run
                from \$PROJECT_DIR/build.
OVERVIEW
    This script builds the powertabeditor project on mac, for mac.

SETUP
    This documents the setup up time of writing which was using Mac OS X v10.8.2
     - YMMV.

    Xcode 4.5   You must have XCode, in particular you must install the command
                line tools (preferences -> download). QtSDK will fail to install
                without these.

    QtSDK v4.8.1 
                On OS X Mountain Lion (v10.8) which prevents e.g. std::cout 
                << std::strings from compiling. A workaround is to change 
                -mmacosx-version-min to a higher version (i.e 10.7). The value 
                is defined globally in g++-macx.conf in the QtSDK. See
                http://qt-project.org/forums/viewthread/19106.

                Mountain Lion (v10.8) is *not* supported and Qt throws warnings
                about unsupported version. So far, the only known issue on 10.8
                is this compile warning and the streams. Fix this by manually 
                adding 10.8 as allowed version to qglobal.h (in both Qt and 
                QtCore directories).

                Later Qt SDK's I expect tosolve this, however 4.8.1 was the
                highest released at time of writing.

    boost v1.51.0
                Boost libraries are not shipped by standard in OS X or as part
                of their dev tools (i.e. XCode). This project expects to find
                the Boost libraries and includes in /opt/local/{lib,include}.
                Boost can be install using macports (see 
                http://www.macports.org/ports.php?by=name&substr=boost).

    svn v1.6.18 Subversion command line tools are required from your path (to 
                get build versions numbers). Goto http://subversion.apache.org
                and see \"Getting Subversion\".


EXIT STATUS
    0 on success (anything else is a failure).
"
}


################################################################################
# Function    : Log
#
# Description : Create a simple formatted log heading. Start signifcant events
#               with this.
#
# Parameters  : 
#     Each parameter is printed on it's own line
#
# Returns     :
#     Logging
#
################################################################################
function Log()
{
    # default the number of log entries to 0
    logEntries=${logEntries:-0}

    # a fancy timestamp
    local logPrefix="*** [`printf "%03d" $logEntries` `date -u +%Y%m%d_%T`] ***"

    # print the log entry
    echo
    while [ "$*" ]; do
    {
        echo "$logPrefix $1"
        shift
    } done

    # increment the log count for next entry
    ((logEntries++))
}


################################################################################
# Function    : LogCommand
#
# Description : Standard way to log running a command
#
# Parameters  : 
#     All treated as the command (print on a single line)
#
# Returns     :
#     Logging
#
################################################################################
function LogCommand()
{
    Log "RUNNING COMMAND" "$*"
    echo
}


################################################################################
# Function    : ExecCriticalCmd
#
# Description : Log the command and exit on failure
#
# Parameters  : 
#     Any parameters are treated as the command and its arguments.
#
# Returns     :
#     Logging and output of the command.
#
################################################################################
function ExecCriticalCmd()
{
    LogCommand $@
    $@ 2>&1

    # check if successful
    if [ $? != 0 ]; then
    {
        Log FAILED \
            "\"$*\"" \
            "Build aborted"
        exit 1
    } fi
}


################################################################################
# Function    : ExecCriticalCmdAndStore
#
# Description : Same as ExecCriticalCmd but stores the output of the command
#               in a name var. Only expected to be used when get some simple
#               information from a command line utlilty (e.g. svnversion)
#
# Parameters  : 
#     1st parameter is the name of the variable to store the command output in
#     Any further parameters are treated as the command and its arguments.
#
# Returns     :
#     Logging and the output of the command.
#
################################################################################
function ExecCriticalCmdAndStore()
{
    local retVar=$1
    shift

    LogCommand $@
    cmdOutput=`$@ 2>&1`

    # check if successful
    if [ $? != 0 ]; then
    {
        Log FAILED \
            "\"$*\"" \
            "Build aborted"
        exit 1
    } fi

    echo $cmdOutput
    # store the cmdOutput in var name contained in retVar
    eval $retVar=\"$cmdOutput\"
}


################################################################################
# Function    : ExecOptionalCmd
#
# Description : A simple wrapper to log the command and warn on failure. Call
#               for any command that may fail but the build can still continue.
#
# Parameters  : 
#     Any parameters are treated as the command and its arguments.
#
# Returns     :
#     Logging and the output of the command.
#
################################################################################
function ExecOptionalCmd()
{
    LogCommand $@
    $@ 2>&1

    if [ $? != 0 ]; then
    {
        Log WARNING \
            "Optional command failed"\
            "$*"
    } fi
}



################################################################################
##
## Main 
##
################################################################################

#
# Parse args to determine actions
#
actionRmClean=0
actionMakeClean=0
actionPackage=1

# read each argument passed
while [ "$*" ]; do
{
    # see DisplayUsage for what these do
    case $1 in
    -clean)
        actionMakeClean=1;;
    -rmClean)
        actionRmClean=1;;
    -noPackage)
        actionPackage=0;;
    -h)
        DisplayUsage 
        exit 0;;
    *)
        echo "`basename $0`: illegal option $1"
        DisplayUsage 
        exit 1;;
    esac

    # process the next argument
    shift
} done


#
# Determine directories, versions of what we're building
#
Log START \
    "Arguments parsed successfully"

projectName="powertabeditor"
buildType="debug-macx"

if [ $PROJECT_DIR != "" ]; then
{
    projectDir="$PROJECT_DIR"
} 
else
{
    projectDir="`pwd`/.."
} fi


# base the other directories off the projectDir
sourceDir="${projectDir}/source"
buildDir="${projectDir}/build/${buildType}"
builtAppDir="${buildDir}/build/powertabeditor.app"


# determine the version we're building
ExecCriticalCmdAndStore svnversion "svnversion $projectDir"

version="vdev${svnversion}"
buildName="${projectName}-${version}_${buildType}"


################################################################################
##
## Start the build
##
################################################################################
Log INFO \
    "Project Directory \"$projectDir\"." \
    "Build name \"$buildName\"."

#
# Ensure we have a clean working directory
#
if [ $actionRmClean == 1 ] && [ -d "$buildDir" ]; then
{
    Log INFO \
        "Removing working directory first"
    ExecCriticalCmd chmod -R u+wrx "$buildDir"
    ExecCriticalCmd rm -rf "$buildDir"
} fi

ExecCriticalCmd mkdir -vp "$buildDir"


#
# Generate platform specific Makefile, clean, compile, and link
#
ExecCriticalCmd qmake "${sourceDir}/source.pro" -o "${buildDir}/Makefile"

if [ $actionMakeClean == 1 ]; then
{
    ExecCriticalCmd make --directory="${buildDir}" -j16 clean
}
else
{
    Log SKIPPED \
        "Skipping make clean"
} fi

ExecCriticalCmd make --directory="${buildDir}" -j16

Log INFO \
    "Finished making"

################################################################################
##
## OSX apps are packaged as <app>.dmg, it takes a bit to produce these.
##
## 1. Create a mountable dmg disk image 
## 2. Mount the image and cp files across
## 3. Unmount and compress
##
################################################################################
if [ $actionPackage == 1 ]; then
{
    Log INFO \
        "Creating Package `pwd`${buildName}.dmg"

    #
    # Part 1: Create a big enough disk image
    #

    # the uncompress, working in progress disk image
    workingDmg="${buildDir}/${buildName}.working.dmg"

    # determine how big to make the disk image and allowed some extra room
    ExecCriticalCmdAndStore sizeInMb du -shm ${builtAppDir}
    sizeInMb=$(echo $sizeInMb | cut -f 1 -d ' ')
    sizeInMb=$(( $sizeInMb + 2 ))

    # creates an empty dmg disk image that we can mount (overwriting any 
    # existing file)
    ExecCriticalCmd hdiutil create\
        -size ${sizeInMb}m\
        -fs HFS+\
        -volname "$buildName"\
        -ov\
        "$workingDmg"

    #
    # Part 2: Mount and Copy
    #

    # always the name of the dmg volname (i.e. buildName) because mounting it
    # causes the directory to be renamed to that.
    workingMountPoint="${buildDir}/${buildName}"
    ExecCriticalCmd mkdir -vp "$workingMountPoint"

    # returns 2 rows of mounted devices giving information where the image has
    # been mounted
    ExecCriticalCmdAndStore hdiutilOutput\
        hdiutil attach "$workingDmg" -mountpoint "$workingMountPoint"

    # Space seperated output, get the first column which is dev/<disk>
    dmgDevDisks=$(echo $hdiutilOutput | cut -f 1)

    # doesn't matter which device we use to unmount later so use the first. 
    # (first device is a human recongisable mount point, the second is a 
    # "GUID_partition_scheme").
    dmgDevDisk=$(echo $dmgDevDisks | cut -f 1 -d ' ')

    # Copy app onto the image and correct permissions"
    ExecCriticalCmd cp -Rv "${builtAppDir}" "${workingMountPoint}"
    ExecCriticalCmd chmod -Rv ugo-w "${workingMountPoint}/powertabeditor.app"

    #
    # Part 3: unmount and compress
    #

    # We're now finished with it so unmount the disk
    ExecOptionalCmd hdiutil detach "$dmgDevDisk"
     
    # Finally, compress to make the file size more distributable (using bzip2)
    # have to remove any pre-existing file
    if [ -f ${buildName}.dmg ]; then
    {
        ExecOptionalCmd rm "${buildName}.dmg"
    } fi

    ExecCriticalCmd hdiutil convert\
        "$workingDmg"\
        -format UDBZ\
        -o "${buildName}.dmg"
}
else
{
    Log SKIPPED \
        "Skipping packaging"
} fi


# If we get this far, the build has been a success
Log SUCCESS \
    "Build completed"
exit 0
