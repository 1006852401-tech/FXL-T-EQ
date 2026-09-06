cmake_minimum_required(VERSION 3.22)

# 强制设置最低兼容系统为 macOS 11.0 (完美兼容所有 M1 芯片电脑与系统)
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum OS X version")
endif()

project(SeesawTiltEQ VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to use local JUCE or auto-fetch JUCE 8 from GitHub
find_package(JUCE CONFIG QUIET)

if(NOT JUCE_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 8.0.4
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(JUCE)
endif()

# Disable automatic copying to system folders when running on CI
if(DEFINED ENV{CI} OR DEFINED ENV{GITHUB_ACTIONS})
    set(JUCE_COPY_PLUGIN FALSE)
else()
    set(JUCE_COPY_PLUGIN TRUE)
endif()

juce_add_plugin(SeesawTiltEQ
    COMPANY_NAME "Equilibrium Audio"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD ${JUCE_COPY_PLUGIN}
    PLUGIN_MANUFACTURER_CODE EqAu
    PLUGIN_CODE StEq
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "Seesaw Tilt EQ"
)

target_sources(SeesawTiltEQ
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
)

target_compile_definitions(SeesawTiltEQ
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(SeesawTiltEQ
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)
