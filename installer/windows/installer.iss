#define ProductName "Power Tab Editor"
#define ExeName "powertabeditor"
#define GroupName "Power Tab"
#define ProductVersion "2.0.22"

#define public Dependency_NoExampleSetup
#include "CodeDependencies.iss"

[Setup]
AppId=6cab03ff-a31b-4c76-a4d1-20a37575896a
AppName={#ProductName}
AppPublisher=Power Tab Community
AppPublisherURL=https://github.com/powertab/powertabeditor
AppVerName={#ProductName} {#ProductVersion}
AppVersion={#ProductVersion}
ArchitecturesAllowed=x86 x64
ArchitecturesInstallIn64BitMode=x64
ChangesAssociations=yes
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
Source: "build\bin\*"; Excludes: "pte_tests.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\bin\data\tunings.json"; DestDir: "{app}\data"; Flags: ignoreversion
Source: "build\bin\data\translations\*"; DestDir: "{app}\data\translations"; Flags: ignoreversion
Source: "build\bin\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion
Source: "build\bin\styles\*"; DestDir: "{app}\styles"; Flags: ignoreversion

[Run]
Filename: "{app}\powertabeditor.exe"; Flags: nowait postinstall skipifsilent; Description: "{cm:LaunchProgram,{#ProductName}}"

; Hooks for MSVC runtime installation
[Code]
procedure InitializeWizard;
begin
  Dependency_InitializeWizard;
end;

function PrepareToInstall(var NeedsRestart: Boolean): String;
begin
  Result := Dependency_PrepareToInstall(NeedsRestart);
end;

function NeedRestart: Boolean;
begin
  Result := Dependency_NeedRestart;
end;

function UpdateReadyMemo(const Space, NewLine, MemoUserInfoInfo, MemoDirInfo, MemoTypeInfo, MemoComponentsInfo, MemoGroupInfo, MemoTasksInfo: String): String;
begin
  Result := Dependency_UpdateReadyMemo(Space, NewLine, MemoUserInfoInfo, MemoDirInfo, MemoTypeInfo, MemoComponentsInfo, MemoGroupInfo, MemoTasksInfo);
end;

function InitializeSetup: Boolean;
begin
  Dependency_AddVC2015To2022;
  Result := True;
end;

[Tasks]
Name: pt2Assoc; Description: "{cm:AssocFileExtension,{#ProductName},.pt2}";
Name: ptbAssoc; Description: "{cm:AssocFileExtension,{#ProductName},.ptb}";
Name: gp3Assoc; Description: "{cm:AssocFileExtension,{#ProductName},.gp3}";
Name: gp4Assoc; Description: "{cm:AssocFileExtension,{#ProductName},.gp4}";
Name: gp5Assoc; Description: "{cm:AssocFileExtension,{#ProductName},.gp5}";
Name: gpxAssoc; Description: "{cm:AssocFileExtension,{#ProductName},.gpx}";
Name: gpAssoc; Description: "{cm:AssocFileExtension,{#ProductName},.gp}";

[Registry]
; .pt2 file association
Root: HKA; Subkey: "Software\Classes\.pt2\OpenWithProgids"; ValueType: string; ValueName: "{#ExeName}.pt2"; ValueData: ""; Flags: uninsdeletevalue; Tasks: pt2Assoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.pt2"; ValueType: string; ValueName: ""; ValueData: "Power Tab Document"; Flags: uninsdeletekey; Tasks: pt2Assoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.pt2\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#ExeName}.exe,0"; Tasks: pt2Assoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.pt2\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#ExeName}.exe"" ""%1"""; Tasks: pt2Assoc
Root: HKA; Subkey: "Software\Classes\Applications\{#ExeName}.exe\SupportedTypes"; ValueType: string; ValueName: ".pt2"; ValueData: ""; Tasks: pt2Assoc

; .ptb file association
Root: HKA; Subkey: "Software\Classes\.ptb\OpenWithProgids"; ValueType: string; ValueName: "{#ExeName}.ptb"; ValueData: ""; Flags: uninsdeletevalue; Tasks: ptbAssoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.ptb"; ValueType: string; ValueName: ""; ValueData: "Power Tab Document"; Flags: uninsdeletekey; Tasks: ptbAssoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.ptb\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#ExeName}.exe,0"; Tasks: ptbAssoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.ptb\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#ExeName}.exe"" ""%1"""; Tasks: ptbAssoc
Root: HKA; Subkey: "Software\Classes\Applications\{#ExeName}.exe\SupportedTypes"; ValueType: string; ValueName: ".ptb"; ValueData: ""; Tasks: ptbAssoc

; Guitar Pro file associations
Root: HKA; Subkey: "Software\Classes\.gp3\OpenWithProgids"; ValueType: string; ValueName: "{#ExeName}.gp3"; ValueData: ""; Flags: uninsdeletevalue; Tasks: gp3Assoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.gp3"; ValueType: string; ValueName: ""; ValueData: "Guitar Pro Document"; Flags: uninsdeletekey; Tasks: gp3Assoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.gp3\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#ExeName}.exe,0"; Tasks: gp3Assoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.gp3\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#ExeName}.exe"" ""%1"""; Tasks: gp3Assoc
Root: HKA; Subkey: "Software\Classes\Applications\{#ExeName}.exe\SupportedTypes"; ValueType: string; ValueName: ".gp3"; ValueData: ""; Tasks: gp3Assoc

Root: HKA; Subkey: "Software\Classes\.gp4\OpenWithProgids"; ValueType: string; ValueName: "{#ExeName}.gp4"; ValueData: ""; Flags: uninsdeletevalue; Tasks: gp4Assoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.gp4"; ValueType: string; ValueName: ""; ValueData: "Guitar Pro Document"; Flags: uninsdeletekey; Tasks: gp4Assoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.gp4\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#ExeName}.exe,0"; Tasks: gp4Assoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.gp4\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#ExeName}.exe"" ""%1"""; Tasks: gp4Assoc
Root: HKA; Subkey: "Software\Classes\Applications\{#ExeName}.exe\SupportedTypes"; ValueType: string; ValueName: ".gp4"; ValueData: ""; Tasks: gp4Assoc

Root: HKA; Subkey: "Software\Classes\.gp5\OpenWithProgids"; ValueType: string; ValueName: "{#ExeName}.gp5"; ValueData: ""; Flags: uninsdeletevalue; Tasks: gp5Assoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.gp5"; ValueType: string; ValueName: ""; ValueData: "Guitar Pro Document"; Flags: uninsdeletekey; Tasks: gp5Assoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.gp5\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#ExeName}.exe,0"; Tasks: gp5Assoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.gp5\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#ExeName}.exe"" ""%1"""; Tasks: gp5Assoc
Root: HKA; Subkey: "Software\Classes\Applications\{#ExeName}.exe\SupportedTypes"; ValueType: string; ValueName: ".gp5"; ValueData: ""; Tasks: gp5Assoc

Root: HKA; Subkey: "Software\Classes\.gpx\OpenWithProgids"; ValueType: string; ValueName: "{#ExeName}.gpx"; ValueData: ""; Flags: uninsdeletevalue; Tasks: gpxAssoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.gpx"; ValueType: string; ValueName: ""; ValueData: "Guitar Pro Document"; Flags: uninsdeletekey; Tasks: gpxAssoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.gpx\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#ExeName}.exe,0"; Tasks: gpxAssoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.gpx\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#ExeName}.exe"" ""%1"""; Tasks: gpxAssoc
Root: HKA; Subkey: "Software\Classes\Applications\{#ExeName}.exe\SupportedTypes"; ValueType: string; ValueName: ".gpx"; ValueData: ""; Tasks: gpxAssoc

Root: HKA; Subkey: "Software\Classes\.gp\OpenWithProgids"; ValueType: string; ValueName: "{#ExeName}.gp"; ValueData: ""; Flags: uninsdeletevalue; Tasks: gpAssoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.gp"; ValueType: string; ValueName: ""; ValueData: "Guitar Pro Document"; Flags: uninsdeletekey; Tasks: gpAssoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.gp\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#ExeName}.exe,0"; Tasks: gpAssoc
Root: HKA; Subkey: "Software\Classes\{#ExeName}.gp\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#ExeName}.exe"" ""%1"""; Tasks: gpAssoc
Root: HKA; Subkey: "Software\Classes\Applications\{#ExeName}.exe\SupportedTypes"; ValueType: string; ValueName: ".gp"; ValueData: ""; Tasks: gpAssoc
