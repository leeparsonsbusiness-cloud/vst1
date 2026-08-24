#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>
#include <string>

struct PresetData
{
    const char* name;
    
    // Osc 1 & Unison
    float osc1Shape;
    int osc1Octave;
    float osc1Detune;
    float osc1Level;
    int unisonCount;
    float unisonDetune;

    // Osc 2
    float osc2Shape;
    int osc2Octave;
    float osc2Detune;
    float osc2Level;
    float fmDepth;
    int oscSync;

    // Transients
    int transientType;
    float transientLevel;
    float transientDecay;

    // Sub Anchor
    int subWave;
    int subOctave;
    float subLevel;
    float subDrive;

    // Filter
    int filterMode;
    float filterCutoff;
    float filterRes;
    float filterDrive;
    float filterEnvAmt;
    float filterKeyTrack;
    float filterLfoMod;
    float formantMorph;

    // LFOs
    int lfo1Rate;
    int lfo1Wave;
    float lfo1ToCutoff;
    float lfo1ToShape;
    
    int lfo2Rate;
    int lfo2Wave;
    float lfo2ToPitch;
    float lfo2ToPan;

    // Envelopes
    float ampAttack;
    float ampDecay;
    float ampSustain;
    float ampRelease;
    float ampDecayCurve;

    float filterAttack;
    float filterDecay;
    float filterSustain;
    float filterRelease;
    float filterDecayCurve;

    // Legato & Pitch Drop
    int playMode;
    float glideTime;
    int pitchDropActive;
    float pitchDropOctaves;
    float pitchDropTime;

    // Chord
    int chordMode;
    int chordType;

    // Trash vs Gloss
    float trashGlossX;
    float trashGlossY;

    // Mono Maker
    int monoMakerActive;
    float monoMakerFreq;

    // Pumper
    int pumpActive;
    int pumpDivision;
    float pumpDepth;
    float pumpCurve;

    // FX
    float fxDrive;
    float fxChorusRate;
    float fxChorusDepth;
    float fxChorusMix;
    float fxDelayTime;
    float fxDelayFeedback;
    float fxDelayMix;
    float fxDelayColor;
    float fxReverbDecay;
    float fxReverbDamping;
    float fxReverbWidth;
    float fxReverbMix;
    
    // Macros
    float macroDrop;
    float macroPunch;
    float macroGrit;
    float macroSpace;
    float macroWidth;
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

    void setAPVTS(juce::AudioProcessorValueTreeState* apvtsToUse) { presetAPVTS = apvtsToUse; }
    void saveUserPreset(const juce::String& category, const juce::String& name);
    bool loadUserPreset(const juce::File& file);

private:
    juce::AudioProcessorValueTreeState* presetAPVTS = nullptr;
    std::vector<PresetData> presets;
    std::vector<std::string> presetNames;
};
