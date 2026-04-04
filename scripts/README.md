# Shared Build & Release Scripts

Shared tooling for all dBob Studio audio plugins. Each plugin references these scripts from its `CMakeLists.txt`.

## Files

| File | Purpose |
|------|---------|
| `sign_and_notarize.sh` | macOS: sign VST3/AU, create .pkg installer, notarize with Apple |
| `entitlements.plist` | Hardened runtime entitlements for macOS audio plugins |
| `installer_template.iss` | Windows: InnoSetup installer template (parameterized via /D flags) |
| `SETUP_SECRETS.md` | Step-by-step guide for setting up GitHub Actions secrets |

## macOS: sign_and_notarize.sh

### Usage

```bash
./scripts/sign_and_notarize.sh <BUILD_CONFIG> <BUNDLE_ID> <PLUGIN_NAME> <VERSION> <PROJECT_NAME> <PLUGIN_DIR> [LICENSE_FILE]
```

### Example

```bash
./scripts/sign_and_notarize.sh Release \
    com.dBobStudio.LeslieMaxwell \
    "Leslie Maxwell" \
    1.0.0 \
    LeslieMaxwell \
    ./LeslieMaxwell \
    ./LICENSE
```

When using the CMake `Installer` target, all values are passed automatically from the plugin's `CMakeLists.txt`.

### Required environment variables

| Variable | Purpose |
|----------|---------|
| `APPLE_CODESIGN_IDENTITY` | Developer ID Application certificate name |
| `APPLE_TEAM_ID` | Apple Developer Team ID |
| `APPLE_ID_EMAIL` | Apple ID email for notarization |
| `NOTARIZATION_KEYCHAIN_PROFILE` | Keychain profile for notarization credentials |

### What it does

1. Validates environment variables
2. Signs VST3 and AU plugins with hardened runtime
3. Creates a .pkg installer with component selection (VST3 / AU)
4. Signs the .pkg with Developer ID Installer certificate
5. Submits to Apple notarization service and waits
6. Staples the notarization ticket

## Windows: installer_template.iss

### Required /D flags (passed by CMake)

| Flag | Example |
|------|---------|
| `/DMyAppName` | `"Leslie Maxwell"` |
| `/DMyAppVersion` | `"1.0.0"` |
| `/DMyAppId` | `{unique-GUID-per-plugin}` |
| `/DOutputDir` | Build output path |
| `/DPluginSourceDir` | Path to built VST3 artefacts |

### Optional /D flags

| Flag | Default |
|------|---------|
| `/DLicenseFile` | *(none)* |
| `/DStandaloneSourceDir` | *(none — standalone component hidden if omitted)* |
| `/DMyAppPublisher` | `"dBob Studio"` |
| `/DMyAppURL` | GitHub repo URL |

## Adding a new plugin

1. Create the plugin directory with its own `CMakeLists.txt`
2. In CMakeLists.txt, reference the shared scripts:
   ```cmake
   set(SCRIPTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../scripts")

   # macOS installer target
   if (APPLE)
       add_custom_target(Installer
           COMMAND ${SCRIPTS_DIR}/sign_and_notarize.sh
               ${CMAKE_BUILD_TYPE} "${BUNDLE_ID}" "${PLUGIN_FINAL_NAME}"
               "${PROJECT_VERSION}" "${PROJECT_NAME}"
               "${CMAKE_CURRENT_SOURCE_DIR}" "${LICENSE_FILE}"
           ...
       )
   endif()

   # Windows installer target
   if (WIN32)
       find_program(ISCC_EXECUTABLE NAMES ISCC.exe ...)
       if (ISCC_EXECUTABLE)
           add_custom_target(Installer
               COMMAND "${ISCC_EXECUTABLE}"
                   "/DMyAppName=${PLUGIN_FINAL_NAME}"
                   "/DMyAppVersion=${PROJECT_VERSION}"
                   "/DMyAppId=${INSTALLER_APP_ID}"
                   ...
                   "${SCRIPTS_DIR}/installer_template.iss"
               ...
           )
       endif()
   endif()
   ```
3. Generate a unique GUID for `INSTALLER_APP_ID` (https://www.guidgen.com)
4. Copy and adapt the GitHub Actions workflows for the new plugin

## Updating credentials

### Regenerate App-Specific Password

```bash
xcrun notarytool store-credentials "dbob-studio-notarization" \
    --apple-id "$APPLE_ID_EMAIL" \
    --team-id "$APPLE_TEAM_ID" \
    --password "your-new-password"
```

### Update signing identity

```bash
security find-identity -v -p codesigning
# Update APPLE_CODESIGN_IDENTITY in ~/.zshrc, then: source ~/.zshrc
```

## Troubleshooting

### "Credentials could not be found"
```bash
source ~/.zshrc
```

### "Notarization failed"
```bash
xcrun notarytool log <submission-id> --keychain-profile dbob-studio-notarization
```

### Verify signatures
```bash
# macOS VST3
codesign --verify --deep --strict --verbose=2 "path/to/plugin.vst3"

# macOS .pkg
pkgutil --check-signature "path/to/installer.pkg"
spctl -a -vv -t install "path/to/installer.pkg"
```
