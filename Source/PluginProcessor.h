#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "SynthVoice.h"
#include "SynthSound.h"
#include "PostFX.h"
#include "PresetManager.h"
#include "ZeddifyEngine.h"
#include "AutoMaster.h"
#include "Arpeggiator.h"
#include "VisualizerComponent.h"
#include "SampleLayer.h"
#include "ModulationMatrix.h"
#include "ExpandedFX.h"
#include "ProducerFlavor.h"
#include "RiserEngine.h"
#include "ScaleManager.h"

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
    ZeddifyEngine& getZeddifyEngine() { return zeddifyEngine; }
    AutoMaster& getAutoMaster() { return autoMaster; }
    Arpeggiator& getArpeggiator() { return arpeggiator; }
    ModulationMatrix& getModMatrix() { return modMatrix; }
    VisualizerComponent& getVisualizer() { return visualizer; }
    ProducerFlavorProcessor& getProducerFlavor() { return producerFlavor; }
    RiserEngine& getRiserEngine() { return riserEngine; }
    ScaleManager& getScaleManager() { return scaleManager; }
    
    int getActiveVoiceCount() const { return activeVoiceCount.load(); }

    float getOutputLevelL() const { return outputLevelL.load(); }
    float getOutputLevelR() const { return outputLevelR.load(); }

    void loadPreset(int index);
    void randomizeParameters();

private:
    juce::Synthesiser synth;
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    PresetManager presetManager;
    ZeddifyEngine zeddifyEngine;
    AutoMaster autoMaster;
    PostFX postFX;
    Arpeggiator arpeggiator;
    SampleLayer sampleLayer;
    ModulationMatrix modMatrix;
    ExpandedFX expandedFX;
    VisualizerComponent visualizer;
    ProducerFlavorProcessor producerFlavor;
    RiserEngine riserEngine;
    ScaleManager scaleManager;

    int currentPresetIndex = 0;
    std::atomic<int> activeVoiceCount { 0 };

    std::atomic<float> outputLevelL { 0.0f };
    std::atomic<float> outputLevelR { 0.0f };

    std::vector<int> heldNotes;
    int activeLegatoNote = -1;
    int lastMidiNoteNumber = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeshaZeddSynthAudioProcessor)
};
