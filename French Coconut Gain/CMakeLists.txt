# dBob Studio 2026
# Javier Rodrigo López
# javiolonchelo@gmail.com

cmake_minimum_required(VERSION 3.15)
project(French_Coconut_Gain VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Fetch JUCE and other modules
include(FetchContent)
if(DEFINED ENV{DBOB_STUDIO_JUCE_PATH} AND NOT "$ENV{DBOB_STUDIO_JUCE_PATH}" STREQUAL "")
    FetchContent_Declare(JUCE SOURCE_DIR "$ENV{DBOB_STUDIO_JUCE_PATH}")
else()
    FetchContent_Declare(JUCE
            GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
            GIT_TAG 8.0.7)
endif()
FetchContent_MakeAvailable(JUCE)

FetchContent_Declare (melatonin_inspector
  GIT_REPOSITORY https://github.com/sudara/melatonin_inspector.git
  GIT_TAG origin/main
  SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/melatonin_inspector)
FetchContent_MakeAvailable (melatonin_inspector)

# Documentation
find_package(Doxygen OPTIONAL_COMPONENTS dot)

set(DOXYGEN_ALPHABETICAL_INDEX YES)
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
set(DOXYGEN_CASE_SENSE_NAMES NO)
set(DOXYGEN_CLASS_DIAGRAMS YES)
set(DOXYGEN_DISTRIBUTE_GROUP_DOC YES)
set(DOXYGEN_EXCLUDE bin)
set(DOXYGEN_EXCLUDE_PATTERNS README.md)
set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_EXTRACT_PRIVATE YES)
set(DOXYGEN_FILE_PATTERNS *.h *.md)
set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_HIDE_FRIEND_COMPOUNDS YES)
set(DOXYGEN_HIDE_IN_BODY_DOCS YES)
set(DOXYGEN_HIDE_UNDOC_CLASSES YES)
set(DOXYGEN_HIDE_UNDOC_MEMBERS YES)
set(DOXYGEN_JAVADOC_AUTOBRIEF YES)
set(DOXYGEN_MARKDOWN_ID_STYLE GITHUB)
set(DOXYGEN_QUIET YES)
set(DOXYGEN_RECURSIVE YES)
set(DOXYGEN_REFERENCED_BY_RELATION YES)
set(DOXYGEN_REFERENCES_RELATION YES)
set(DOXYGEN_SORT_BY_SCOPE_NAME YES)
set(DOXYGEN_SORT_MEMBER_DOCS NO)
set(DOXYGEN_SOURCE_BROWSER YES)
set(DOXYGEN_STRIP_CODE_COMMENTS NO)
set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs")

if (DOXYGEN_FOUND)
    doxygen_add_docs(doc "src/"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            COMMENT "Creating HTML documentation...")
endif ()

set(COMPANY_NAME "dBob Studio")
set(PLUGIN_MANUFACTURER_CODE dBob)

# Customize the plugin here
set(BUNDLE_ID com.dBobStudio.FrenchCoconutGain)
set(PLUGIN_CODE Frgn)
if(APPLE)
    set(FORMATS AU VST3 Standalone)
    set(COPY_PLUGIN_AFTER_BUILD TRUE)
else()
    set(FORMATS VST3 Standalone)
    set(COPY_PLUGIN_AFTER_BUILD FALSE)
endif()
set(PLUGIN_FINAL_NAME "French Coconut Gain")

# Paths
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

juce_add_plugin(
        ${PROJECT_NAME}
        BUNDLE_ID com.dBobStudio.FrenchCoconutGain
        COMPANY_NAME ${COMPANY_NAME}
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        FORMATS ${FORMATS}
        IS_MIDI_EFFECT FALSE
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        PLUGIN_CODE ${PLUGIN_CODE}
        PLUGIN_MANUFACTURER_CODE ${PLUGIN_MANUFACTURER_CODE}
        PRODUCT_NAME ${PLUGIN_FINAL_NAME}
        COPY_PLUGIN_AFTER_BUILD ${COPY_PLUGIN_AFTER_BUILD}
        JUCE_ENABLE_MODULE_SOURCE_GROUPS TRUE
        ICON_BIG "${CMAKE_SOURCE_DIR}/assets/favicon-32x32.png"
        ICON_SMALL "${CMAKE_SOURCE_DIR}/assets/favicon-32x32.png"
        VST3_AUTO_MANIFEST TRUE
)

juce_add_binary_data(BinaryData
        SOURCES
        # Images
        ${CMAKE_SOURCE_DIR}/assets/background.jpg
        ${CMAKE_SOURCE_DIR}/assets/coco.png
        ${CMAKE_SOURCE_DIR}/assets/croissant.png
        ${CMAKE_SOURCE_DIR}/assets/flag.png
        ${CMAKE_SOURCE_DIR}/assets/room-for-crazy-cats.jpg

        # Fonts
        ${CMAKE_SOURCE_DIR}/assets/fonts/JuliaMono-ExtraBoldItalic.ttf
)

target_sources(${PROJECT_NAME}
        PRIVATE
        # .cpp
        ${SOURCE_PATH}/CustomLookAndFeel.cpp
        ${SOURCE_PATH}/PluginEditor.cpp
        ${SOURCE_PATH}/PluginProcessor.cpp
        ${SOURCE_PATH}/CocoKnob.cpp
        ${SOURCE_PATH}/TitleBox.cpp
        ${SOURCE_PATH}/MyCanvas.cpp

        # .h
        ${SOURCE_PATH}/CustomLookAndFeel.h
        ${SOURCE_PATH}/PluginEditor.h
        ${SOURCE_PATH}/PluginProcessor.h
        ${SOURCE_PATH}/CocoKnob.h
        ${SOURCE_PATH}/TitleBox.h
        ${SOURCE_PATH}/MyCanvas.h
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${SOURCE_PATH}
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
message(STATUS "Source path: ${SOURCE_PATH}")

target_compile_definitions(${PROJECT_NAME}
        PUBLIC
        DONT_SET_USING_JUCE_NAMESPACE
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_WEB_BROWSER=0
)
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        BinaryData
        melatonin_inspector

        juce::juce_audio_utils
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

set(LICENSE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE")

if (APPLE)
    if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        add_custom_target(Installer
                COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/sign_and_notarize.sh
                ${CMAKE_BUILD_TYPE}
                "${BUNDLE_ID}"
                "${PLUGIN_FINAL_NAME}"
                "${PROJECT_VERSION}"
                "${PROJECT_NAME}"
                "${LICENSE_FILE}"
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Creating signed and notarized installer for ${PLUGIN_FINAL_NAME}"
                USES_TERMINAL
                VERBATIM
        )

        add_dependencies(Installer ${PROJECT_NAME}_VST3 ${PROJECT_NAME}_AU ${PROJECT_NAME}_Standalone)

        message(STATUS "Code signing integration enabled")
        message(STATUS "Plugin: ${PLUGIN_FINAL_NAME} v${PROJECT_VERSION}")
        message(STATUS "Bundle ID: ${BUNDLE_ID}")
        message(STATUS "To create a signed installer, run: cmake --build . --target Installer")
    else()
        message(STATUS "Installer target only available for Release builds")
        message(STATUS "Current build type: ${CMAKE_BUILD_TYPE}")
    endif()
endif ()

if (WIN32)
    find_program(ISCC_EXECUTABLE
            NAMES ISCC.exe ISCC
            PATHS
            "C:/Program Files (x86)/Inno Setup 6"
            "C:/Program Files/Inno Setup 6"
            "$ENV{LOCALAPPDATA}/Programs/Inno Setup 6"
    )
    if (ISCC_EXECUTABLE)
        set(INSTALLER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/installer_output")
        set(VST3_ARTEFACTS_DIR "${CMAKE_BINARY_DIR}/${PROJECT_NAME}_artefacts/$<CONFIG>/VST3")
        add_custom_target(Installer
                COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALLER_OUTPUT_DIR}"
                COMMAND "${ISCC_EXECUTABLE}"
                "/DMyAppVersion=${PROJECT_VERSION}"
                "/DOutputDir=${INSTALLER_OUTPUT_DIR}"
                "/DPluginSourceDir=${VST3_ARTEFACTS_DIR}"
                "/DLicenseFile=${LICENSE_FILE}"
                "${CMAKE_CURRENT_SOURCE_DIR}/installer/InstallerTemplate.iss"
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Creating Windows installer for ${PLUGIN_FINAL_NAME}"
                USES_TERMINAL
                VERBATIM
        )
        add_dependencies(Installer ${PROJECT_NAME}_VST3 ${PROJECT_NAME}_Standalone)
        message(STATUS "Inno Setup found: ${ISCC_EXECUTABLE}")
        message(STATUS "To create installer, run: cmake --build . --target Installer --config Release")
    else ()
        message(STATUS "Inno Setup not found - Installer target not available")
        message(STATUS "Install from https://jrsoftware.org/isinfo.php")
    endif ()
endif ()
