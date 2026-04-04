#!/bin/bash
set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_status() { echo -e "${BLUE}[INFO]${NC} $1"; }
print_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
print_warning() { echo -e "${YELLOW}[WARNING]${NC} $1"; }
print_error() { echo -e "${RED}[ERROR]${NC} $1"; }

if [ $# -lt 6 ]; then
    print_error "Missing required arguments!"
    echo ""
    echo "Usage: $0 <BUILD_CONFIG> <BUNDLE_ID> <PLUGIN_NAME> <VERSION> <PROJECT_NAME> <PLUGIN_DIR> [LICENSE_FILE]"
    echo ""
    echo "Example:"
    echo "  $0 Release com.dBobStudio.LeslieMaxwell \"Leslie Maxwell\" 1.0.0 LeslieMaxwell /path/to/LeslieMaxwell /path/to/LICENSE"
    echo ""
    exit 1
fi

BUILD_CONFIG="$1"
PLUGIN_BUNDLE_ID="$2"
PLUGIN_NAME="$3"
PLUGIN_VERSION="$4"
PROJECT_NAME="$5"
PLUGIN_DIR="$6"
LICENSE_FILE="${7:-}"

if [ "$BUILD_CONFIG" != "Release" ] && [ "$BUILD_CONFIG" != "RelWithDebInfo" ]; then
    print_error "Only Release builds can be used for installers!"
    print_error "Current build configuration: $BUILD_CONFIG"
    print_error "Please build in Release mode to create a distributable installer."
    exit 1
fi

print_status "Project: $PROJECT_NAME"
print_status "Plugin: $PLUGIN_NAME v$PLUGIN_VERSION"
print_status "Bundle ID: $PLUGIN_BUNDLE_ID"
print_status "Build configuration: $BUILD_CONFIG"
print_status "Plugin directory: $PLUGIN_DIR"

check_env_var() {
    if [ -z "${!1}" ]; then
        print_error "Environment variable $1 is not set!"
        print_error "Please ensure your ~/.zshrc is properly configured and sourced."
        print_error "Run: source ~/.zshrc"
        exit 1
    fi
}

print_status "Checking Apple Developer credentials..."
check_env_var "APPLE_CODESIGN_IDENTITY"
check_env_var "APPLE_TEAM_ID"
check_env_var "APPLE_ID_EMAIL"
check_env_var "NOTARIZATION_KEYCHAIN_PROFILE"
print_success "All credentials are set"

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( cd "$PLUGIN_DIR" && pwd )"

BUILD_DIR="$PROJECT_ROOT/cmake-build-$(echo $BUILD_CONFIG | tr '[:upper:]' '[:lower:]')"
ARTEFACTS_DIR="$BUILD_DIR/${PROJECT_NAME}_artefacts/$BUILD_CONFIG"
VST3_PATH="$ARTEFACTS_DIR/VST3/$PLUGIN_NAME.vst3"
AU_PATH="$ARTEFACTS_DIR/AU/$PLUGIN_NAME.component"
OUTPUT_DIR="$BUILD_DIR/installer_output"
ENTITLEMENTS="$SCRIPT_DIR/entitlements.plist"

print_status "Artifacts directory: $ARTEFACTS_DIR"

mkdir -p "$OUTPUT_DIR"

if [ -n "$LICENSE_FILE" ] && [ -f "$LICENSE_FILE" ]; then
    cp "$LICENSE_FILE" "$OUTPUT_DIR/LICENSE"
    print_status "License file: $LICENSE_FILE"
else
    print_warning "No license file provided or file not found — installer will have no license screen"
fi

if [ ! -d "$VST3_PATH" ]; then
    print_error "VST3 plugin not found at: $VST3_PATH"
    print_error "Please build the plugin first using CMake/CLion"
    exit 1
fi

if [ ! -d "$AU_PATH" ]; then
    print_error "AU plugin not found at: $AU_PATH"
    print_error "Please build the plugin first using CMake/CLion"
    exit 1
fi

print_success "Found plugin artifacts"

print_status "Signing VST3 plugin..."
codesign --force \
    --sign "$APPLE_CODESIGN_IDENTITY" \
    --timestamp \
    --options runtime \
    --entitlements "$ENTITLEMENTS" \
    --deep \
    "$VST3_PATH"

codesign --verify --deep --strict --verbose=2 "$VST3_PATH" 2>&1
if [ $? -eq 0 ]; then
    print_success "VST3 plugin signed successfully"
else
    print_error "VST3 signature verification failed"
    exit 1
fi

print_status "Signing AU plugin..."
codesign --force \
    --sign "$APPLE_CODESIGN_IDENTITY" \
    --timestamp \
    --options runtime \
    --entitlements "$ENTITLEMENTS" \
    --deep \
    "$AU_PATH"

codesign --verify --deep --strict --verbose=2 "$AU_PATH" 2>&1
if [ $? -eq 0 ]; then
    print_success "AU plugin signed successfully"
else
    print_error "AU signature verification failed"
    exit 1
fi

print_status "Creating component packages..."

# Create separate staging areas
STAGING_VST3="$OUTPUT_DIR/staging_vst3"
STAGING_AU="$OUTPUT_DIR/staging_au"
rm -rf "$STAGING_VST3" "$STAGING_AU"

mkdir -p "$STAGING_VST3/Library/Audio/Plug-Ins/VST3"
mkdir -p "$STAGING_AU/Library/Audio/Plug-Ins/Components"

cp -R "$VST3_PATH" "$STAGING_VST3/Library/Audio/Plug-Ins/VST3/"
cp -R "$AU_PATH" "$STAGING_AU/Library/Audio/Plug-Ins/Components/"

# Build individual component PKGs (these are invisible to the user)
pkgbuild --root "$STAGING_VST3" --identifier "${PLUGIN_BUNDLE_ID}.vst3" --version "$PLUGIN_VERSION" --install-location "/" "$OUTPUT_DIR/vst3_comp.pkg"
pkgbuild --root "$STAGING_AU" --identifier "${PLUGIN_BUNDLE_ID}.au" --version "$PLUGIN_VERSION" --install-location "/" "$OUTPUT_DIR/au_comp.pkg"

# Generate the Distribution XML
DIST_XML="$OUTPUT_DIR/distribution.xml"
LICENSE_XML=""
if [ -f "$OUTPUT_DIR/LICENSE" ]; then
    LICENSE_XML='    <license file="LICENSE" mime-type="text/plain"/>'
fi

cat <<EOF > "$DIST_XML"
<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="1">
    <title>${PLUGIN_NAME}</title>
    <options customize="always" require-scripts="false" hostArchitectures="x86_64,arm64"/>
${LICENSE_XML}
    <choices-outline>
        <line choice="vst3_choice"/>
        <line choice="au_choice"/>
    </choices-outline>
    <choice id="vst3_choice" title="VST3 Plugin" visible="true" selected="true">
        <pkg-ref id="${PLUGIN_BUNDLE_ID}.vst3"/>
    </choice>
    <choice id="au_choice" title="Audio Unit (AU) Plugin" visible="true" selected="true">
        <pkg-ref id="${PLUGIN_BUNDLE_ID}.au"/>
    </choice>
    <pkg-ref id="${PLUGIN_BUNDLE_ID}.vst3" version="${PLUGIN_VERSION}" onConclusion="none">vst3_comp.pkg</pkg-ref>
    <pkg-ref id="${PLUGIN_BUNDLE_ID}.au" version="${PLUGIN_VERSION}" onConclusion="none">au_comp.pkg</pkg-ref>
</installer-gui-script>
EOF

print_status "Synthesizing product archive..."
UNSIGNED_PKG="$OUTPUT_DIR/${PLUGIN_NAME// /_}-unsigned.pkg"

# Use productbuild instead of pkgbuild for the final step
productbuild --distribution "$DIST_XML" \
    --package-path "$OUTPUT_DIR" \
    --resources "$OUTPUT_DIR" \
    "$UNSIGNED_PKG"

print_success "Unsigned package created"

print_status "Signing installer package..."

INSTALLER_IDENTITY=$(security find-identity -v -p basic | grep "Developer ID Installer" | head -1 | sed -n 's/.*"\(.*\)"/\1/p')

if [ -z "$INSTALLER_IDENTITY" ]; then
    print_warning "No 'Developer ID Installer' certificate found."
    print_warning "Attempting to sign with Application certificate..."
    INSTALLER_IDENTITY="$APPLE_CODESIGN_IDENTITY"
fi

print_status "Using installer identity: $INSTALLER_IDENTITY"

SIGNED_PKG="$OUTPUT_DIR/${PLUGIN_NAME// /_}-v${PLUGIN_VERSION}.pkg"
productsign --sign "$INSTALLER_IDENTITY" \
    "$UNSIGNED_PKG" \
    "$SIGNED_PKG"

if [ $? -eq 0 ]; then
    print_success "Installer package signed successfully"
    rm "$UNSIGNED_PKG"
    # Clean up intermediate component packages
    rm -f "$OUTPUT_DIR/vst3_comp.pkg" "$OUTPUT_DIR/au_comp.pkg"
    rm -f "$DIST_XML"
    rm -rf "$STAGING_VST3" "$STAGING_AU"
else
    print_error "Failed to sign installer package"
    exit 1
fi

# Rename to website-compatible format: MAC_{ProjectName}_v{version}.pkg
FINAL_PKG="$OUTPUT_DIR/MAC_${PROJECT_NAME}_v${PLUGIN_VERSION}.pkg"
if [ "$SIGNED_PKG" != "$FINAL_PKG" ]; then
    mv "$SIGNED_PKG" "$FINAL_PKG"
    SIGNED_PKG="$FINAL_PKG"
    print_status "Renamed installer to: $(basename "$FINAL_PKG")"
fi

print_status "Submitting package for notarization..."
print_status "This may take several minutes..."

NOTARIZATION_OUTPUT=$(xcrun notarytool submit "$SIGNED_PKG" \
    --keychain-profile "$NOTARIZATION_KEYCHAIN_PROFILE" \
    --wait 2>&1)

echo "$NOTARIZATION_OUTPUT"

if echo "$NOTARIZATION_OUTPUT" | grep -q "status: Accepted"; then
    print_success "Notarization successful!"
else
    print_error "Notarization failed!"
    print_error "Run this command to see details:"
    SUBMISSION_ID=$(echo "$NOTARIZATION_OUTPUT" | grep "id:" | head -1 | awk '{print $2}')
    if [ -n "$SUBMISSION_ID" ]; then
        print_error "xcrun notarytool log $SUBMISSION_ID --keychain-profile $NOTARIZATION_KEYCHAIN_PROFILE"
    fi
    exit 1
fi

print_status "Stapling notarization ticket..."
xcrun stapler staple "$SIGNED_PKG"

if [ $? -eq 0 ]; then
    print_success "Notarization ticket stapled successfully"
else
    print_warning "Failed to staple ticket (notarization may still be valid)"
fi

echo ""
print_success "=========================================="
print_success "  PLUGIN SIGNED & NOTARIZED SUCCESSFULLY"
print_success "=========================================="
echo ""
print_status "Installer package: $SIGNED_PKG"
print_status "Build configuration: $BUILD_CONFIG"
print_status "Plugin version: $PLUGIN_VERSION"
echo ""
print_status "You can now distribute this installer to users!"
echo ""
