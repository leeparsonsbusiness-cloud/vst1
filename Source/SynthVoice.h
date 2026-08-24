#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "SynthSound.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice();
    ~SynthVoice() override = default;

    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    
    void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels);
    void updateParameters(juce::AudioProcessorValueTreeState& apvts);
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

private:
    float generateSample(int waveType, float phase);

    double currentSampleRate = 44100.0;
    juce::Random random;

    // 7 Unison sub-oscillators for Osc 1
    float osc1Phases[7] = { 0.0f };

    // Single oscillator phase for Osc 2
    float osc2Phase = 0.0f;

    // ADSR Envelopes
    juce::ADSR ampEnv;
    juce::ADSR filterEnv;

    // JUCE DSP Ladder Filter
    juce::dsp::LadderFilter<float> filter;

    // Synth parameters cached from APVTS
    int osc1Wave = 1;         // Saw
    int osc1Octave = 0;
    float osc1DetuneCents = 0.0f;
    int unisonCount = 7;
    float unisonDetuneCents = 15.0f;
    float osc1Level = 0.8f;

    int osc2Wave = 1;         // Saw
    int osc2Octave = -1;
    float osc2DetuneCents = 5.0f;
    float osc2Level = 0.5f;

    float baseCutoffHz = 2000.0f;
    float filterResonance = 0.3f;
    float filterEnvAmount = 0.5f;
    int filterMode = 0;       // LPF12

    float noteFrequency = 440.0f;
    float noteVelocity = 0.0f;
    bool isPrepared = false;

public:
    void glideTo(float targetFreq, float glideTimeMsVal);

private:
    // Transient Click Layer
    float clickTime = 0.0f;
    float clickDecay = 0.015f;
    float clickLevel = 0.0f;
    bool clickActive = false;

    // Phase-Locked Mono Sub Anchor
    float subPhase = 0.0f;
    float subLevel = 0.0f;
    int subOctave = -1;
    int subWave = 0;

    // Pitch Drop Sweep
    bool pitchDropActive = false;
    float pitchDropOctaves = 2.0f;
    float pitchDropTime = 0.0f;
    float pitchDropDuration = 0.05f;

    // Legato Glide
    float currentFrequency = 440.0f;
    float targetFrequency = 440.0f;
    float glideFactor = 0.0f;
    float glideTimeMs = 100.0f;

    // Vowel Formant Filter morph and parallel SVFs
    float formantMorph = 0.0f;
    juce::dsp::StateVariableTPTFilter<float> formantF1L, formantF1R;
    juce::dsp::StateVariableTPTFilter<float> formantF2L, formantF2R;
    juce::dsp::StateVariableTPTFilter<float> formantF3L, formantF3R;
};
