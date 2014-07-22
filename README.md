# Power Tab Editor

[![Build Status](https://travis-ci.org/powertab/powertabeditor.svg?branch=master)](https://travis-ci.org/powertab/powertabeditor)

#### Compiling
Dependencies:
* [CMake](http://www.cmake.org/) >= 2.8.9
* [Boost 1.55](http://www.boost.org/) (earlier versions may work, but are currently untested)
* [Qt 5.x](http://qt-project.org/)
* [zlib](http://www.zlib.net/)
* (Linux only) - ALSA library (e.g. `libasound2-dev`)
* (Linux only) - MIDI sequencer (e.g. `timidity`)
* A compiler with C++11 support (gcc 4.8, Clang, VS 2013)

Windows:
* Install Git - see https://help.github.com/articles/set-up-git
* Building Boost:
  * Download and extract [Boost 1.55](http://www.boost.org/users/history/version_1_55_0.html) to `C:\Program Files\boost\boost_1_55_0`.
  * Download and extract [zlib](http://www.zlib.net/) to any directory. However, the path must *not* contain any spaces.
  * Open a command prompt (VS2013 x64 Native Tools Command Prompt) and navigate to the Boost directory.
  * Run `bootstrap` and then `b2 -s ZLIB_SOURCE=/path/to/zlib link=shared address-model=64`.
* Installing Qt:
  * Download the online installer from http://qt-project.org/downloads.
  * Install the `msvc2013 64-bit` component.
* Install and open CMake, and browse to select the location of the root directory (e.g. `$HOME/Documents/GitHub/powertabeditor`).
* Set the build directory to something like `$HOME/Documents/GitHub/powertabeditor/build_cmake`.
* Use the "Add Entry" button to set the `STRING` `CMAKE_PREFIX_PATH` to the `cmake` directory inside Qt's installation directory (e.g. `C:\Qt\5.2.0\qtbase\lib\cmake`)
* Press `Configure` and select your compiler version (e.g. `Visual Studio 12 Win64`) and then press `Generate`
* Open the resulting solution (`powertabeditor.sln`) and select `Build Solution` from the `Build` menu.
* Right-click on the `powertabeditor` project and select "Set as Startup Project" before running.
