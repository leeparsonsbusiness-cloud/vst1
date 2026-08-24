#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "SynthSound.h"
#include "CurveEnvelope.h"

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
    void setHostInfo(double bpm);
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void glideTo(float targetFreq, float glideTimeMsVal);
    void setPitchBend(float semitones);
    void setPitchBendRange(float rangeSemitones) { pitchBendRange = rangeSemitones; }
    void setPortamento(int mode, float timeMs) { glideMode = mode; glideTimeMs = timeMs; }
    float getTargetFrequency() const { return targetFrequency; }
    float getCurrentFrequency() const { return currentFrequency; }

private:
    struct Biquad
    {
        float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f, a1 = 0.0f, a2 = 0.0f;
        float x1 = 0.0f, x2 = 0.0f, y1 = 0.0f, y2 = 0.0f;
        void reset() { x1 = x2 = y1 = y2 = 0.0f; }
        
        float process(float in)
        {
            float out = b0 * in + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
            x2 = x1; x1 = in;
            y2 = y1; y1 = out;
            return out;
        }
        
        void setNotch(float sRate, float frequency, float Q)
        {
            float w0 = 2.0f * juce::MathConstants<float>::twoPi * frequency / sRate;
            float alpha = std::sin(w0) / (2.0f * Q);
            float cosw0 = std::cos(w0);
            
            float a0 = 1.0f + alpha;
            b0 = 1.0f / a0;
            b1 = (-2.0f * cosw0) / a0;
            b2 = 1.0f / a0;
            a1 = (-2.0f * cosw0) / a0;
            a2 = (1.0f - alpha) / a0;
        }
    };

    float generateMorphedSample(float shape, float phase);
    float generateLfoSample(int waveType, float phase, float& lastRandVal, bool phaseWrapped);

    double currentSampleRate = 44100.0;
    juce::Random random;

    // 7 Unison sub-oscillators for Osc 1
    float osc1Phases[7] = { 0.0f };

    // Single oscillator phase for Osc 2
    float osc2Phase = 0.0f;

    // Custom Curve Envelopes
    CurveEnvelope ampEnv;
    CurveEnvelope filterEnv;

    // JUCE DSP Ladder Filter
    juce::dsp::LadderFilter<float> filter;
    
    // Custom Notch filter per voice
    Biquad notchFilterL, notchFilterR;

    // Synth parameters cached from APVTS
    float osc1Shape = 2.0f; // default Saw (2.0)
    int osc1Octave = 0;
    float osc1DetuneCents = 0.0f;
    int unisonCount = 7;
    float unisonDetuneCents = 15.0f;
    float osc1Level = 0.8f;

    float osc2Shape = 2.0f; // default Saw
    int osc2Octave = -1;
    float osc2DetuneCents = 5.0f;
    float osc2Level = 0.5f;
    float fmDepth = 0.0f;
    bool oscSyncActive = false;

    float baseCutoffHz = 2000.0f;
    float filterResonance = 0.3f;
    float filterDrive = 1.0f;
    float filterEnvAmount = 0.5f;
    float filterKeyTrack = 0.0f;
    float filterLfoModAmount = 0.0f;
    int filterMode = 0; // LPF12, LPF24, BPF, HPF, Notch, Formant

    int midiNote = 60;
    float noteVelocity = 0.0f;
    bool isPrepared = false;

    // Transient Strike Attack Layer
    float transientTime = 0.0f;
    float transientDecay = 0.015f;
    float transientLevel = 0.0f;
    int transientType = 0;
    float transientPhase = 0.0f;
    float lastTransientSample = 0.0f;
    bool transientActive = false;

    // Phase-Locked Mono Sub Anchor & Sub Drive
    float subPhase = 0.0f;
    float subLevel = 0.0f;
    int subOctave = -1;
    int subWave = 0;
    float subDrive = 0.0f;

    // Pitch Drop Sweep
    bool pitchDropActive = false;
    float pitchDropOctaves = 2.0f;
    float pitchDropTime = 0.0f;
    float pitchDropDuration = 0.05f;

    // Smooth Portamento & Legato Pitch Slide
    int glideMode = 0; // 0: Auto/Legato, 1: Always, 2: Off
    float glideTimeMs = 80.0f;
    float currentFrequency = 440.0f;
    float targetFrequency = 440.0f;
    float lastPlayedFrequency = 440.0f;
    float pitchBendRange = 2.0f;
    float pitchWheelSemitones = 0.0f;

    // Dual Synced LFO variables
    float lfo1Phase = 0.0f;
    float lfo2Phase = 0.0f;
    int lfo1Rate = 2;
    int lfo1Wave = 0;
    float lfo1ToCutoff = 0.0f;
    float lfo1ToShape = 0.0f;
    int lfo2Rate = 3;
    int lfo2Wave = 0;
    float lfo2ToPitch = 0.0f;
    float lfo2ToPan = 0.0f;
    float lastRandomVal1 = 0.0f;
    float lastRandomVal2 = 0.0f;

    // Envelope Curve variables
    float ampDecayCurve = 1.0f;
    float filterDecayCurve = 1.0f;

    // Host Transport Info
    double hostBpm = 120.0;

    // Vowel Formant Filter morph and parallel SVFs
    float formantMorph = 0.0f;
    juce::dsp::StateVariableTPTFilter<float> formantF1L, formantF1R;
    juce::dsp::StateVariableTPTFilter<float> formantF2L, formantF2R;
    juce::dsp::StateVariableTPTFilter<float> formantF3L, formantF3R;
};
