# Code Signing & Notarization Scripts

This directory contains automated scripts for signing and notarizing the French Coconut Gain audio plugin.

## Prerequisites

All sensitive credentials are stored securely:
- **Environment variables**: Stored in `~/.zshrc`
- **App-specific password**: Stored in macOS Keychain (via `notarytool`)

## Quick Start

### Option 1: Build and Sign from CLion

1. Open the project in CLion
2. Select **Release** configuration
3. Build → **Installer** target

### Option 2: Build and Sign from Command Line

```bash
# Navigate to project directory
cd "/path/to/audio-plugins-dev/French Coconut Gain"

# Configure CMake for Release
cmake -B cmake-build-release -DCMAKE_BUILD_TYPE=Release

# Build the plugins
cmake --build cmake-build-release --config Release

# Create signed & notarized installer
cmake --build cmake-build-release --target Installer
```

### Option 3: Run Script Directly

If you've already built the plugins, you can run the script manually:

```bash
# For Release build
./scripts/sign_and_notarize.sh Release "com.dBobStudio.FrenchCoconutGain" "French Coconut Gain" "1.0.0" "French_Coconut_Gain"

# General format
./scripts/sign_and_notarize.sh <BUILD_CONFIG> <BUNDLE_ID> <PLUGIN_NAME> <VERSION> <PROJECT_NAME>
```

**Note:** When using the CMake target `Installer`, all values are automatically passed from your `CMakeLists.txt`.

## What the Script Does

The `sign_and_notarize.sh` script performs the following steps:

1. **Validates Environment**: Checks all required environment variables are set
2. **Signs VST3 Plugin**: Code signs the VST3 with hardened runtime
3. **Signs AU Plugin**: Code signs the Audio Unit with hardened runtime
4. **Creates Installer Package**: Builds a .pkg installer with proper directory structure
5. **Signs Installer**: Signs the package with your Developer ID Installer certificate
6. **Notarizes Package**: Submits to Apple's notarization service
7. **Staples Ticket**: Attaches the notarization ticket to the installer

## Output

The signed and notarized installer will be located at:
```
cmake-build-release/installer_output/French_Coconut_Gain-v1.0.0.pkg
```

## Configuration

### Environment Variables (in `~/.zshrc`)

Apple Developer credentials are stored globally and used for all plugins:

| Variable | Purpose |
|----------|---------|
| `APPLE_CODESIGN_IDENTITY` | Your Developer ID Application certificate |
| `APPLE_TEAM_ID` | Your Apple Developer Team ID |
| `APPLE_ID_EMAIL` | Your Apple ID email for notarization |
| `NOTARIZATION_KEYCHAIN_PROFILE` | Keychain profile name for notarization credentials |

### Plugin-Specific Settings (in `CMakeLists.txt`)

Plugin information is defined in each project's `CMakeLists.txt`:

| Variable | Purpose |
|----------|---------|
| `BUNDLE_ID` | Plugin bundle identifier (e.g., `com.dBobStudio.FrenchCoconutGain`) |
| `PLUGIN_FINAL_NAME` | Display name of the plugin (e.g., `"French Coconut Gain"`) |
| `PROJECT_VERSION` | Plugin version (e.g., `1.0.0`) |

## Updating Credentials

### Update App-Specific Password

If you need to regenerate your app-specific password:

```bash
# 1. Generate new password at https://appleid.apple.com
# 2. Store in keychain
xcrun notarytool store-credentials "dbob-studio-notarization" \
    --apple-id "$APPLE_ID_EMAIL" \
    --team-id "$APPLE_TEAM_ID" \
    --password "your-new-password"
```

### Update Signing Identity

If your certificate changes, update in `~/.zshrc`:

```bash
# Find your new identity
security find-identity -v -p codesigning

# Update APPLE_CODESIGN_IDENTITY in ~/.zshrc
# Then reload: source ~/.zshrc
```

## Troubleshooting

### "Credentials could not be found"

Reload your shell environment:
```bash
source ~/.zshrc
```

### "Notarization failed"

Check notarization logs:
```bash
# Get submission ID from error output, then:
xcrun notarytool log <submission-id> --keychain-profile dbob-studio-notarization
```

### "No 'Developer ID Installer' certificate found"

The script will fall back to your Application certificate. This usually works, but if you need a separate Installer certificate:
1. Go to https://developer.apple.com/account/resources/certificates
2. Create a "Developer ID Installer" certificate
3. Download and install it

### Verify Signatures

```bash
# Check VST3 signature
codesign --verify --deep --strict --verbose=2 "path/to/plugin.vst3"

# Check package signature
pkgutil --check-signature "path/to/installer.pkg"

# Check notarization
spctl -a -vv -t install "path/to/installer.pkg"
```

## Files

- `sign_and_notarize.sh` - Main signing and notarization script
- `entitlements.plist` - Hardened runtime entitlements for audio plugins
- `README.md` - This documentation

## Security Notes

- **Never commit credentials to git**: All sensitive data is in `~/.zshrc` (already gitignored)
- **App-specific password**: Stored securely in macOS Keychain, not in plain text
- **Entitlements**: Required for audio plugins to work with hardened runtime
- **Timestamp**: Always included for certificate validity verification

## Distribution

After the script completes successfully, you can distribute:
- The signed `.pkg` installer to users
- The package will install both VST3 and AU formats

Users can verify the installer:
```bash
spctl -a -vv -t install French_Coconut_Gain-v1.0.0.pkg
```

## DMG Alternative (Optional)

The script includes commented-out code for creating a DMG instead of (or in addition to) PKG. Uncomment the DMG section at the end of `sign_and_notarize.sh` if you prefer DMG distribution.
