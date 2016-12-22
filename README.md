# Power Tab Editor

[![Build Status](https://travis-ci.org/powertab/powertabeditor.svg?branch=master)](https://travis-ci.org/powertab/powertabeditor)
[![Build status](https://ci.appveyor.com/api/projects/status/9q4cnu1l6rjxg96h?svg=true)](https://ci.appveyor.com/project/cameronwhite/powertabeditor)

### Users
* New user? Read the [intro page](https://github.com/powertab/powertabeditor/wiki/Power-Tab-Editor-2.0,-Here-at-last!) to learn more about the project!
* You can get the latest releases from our [downloads page](https://github.com/powertab/powertabeditor/releases).
* You can report bugs or suggest features on our [issue tracker](https://github.com/powertab/powertabeditor/issues).

### Developers
#### Getting the code:

If you're using [GitHub for Windows](https://windows.github.com/), just use the "Clone in Desktop" button.

Otherwise, make sure to use `git clone --recursive` to initialize submodules in the `external` folder:

`git clone --recursive git://github.com/powertab/powertabeditor.git`

If you've already cloned the repository, you can run `git submodule init && git submodule update`.

#### Dependencies:
* [CMake](http://www.cmake.org/) >= 2.8.9
* [Boost 1.55](http://www.boost.org/) or greater
  * Earlier versions may work, but are currently untested
* [Qt 5.4](http://qt-project.org/) or greater
* [zlib](http://www.zlib.net/)
* (Linux only) - ALSA library (e.g. `libasound2-dev`)
* (Linux only) - MIDI sequencer (e.g. `timidity`)
* (Linux only) - libbfd (e.g. `binutils-dev`)
* A compiler with C++11 support (gcc 4.8+, Clang, VS 2013)

#### Windows:
* Install Git - see https://help.github.com/articles/set-up-git
* Building Boost:
  * Download and extract [Boost 1.56](http://www.boost.org/users/history/version_1_56_0.html) to `C:\Program Files\boost\boost_1_56_0`.
  * Download and extract [zlib](http://www.zlib.net/) to any directory. However, the path must *not* contain any spaces.
  * Open a command prompt (e.g. VS2013 x64 Native Tools Command Prompt) and navigate to the Boost directory.
  * Run `bootstrap` and then `b2 -s ZLIB_SOURCE=/path/to/zlib link=shared address-model=64 variant=debug,release`.
    * Use `address-model=32` for a 32-bit build.
    * If building both 32-bit and 64-bit versions, use `--stagedir=stage32` to place the 32-bit versions under a different directory (`stage32/lib` instead of `stage/lib`), and set `BOOST_LIBRARYDIR` accordingly when running CMake.
* Installing Qt:
  * Download the online installer from http://qt-project.org/downloads.
  * Install the `msvc2013 64-bit` component (or `msvc2013 32-bit` for a 32-bit build)
* Compiling Qt (optional):
  * You may want to do this instead if you intend on building the installer so that Qt can be built without dependencies on huge libraries such as `icu`.
  * [Download](http://qt-project.org/downloads) and extract the source code.
  * If necessary, install [Python 2.7.x](https://www.python.org/downloads/) and add it to your PATH.
  * Open a command prompt (e.g. VS2013 x64 Native Tools Command Prompt) and navigate to the Qt source directory.
  * Run `configure -opensource -nomake examples -nomake tests -skip qtwebkit -skip qtwebengine -skip qtconnectivity -skip qtandroidextras -skip qtlocation -skip qtscript -opengl desktop -debug-and-release -mp -no-icu -c++11 -prefix C:\Qt\5.4.1` and accept the license agreement.
  * Run `nmake` and `nmake install` and grab a cup of coffee ...
* Install and open CMake, and browse to select the location of the root directory (e.g. `$HOME/Documents/GitHub/powertabeditor`).
* Set the build directory to `$HOME/Documents/GitHub/powertabeditor/build`.
* Use the "Add Entry" button to set the `STRING` `CMAKE_PREFIX_PATH` to the `cmake` directory inside Qt's installation directory (e.g. `C:\Qt\5.4.1\lib\cmake`)
* For Windows XP support, set `CMAKE_GENERATOR_TOOLSET` to `v140_xp`.
* Press `Configure` and select your compiler version (e.g. `Visual Studio 14 Win64`, or `Visual Studio 14` for a 32-bit build) and then press `Generate`
* Open the resulting solution (`powertabeditor.sln`) and select `Build Solution` from the `Build` menu.
* Right-click on the `powertabeditor` project and select "Set as Startup Project" before running.

#### Linux:
* These instructions assume a recent Ubuntu/Debian-based system, but the package names should be similar for other package managers.
  * For older Ubuntu systems (such as Ubuntu 12.04) - you may need to [add some PPAs](https://github.com/powertab/powertabeditor/blob/master/.travis/setup_linux.sh) to get updated versions of the dependencies.
* Install dependencies:
  * `sudo apt-get update`
  * `sudo apt-get install cmake qtbase5-dev libboost-dev libboost-date-time-dev libboost-filesystem-dev libboost-iostreams-dev libboost-program-options-dev libboost-regex-dev libasound2-dev libiberty-dev binutils-dev rapidjson-dev libpugixml-dev catch librtmidi-dev`
  * `sudo apt-get install timidity` - timidity is not required for building, but is a good sequencer for MIDI playback.
  * Optionally, use [Ninja](http://martine.github.io/ninja/) instead of `make` (`sudo apt-get install ninja-build`)
* Build:
  * `mkdir build && cd build`
  * `cmake ..`
    * Add `-DCMAKE_INSTALL_PREFIX=/some/path` to customize the install directory.
    * Add `-DCMAKE_BUILD_TYPE=Debug` for a debug build instead of a `Release` build.
    * Add `-DCMAKE_CXX_COMPILER=clang++` to compile with Clang.
    * Add `-G Ninja` to generate [Ninja](http://martine.github.io/ninja/) build files.
  * `make -j8` or `ninja`
* Run:
  * `./bin/powertabeditor`
  * `./bin/pte_tests` to run the unit tests.
* Install:
  * `make install` or `ninja install`

#### OS X:
* Tested with Mac OS X 10.9 and above.
* Install Xcode along with its Command Line Tools.
* Install CMake:
  * If you prefer a GUI, download the [CMake installer](http://www.cmake.org).
  * Otherwise, run `brew install cmake`
* Install dependencies:
  * `brew install boost qt5`
* Build:
  * `mkdir build && cd build`
  * `cmake -DCMAKE_PREFIX_PATH=/usr/local/opt/qt5/lib/cmake ..`
    * If necessary, define `BOOST_ROOT` to point to the root directory where Boost was installed (e.g. `/usr/local/opt/boost`).
    * To generate an Xcode project, add `-G Xcode`.
  * For a Makefile build, run `make -j4`.
  * For Xcode, open and build `build/powertabeditor.xcodeproj`.
* Run:
  * `./bin/powertabeditor`
  * `./bin/pte_tests` or `make test` to run the unit tests.
  * For Xcode, select `Product/Scheme/powertabeditor` and then `Product/Run`.
  
