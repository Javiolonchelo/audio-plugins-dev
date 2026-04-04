; dBob Studio - Shared InnoSetup Installer Template
; All values are injected by CMake via /D flags.
;
; Required /D flags:
;   /DMyAppName=<plugin name>       e.g. "French Coconut Gain"
;   /DMyAppVersion=<version>        e.g. "1.0.0"
;   /DMyAppId=<GUID>                Unique per plugin (generate at https://www.guidgen.com)
;   /DOutputDir=<path>              Output directory for the installer
;   /DPluginSourceDir=<path>        Directory containing the built .vst3 bundle
;
; Optional /D flags:
;   /DLicenseFile=<path>            License file to show during install
;   /DStandaloneSourceDir=<path>    Directory containing the standalone .exe
;   /DMyAppPublisher=<name>         Publisher name (default: "dBob Studio")
;   /DMyAppURL=<url>                Publisher URL

#ifndef MyAppName
  #error "MyAppName must be defined via /DMyAppName=..."
#endif

#ifndef MyAppVersion
  #define MyAppVersion "0.0.0"
#endif

#ifndef MyAppId
  #error "MyAppId must be defined via /DMyAppId=... (use a unique GUID per plugin)"
#endif

#ifndef MyAppPublisher
  #define MyAppPublisher "dBob Studio"
#endif

#ifndef MyAppURL
  #define MyAppURL "https://github.com/Javiolonchelo/audio-plugins-dev"
#endif

#ifndef OutputDir
  #define OutputDir "."
#endif

#ifndef PluginSourceDir
  #error "PluginSourceDir must be defined via /DPluginSourceDir=..."
#endif

[Setup]
AppId={#MyAppId}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppPublisher}\{#MyAppName}
DefaultGroupName={#MyAppPublisher}
OutputDir={#OutputDir}
OutputBaseFilename={#StringChange(MyAppName, " ", "_")}-v{#MyAppVersion}-Windows
Compression=lzma2/ultra64
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64compatible
WizardStyle=modern
#ifdef LicenseFile
LicenseFile={#LicenseFile}
#endif
#ifdef StandaloneSourceDir
UninstallDisplayIcon={app}\{#MyAppName}.exe
#endif
DisableProgramGroupPage=yes

[Types]
Name: "full"; Description: "Full installation"
Name: "vst3only"; Description: "VST3 only"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "vst3"; Description: "VST3 Plugin"; Types: full vst3only custom
#ifdef StandaloneSourceDir
Name: "standalone"; Description: "Standalone Application"; Types: full custom
#endif

[Files]
Source: "{#PluginSourceDir}\{#MyAppName}.vst3\*"; DestDir: "{commoncf64}\VST3\{#MyAppPublisher}\{#MyAppName}.vst3"; Components: vst3; Flags: ignoreversion recursesubdirs createallsubdirs
#ifdef StandaloneSourceDir
Source: "{#StandaloneSourceDir}\*"; DestDir: "{app}"; Components: standalone; Flags: ignoreversion recursesubdirs
#endif

[Icons]
#ifdef StandaloneSourceDir
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppName}.exe"; Components: standalone
Name: "{group}\Uninstall {#MyAppName}"; Filename: "{uninstallexe}"
#endif
