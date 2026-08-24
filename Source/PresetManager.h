#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>
#include <string>

struct PresetData
{
    const char* name;
    int osc1Wave;
    int osc1Octave;
    float osc1Detune;
    int unisonCount;
    float unisonDetune;
    float osc1Level;

    int osc2Wave;
    int osc2Octave;
    float osc2Detune;
    float osc2Level;

    float ampAttack;
    float ampDecay;
    float ampSustain;
    float ampRelease;

    float filterCutoff;
    float filterRes;
    int filterMode;
    float filterEnvAmt;
    float filterAttack;
    float filterDecay;
    float filterSustain;
    float filterRelease;

    float fxDrive;
    float fxChorusRate;
    float fxChorusDepth;
    float fxChorusMix;
    float fxDelayTime;
    float fxDelayFeedback;
    float fxDelayMix;
};

class PresetManager
{
public:
    PresetManager();
    ~PresetManager() = default;

    int getNumPresets() const;
    juce::String getPresetName(int index) const;
    const std::vector<std::string>& getPresetNames() const;

    void applyPreset(int index, juce::AudioProcessorValueTreeState& apvts);

    void setAPVTS(juce::AudioProcessorValueTreeState* apvtsToUse) { apvts = apvtsToUse; }
    void saveUserPreset(const juce::String& name);
    bool loadUserPreset(const juce::String& name);

private:
    juce::AudioProcessorValueTreeState* apvts = nullptr;
    std::vector<PresetData> presets;
    std::vector<std::string> presetNames;
};
