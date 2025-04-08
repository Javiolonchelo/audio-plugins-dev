# dBob Studio 2024 - Lissajous Plugin Authors: Hao Feng Chen Fu Javier Rodrigo
# López

cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)
project(Gain VERSION 0.0.2 LANGUAGES CXX)

set(BAD_INTERNET_CONNECTION ON)

# Customize the plugin here
set(COMPANY_NAME "dBob Studio")
set(PLUGIN_MANUFACTURER_CODE dBob)
set(PLUGIN_CODE Gain)
set(FORMATS AU VST3 Standalone)
set(PLUGIN_FINAL_NAME Gain)

# Paths
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Fetch the libraries
include(FetchContent)
if (BAD_INTERNET_CONNECTION)
    # FetchContent_Declare(
    #     Catch2
    #     SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/offline/Catch2/"
    # )
    FetchContent_Declare(
            JUCE
            SOURCE_DIR "$ENV{JUCE_PATH}"
    )
else ()
    # FetchContent_Declare(
    #     Catch2
    #     GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    #     GIT_TAG v3.7.1
    # )
    FetchContent_Declare(
            JUCE
            GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
            GIT_TAG 8.0.4
    )
endif ()
# FetchContent_MakeAvailable(Catch2)
FetchContent_MakeAvailable(JUCE)

# juce_set_vst2_sdk_path(...) juce_set_aax_sdk_path(...)
juce_add_plugin(
        ${PROJECT_NAME}
        BUNDLE_ID com.dBobStudio.${PLUGIN_FINAL_NAME}
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

juce_add_binary_data(BinaryData SOURCES
        # Images
        ${CMAKE_SOURCE_DIR}/assets/background.jpg
        ${CMAKE_SOURCE_DIR}/assets/coco.png

        # Fonts
        ${CMAKE_SOURCE_DIR}/assets/lmroman12-regular.otf
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
