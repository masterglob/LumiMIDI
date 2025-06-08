cmake_minimum_required(VERSION 3.22)

project(LumiMIDI VERSION 1.0.0)

# Options de build
option(JUCE_BUILD_VST2 "Build VST2 plugin" ON)
option(JUCE_BUILD_VST3 "Build VST3 plugin" ON)
option(JUCE_BUILD_AU "Build AudioUnit plugin" ON)
option(JUCE_BUILD_AAX "Build AAX plugin" ON)
option(JUCE_BUILD_STANDALONE "Build Standalone application" ON)
set(JUCE_USE_FREETYPE OFF)
set(JUCE_WEB_BROWSER_AVAILABLE OFF)
set(JUCE_USE_WEBVIEW OFF)

# Configuration JUCE
add_subdirectory(ThirdParty/JUCE)
include_directories(/usr/include/freetype2)
#juce_set_vst2_sdk_path("ThirdParty/VST")

juce_add_plugin(LumiMIDI
    COMPANY_NAME "CMM_Studios"
    PLUGIN_MANUFACTURER_CODE CMMs
    PLUGIN_CODE LuMi
    FORMATS VST3 Standalone
    ProductName "LumiMIDI"
    PLUGIN_NAME "LumiMIDI"
    PLUGIN_DESCRIPTION "Generate MIDI signals for controlling lighting systems"
    PLUGIN_VERSION "1.0.0"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST2_CATEGORY kPlugCategEffect
    VST3_CATEGORIES Fx
    AU_MAIN_TYPE kAudioUnitType_MIDIProcessor)

# Sources
target_sources(LumiMIDI
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
		
    # Parameters
    Source/Parameters/ParameterIDs.h
    Source/Parameters/ParameterManager.cpp
    Source/Parameters/ParameterManager.h
    
    # DSP
    Source/DSP/AudioEngine.cpp

    # UI Components
    Source/UI/Components/KnobComponent.cpp
    Source/UI/Components/KnobComponent.h
    Source/UI/Components/FilterSection.cpp
    Source/UI/Components/FilterSection.h
    
    # UI LookAndFeel
    Source/UI/LookAndFeel/CustomLookAndFeel.cpp
    Source/UI/LookAndFeel/CustomLookAndFeel.h
    
    # UI Resources
    Source/UI/Resources/ColourPalette.h
	)

# Définitions de compilation
target_compile_definitions(LumiMIDI
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

# Liaison avec les modules JUCE
target_link_libraries(LumiMIDI
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)