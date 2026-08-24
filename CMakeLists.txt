cmake_minimum_required(VERSION 3.22)
project(KeshaZeddSynth VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(JUCE)

juce_add_plugin(KeshaZeddSynth
    COMPANY_NAME "ElectroDSP"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    PLUGIN_MANUFACTURER_CODE Kesz
    PLUGIN_CODE Kzs1
    FORMATS VST3 AU
    PRODUCT_NAME "KeshaZeddSynth"
    COPY_PLUGIN_AFTER_BUILD TRUE
)

juce_generate_juce_header(KeshaZeddSynth)

target_sources(KeshaZeddSynth
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/SynthVoice.cpp
        Source/PostFX.cpp
        Source/PresetManager.cpp
        Source/AutoMaster.cpp
        Source/ExpandedFX.cpp
)

target_include_directories(KeshaZeddSynth
    PRIVATE
        Source
)

target_link_libraries(KeshaZeddSynth
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_audio_plugin_client
        juce::juce_dsp
        juce::juce_gui_extra
        juce::juce_gui_basics
        juce::juce_graphics
        juce::juce_data_structures
        juce::juce_events
        juce::juce_core
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)


