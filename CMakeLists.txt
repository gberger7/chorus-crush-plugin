cmake_minimum_required(VERSION 3.15)

project(ChorusCrush VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

# Add JUCE
add_subdirectory(JUCE)

# Create the plugin target
juce_add_plugin(ChorusCrush
    COMPANY_NAME "YourCompany"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Yrcm
    PLUGIN_CODE Chcr
    FORMATS AU VST3
    PRODUCT_NAME "Chorus Crush")

# Add source files
target_sources(ChorusCrush
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/BitCrusher.cpp
        Source/Chorus.cpp)

# Link JUCE modules
target_link_libraries(ChorusCrush
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_basics
        juce::juce_gui_basics
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# Compile definitions
target_compile_definitions(ChorusCrush
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)