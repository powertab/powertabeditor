<h1 align="center">
  <b>
    Power Tab Editor
  </b>
</h1>

<p align="center"> Guitar tablature viewer and editor </p>
<p align="center"><a href="https://powertab.github.io/"><b>Visit our website</b></a></p>

<div align="center">

  [![Build Status](https://github.com/powertab/powertabeditor/workflows/Build/badge.svg)](https://github.com/powertab/powertabeditor/actions)

  [![Translation status](https://hosted.weblate.org/widgets/powertabeditor/-/powertabeditor/287x66-white.png)](https://hosted.weblate.org/engage/powertabeditor/)

  [![Get it from the Snap Store](https://snapcraft.io/static/images/badges/en/snap-store-black.svg)](https://snapcraft.io/powertabeditor)
  <a href='https://flathub.org/apps/details/com.github.powertab.powertabeditor'><img width='185' height='58' alt='Download on Flathub' src='https://flathub.org/assets/badges/flathub-badge-en.png'/></a>
</div>


## Table of Contents

  - [About](#about-power-tab-editor)
    - [Key Features](#key-features)
    - [Supported File Types](#supported-file-types)
  - [Users](#users)
  - [Developers](#developers)
    - [Dependencies](#dependencies)
    - [Building](#building)
      - [Windows](#windows)
        - [PowerShell](#powershell-installation-scripts)
      - [Linux](#linux)
      - [OS X](#os-x)


## About Power Tab Editor
Power Tab Editor 2.0 - A powerful cross platform guitar tablature viewer and editor inspired by the ceased development and missing source code from the original [Power Tab Editor](http://www.power-tab.net/guitar.php). This project is open-source and written from scratch so that your favorite tabbing platform can continuously grow with your needs.

### Key Features
- Cross platform - Windows, Mac, & Linux
- Tabbed layout for opening multiple files at the same time
- Mixer interface for adjusting volumes during playback
- Complete customization of keyboard shortcuts
- Importing of Guitar Pro tabs

### Supported File Types
- .pt2
- .ptb
- .gp3, .gp4, .gp5
- .gpx
- .gp

## Users
* New user? Read the [intro page](https://github.com/powertab/powertabeditor/wiki/Power-Tab-Editor-2.0,-Here-at-last!) to learn more about the project!
* Download:
  - Latest stable releases [ here ](https://github.com/powertab/powertabeditor/releases)
  - Latest development releases [ here ](https://github.com/powertab/powertabeditor/actions) (requires a GitHub login to download)
* You can report bugs or suggest features on our [issue tracker](https://github.com/powertab/powertabeditor/issues), or discuss ideas on the [discussions forum](https://github.com/powertab/powertabeditor/discussions)
* You can [translate](https://hosted.weblate.org/engage/powertabeditor/) the project into your language.
* Join the [Discord community](https://discord.gg/mpXfquuzxe)

## Developers
* Development help is very welcome! If you're interested in contributing, the issue tracker has a list of [good first issues](https://github.com/powertab/powertabeditor/issues?q=is%3Aissue+is%3Aopen+label%3A%22good+first+issue%22) that are easier to dive into!
* Feel free to ask development-related questions on the [discussions page](https://github.com/powertab/powertabeditor/discussions) or on [Discord](https://discord.gg/mpXfquuzxe)
* See below for instructions on how to compile and run the project:

### Dependencies:
* [CMake](http://www.cmake.org/) >= 3.12
* [Boost](http://www.boost.org/) >= 1.74
  * algorithm
  * date_time
  * endian
  * functional
  * iostreams
  * operators
  * rational
  * signals2
  * stacktrace
* [Qt](http://qt-project.org/) >= 6.2 version or greater
* [nlohmann-json](https://github.com/nlohmann/json) >= 3.7.3
* [RtMidi](https://www.music.mcgill.ca/~gary/rtmidi/) >= 4.0
* [pugixml](https://pugixml.org/)
* [minizip](https://github.com/madler/zlib)
* [doctest](https://github.com/onqtam/doctest)
* (Linux only) - ALSA library (e.g. `libasound2-dev`)
* (Linux only) - MIDI sequencer (e.g. `timidity-daemon`)
* A compiler with C++20 support.

### Building:
#### Windows:
* Install Git - see https://help.github.com/articles/set-up-git
* Install [vcpkg](https://github.com/microsoft/vcpkg) and run `vcpkg install --triplet x64-windows boost-algorithm boost-date-time boost-endian boost-functional boost-iostreams boost-rational boost-signals2 boost-stacktrace doctest minizip nlohmann-json pugixml` to install dependencies.
* Install Qt by running `vcpkg install --triplet x64-windows qtbase qttools` (this may take a while), or install a binary release from the Qt website or https://github.com/miurahr/aqtinstall.
* Open the project folder in Visual Studio and build.
  * If running CMake manually, set `CMAKE_TOOLCHAIN_FILE` to `[vcpkg root]\scripts\buildsystems\vcpkg.cmake`

##### PowerShell Installation Scripts
* For a more installation streamlined process
  - with at least PowerShell 5.1 is installed on your machine
  - Run these powershell scripts in your powershell terminal with your shell working directory as your cloned project
```powershell
.\scripts\Get-Dependencies.ps1 -vcpkg_exe_path "your/path/to/your/vcpkg.exe" -python_exe_path "your/path/to/your/python.exe";
.\scripts\Run-CMakeBuild.ps1 -vcpkg_dir "your/path/to/your/vcpkg" -qt_path "your/path/to/qt";
```
You'll have to update the variables for your specific paths to `$vcpkg_exe_path`, `$python_exe_path`, and `$qt_path`


#### Linux:
* These instructions assume a recent Ubuntu/Debian-based system, but the package names should be similar for other package managers.
* Install dependencies:
  * `sudo apt update`
  * `sudo apt install cmake qt6-base-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools libboost-dev libboost-date-time-dev libboost-iostreams-dev nlohmann-json3-dev libasound2-dev librtmidi-dev libpugixml-dev libminizip-dev doctest-dev`
    * You may also need to install `libglx-dev libgl1-mesa-dev` along with Qt6
  * `sudo apt install timidity-daemon` - timidity is not required for building, but is a good sequencer for MIDI playback.
  * Optionally, use [Ninja](http://martine.github.io/ninja/) instead of `make` (`sudo apt install ninja-build`)
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
* Install Xcode along with its Command Line Tools.
* Install dependencies:
  * `brew install boost cmake doctest minizip ninja nlohmann-json pugixml qt6 pugixml rtmidi`
  * Note that the Qt package from homebrew [does not currently work for building the installer|https://github.com/Homebrew/brew/issues/15354]. Installing via https://github.com/miurahr/aqtinstall is an alternative.
* Build with ninja or Xcode:
  * `mkdir build && cd build`
  * Ninja
    * `cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=/usr/local/opt/qt6/lib/cmake ..`
    * `ninja`
  * Xcode
    * `cmake -G Xcode -DCMAKE_PREFIX_PATH=/usr/local/opt/qt6/lib/cmake ..`
    * With Xcode open and build `build/powertabeditor.xcodeproj`
* Run:
  * `open ./bin/Power\ Tab\ Editor.app`
  * `./bin/pte_tests` to run the unit tests.
  * For Xcode, select `Product/Scheme/powertabeditor` and then `Product/Run`.
