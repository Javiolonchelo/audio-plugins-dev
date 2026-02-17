; French Coconut Gain - Inno Setup Script
; All machine-specific paths are injected by CMake via /D flags.
; Run through CMake:  cmake --build cmake-build-release --target Installer
; Run manually:       ISCC /DMyAppVersion=1.0.0 /DOutputDir=. /DPluginSourceDir=<path> InstallerTemplate.iss

#define MyAppName      "French Coconut Gain"
#define MyAppPublisher "dBob Studio"
#define MyAppURL       "https://github.com/Javiolonchelo/audio-plugins-dev"

; --- CMake-injected defines (override with /D<Name>=<Value> when calling ISCC) ---

#ifndef MyAppVersion
  #define MyAppVersion "0.0.0"
#endif

; Output directory for the generated installer executable.
#ifndef OutputDir
  #define OutputDir "."
#endif

; Root directory containing the built <AppName>.vst3 bundle folder.
; CMake passes the JUCE artefacts path:
;   <build_dir>/<ProjectName>_artefacts/<BuildType>/VST3
#ifndef PluginSourceDir
  #define PluginSourceDir "C:\Program Files\Common Files\VST3"
#endif

; --- Setup ---

[Setup]
; NOTE: AppId uniquely identifies this application — do not reuse for other apps.
AppId={{18124108-B4BC-497F-BB82-66423C8472CD}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
CreateAppDir=no
; License screen is shown only when a file path is passed via /DLicenseFile=...
#ifdef LicenseFile
LicenseFile={#LicenseFile}
#endif
OutputDir={#OutputDir}
OutputBaseFilename="{#MyAppName} v{#MyAppVersion}"
SolidCompression=yes
WizardStyle=modern
; Force 64-bit install mode so the plugin lands in the correct Program Files path.
ArchitecturesInstallIn64BitMode=x64

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
; Packages the entire .vst3 bundle from the CMake build artefacts.
; NOTE: Do not use "Flags: ignoreversion" on shared system files.
Source: "{#PluginSourceDir}\{#MyAppName}.vst3\*"; \
  DestDir: "{commonpf64}\Common Files\VST3\{#MyAppName}.vst3"; \
  Flags: ignoreversion recursesubdirs createallsubdirs
