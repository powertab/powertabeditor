There is some additional setup and manual steps required to build the Windows installer.

# Build steps
- Install Wix 3.14 from the Development Releases section of
  https://wixtoolset.org/releases/, along with the Visual Studio extension.
- Build `powertabeditor` in the Release configuration.
- Download the VS redistributable from the link in `common.wxi`, and place it in a folder named `redist` under this directory.
- Open `pte_windows_build.sln` and build it in the Release configuration. Find `powertabeditor.exe` under the `bin` folder.
