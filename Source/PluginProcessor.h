#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "SynthVoice.h"
#include "SynthSound.h"
#include "PostFX.h"
#include "PresetManager.h"

class KeshaZeddSynthAudioProcessor : public juce::AudioProcessor
{
public:
    KeshaZeddSynthAudioProcessor();
    ~KeshaZeddSynthAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Kesha & Zedd Synth"; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return presetManager.getNumPresets(); }
    int getCurrentProgram() override { return currentPresetIndex; }
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override { return presetManager.getPresetName(index); }
    void changeProgramName(int /*index*/, const juce::String& /*newName*/) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    PresetManager& getPresetManager() { return presetManager; }
    int getActiveVoiceCount() const { return activeVoiceCount.load(); }

    void loadPreset(int index);
    void randomizeParameters();

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts;
    juce::Synthesiser synth;
    PostFX postFX;
    PresetManager presetManager;

    std::atomic<int> activeVoiceCount { 0 };
    int currentPresetIndex = 0;

    // Legato note tracker stack
    std::vector<int> heldNotes;
    int lastMidiNoteNumber = -1;
    int activeLegatoNote = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeshaZeddSynthAudioProcessor)
};
