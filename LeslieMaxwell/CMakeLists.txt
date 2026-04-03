# dBob Studio 2025 - Javier Rodrigo López
# javiolonchelo@gmail.com

cmake_minimum_required(VERSION 3.15)
project(LeslieMaxwell VERSION 1.0.0 LANGUAGES C CXX)

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

# Melatonin Inspector (externally cloned)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ext/melatonin_inspector ${CMAKE_CURRENT_BINARY_DIR}/melatonin_inspector)

# Melatonin Blur (externally cloned)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ext/melatonin_blur ${CMAKE_CURRENT_BINARY_DIR}/melatonin_blur)

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

set(BUNDLE_ID com.dBobStudio.LeslieMaxwell)
set(PLUGIN_CODE Lsmx)
set(FORMATS AU VST3 Standalone)
set(PLUGIN_FINAL_NAME "Leslie Maxwell")
set(COMPANY_NAME "dBob Studio")
set(PLUGIN_MANUFACTURER_CODE dBob)

# Paths
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

# juce_set_vst2_sdk_path(...) juce_set_aax_sdk_path(...)
juce_add_plugin(
        ${PROJECT_NAME}
        BUNDLE_ID ${BUNDLE_ID}
        COMPANY_NAME ${COMPANY_NAME}
        COPY_PLUGIN_AFTER_BUILD TRUE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        FORMATS ${FORMATS}
        IS_MIDI_EFFECT FALSE
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        PLUGIN_CODE ${PLUGIN_CODE}
        PLUGIN_MANUFACTURER_CODE ${PLUGIN_MANUFACTURER_CODE}
        PRODUCT_NAME ${PLUGIN_FINAL_NAME}
)

# JPEG filmstrips (split due to JPEG 65535px height limit)
juce_add_binary_data(BinaryData
        SOURCES
        "${CMAKE_SOURCE_DIR}/assets/maxwell_strip_0.jpg"
        "${CMAKE_SOURCE_DIR}/assets/maxwell_strip_1.jpg"
        "${CMAKE_SOURCE_DIR}/assets/top.png"
        "${CMAKE_SOURCE_DIR}/assets/rev_on.png"
        "${CMAKE_SOURCE_DIR}/assets/rev_off.png"
        "${CMAKE_SOURCE_DIR}/assets/rev_hold.png"
        "${CMAKE_SOURCE_DIR}/assets/sync_on.png"
        "${CMAKE_SOURCE_DIR}/assets/sync_off.png"
        "${CMAKE_SOURCE_DIR}/assets/sync_hold.png"
        "${CMAKE_SOURCE_DIR}/assets/fonts/HellasFunBold.ttf"
)

target_sources(${PROJECT_NAME}
        PRIVATE
        # .cpp
        ${SOURCE_PATH}/CustomLookAndFeel.cpp
        ${SOURCE_PATH}/PluginEditor.cpp
        ${SOURCE_PATH}/PluginProcessor.cpp
        # .h
        ${SOURCE_PATH}/CustomLookAndFeel.h
        ${SOURCE_PATH}/PluginEditor.h
        ${SOURCE_PATH}/PluginProcessor.h
        ${SOURCE_PATH}/FractionalRingBuffer.h
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${SOURCE_PATH}
)
message(STATUS "Source path: ${SOURCE_PATH}")

target_compile_definitions(${PROJECT_NAME}
        PUBLIC
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_WEB_BROWSER=0
)
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        BinaryData
        melatonin_blur
        melatonin_inspector
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_opengl
        juce::juce_gui_basics
        juce::juce_gui_extra
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
