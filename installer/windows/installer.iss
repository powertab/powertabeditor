#define ProductName "Power Tab Editor"
#define ExeName "powertabeditor"
#define GroupName "Power Tab"
#define ProductVersion "2.0.0-alpha15"

[Setup]
AppId=6cab03ff-a31b-4c76-a4d1-20a37575896a
AppName={#ProductName}
AppPublisher=Power Tab Community
AppPublisherURL=https://github.com/powertab/powertabeditor
AppVerName={#ProductName} {#ProductVersion}
AppVersion={#ProductVersion}
ArchitecturesAllowed=x86 x64
ArchitecturesInstallIn64BitMode=x64
Compression=lzma2
DefaultDirName={autopf}\{#GroupName}\{#ProductName}
DefaultGroupName={#GroupName}
LicenseFile=installer\windows\license.rtf
OutputBaseFilename={#ExeName}
OutputDir=installer\windows
SetupIconFile=source\icons\app_icon.ico
SolidCompression=yes
SourceDir=..\..\
UninstallDisplayIcon={app}\{#ExeName}.exe
VersionInfoVersion=1.98.0.15
WizardSmallImageFile=installer\windows\logo.bmp
WizardStyle=modern

[Icons]
Name: "{group}\{#ProductName}"; Filename: "{app}\{#ExeName}.exe"

[Files]
Source: "build\bin\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs

; TODO - install MSVC runtime
; TODO - file associations

[Run]
Filename: "{app}\powertabeditor.exe"; Flags: nowait postinstall; Description: "{cm:LaunchProgram,{#ProductName}}"
