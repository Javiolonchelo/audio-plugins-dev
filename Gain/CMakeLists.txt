# dBob Studio 2024 - Lissajous Plugin Authors: Hao Feng Chen Fu Javier Rodrigo
# López

cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)
project(Gain VERSION 0.0.1)

set(BAD_INTERNET_CONNECTION ON)

if(WIN32)
    set(CMAKE_CXX_COMPILER "C:/Program Files/LLVM/bin/clang++.exe")
    set(CMAKE_GENERATOR "Visual Studio 17 2022")
elseif(LINUX)
    set(CMAKE_CXX_COMPILER "/usr/local/bin/clang++")
    set(CMAKE_GENERATOR "Unix Makefiles")
endif()

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
if(BAD_INTERNET_CONNECTION)
    # FetchContent_Declare(
    #     Catch2
    #     SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/offline/Catch2/"
    # )
    FetchContent_Declare(
        JUCE
        SOURCE_DIR "$ENV{JUCE_PATH}"
    )
else()
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
endif()
# FetchContent_MakeAvailable(Catch2)
FetchContent_MakeAvailable(JUCE)

# juce_set_vst2_sdk_path(...) juce_set_aax_sdk_path(...)
juce_add_plugin(
    ${PROJECT_NAME}
    COMPANY_NAME
    ${COMPANY_NAME}
    IS_SYNTH
    FALSE
    NEEDS_MIDI_INPUT
    FALSE
    NEEDS_MIDI_OUTPUT
    FALSE
    IS_MIDI_EFFECT
    FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS
    FALSE
    PLUGIN_MANUFACTURER_CODE
    ${PLUGIN_MANUFACTURER_CODE}
    PLUGIN_CODE
    ${PLUGIN_CODE}
    FORMATS
    ${FORMATS}
    PRODUCT_NAME
    ${PLUGIN_FINAL_NAME}
    BUNDLE_ID
    com.dBobStudio.${PLUGIN_FINAL_NAME}
    COPY_PLUGIN_AFTER_BUILD
    TRUE
)
juce_add_binary_data(BinaryData SOURCES
    ${CMAKE_SOURCE_DIR}/assets/lmroman12-regular.otf
    )
juce_generate_juce_header(${PROJECT_NAME})

target_sources(
    ${PROJECT_NAME}
    PRIVATE
        # .cpp
        ${SOURCE_PATH}/CustomLookAndFeel.cpp
        ${SOURCE_PATH}/PluginEditor.cpp
        ${SOURCE_PATH}/PluginProcessor.cpp
        # .h
        ${SOURCE_PATH}/CustomLookAndFeel.h
        ${SOURCE_PATH}/PluginEditor.h
        ${SOURCE_PATH}/PluginProcessor.h
        # $<TARGET_PROPERTY:${PROJECT_NAME},JUCE_GENERATED_SOURCES_DIRECTORY>/JuceHeader.h
)
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_PATH})
message(STATUS "Source path: ${SOURCE_PATH}")
target_compile_definitions(
    ${PROJECT_NAME}
    PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0
           DONT_SET_USING_JUCE_NAMESPACE
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
