cmake_minimum_required(VERSION 3.15)
project(COMPRESSOR VERSION 0.0.0)

add_subdirectory(JUCE)

juce_add_plugin(Compressor
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    PLUGIN_MANUFACTURER_CODE MaSm
    PLUGIN_CODE Comp
    FORMATS VST2 VST3 Standalone
    PRODUCT_NAME "Compressor")

target_sources(Compressor
    PRIVATE
        Source/Compressor.cpp
        Source/PluginEditor.cpp
        Source/PluginProcessor.cpp)

target_compile_definitions(Compressor
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(Compressor
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
