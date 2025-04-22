# dBob Studio 2025
# Javier Rodrigo López
# javiolonchelo@gmail.com

cmake_minimum_required(VERSION 3.15)
project(Gain VERSION 0.2.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

# Fetch JUCE
include(FetchContent)
if (DEFINED ENV{JUCE_PATH} AND NOT "$ENV{JUCE_PATH}" STREQUAL "")
    FetchContent_Declare(JUCE
            SOURCE_DIR "$ENV{JUCE_PATH}")
else ()
    FetchContent_Declare(JUCE
            GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
            GIT_TAG 8.0.7
    )
endif ()
FetchContent_MakeAvailable(JUCE)

# Documentation
find_package(Doxygen REQUIRED dot)

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

doxygen_add_docs(doc "src/" ALL
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Creating HTML documentation...")

set(COMPANY_NAME "dBob Studio")
set(PLUGIN_MANUFACTURER_CODE dBob)

# Customize the plugin here
set(BUNDLE_ID com.dBobStudio.FrenchCoconutGain)
set(PLUGIN_CODE Frgn)
set(FORMATS AU VST3 Standalone)
set(PLUGIN_FINAL_NAME "French Coconut Gain")

# Paths
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

# juce_set_aax_sdk_path(...)
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
        COPY_PLUGIN_AFTER_BUILD TRUE
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
        # .h
        ${SOURCE_PATH}/CustomLookAndFeel.h
        ${SOURCE_PATH}/PluginEditor.h
        ${SOURCE_PATH}/PluginProcessor.h
        ${SOURCE_PATH}/CocoKnob.h
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${SOURCE_PATH}
)
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

        juce::juce_audio_utils
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
