#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include <algorithm>

class RiserEngine
{
public:
    RiserEngine() = default;
    ~RiserEngine() = default;

    void prepare(double sRate)
    {
        sampleRate = sRate;
        hpfL.reset(); hpfR.reset();
    }

    float getPitchOffsetSemitones(float riserProgress) const
    {
        if (riserProgress <= 0.001f) return 0.0f;
        // Exponential pitch rise up to +24 semitones (+2 octaves)
        return std::pow(riserProgress, 1.5f) * 24.0f;
    }

    void process(juce::AudioBuffer<float>& buffer, bool active, float progress, double bpm)
    {
        if (!active && currentProgress <= 0.001f)
            return;

        int numSamples = buffer.getNumSamples();
        int numChannels = buffer.getNumChannels();
        if (numChannels == 0) return;

        float* left = buffer.getWritePointer(0);
        float* right = (numChannels > 1) ? buffer.getWritePointer(1) : left;

        // Smoothly interpolate progress
        float targetProgress = active ? juce::jlimit(0.0f, 1.0f, progress) : 0.0f;
        float progStep = (targetProgress - currentProgress) / static_cast<float>(numSamples);

        double secondsPerBeat = 60.0 / std::max(20.0, bpm);

        for (int i = 0; i < numSamples; ++i)
        {
            currentProgress += progStep;
            currentProgress = juce::jlimit(0.0f, 1.0f, currentProgress);

            if (currentProgress <= 0.001f) continue;

            // 1. Highpass Filter Sweep (20 Hz -> 3500 Hz)
            float hpfFreq = 20.0f + std::pow(currentProgress, 2.0f) * 3500.0f;
            float dt = 1.0f / static_cast<float>(sampleRate);
            float rc = 1.0f / (juce::MathConstants<float>::twoPi * hpfFreq);
            float alpha = rc / (rc + dt);

            float inL = left[i];
            float inR = right[i];
            float hpL = alpha * (lastHpL + inL - lastInL);
            float hpR = alpha * (lastHpR + inR - lastInR);
            lastInL = inL; lastInR = inR;
            lastHpL = hpL; lastHpR = hpR;

            // 2. White Noise Swell
            float noise = (random.nextFloat() * 2.0f - 1.0f) * (std::pow(currentProgress, 2.5f) * 0.35f);

            // 3. Accelerating Stutter Gate (1/4 -> 1/8 -> 1/16 -> 1/32 notes)
            float stutterDiv = 1.0f;
            if (currentProgress < 0.25f) stutterDiv = 1.0f;        // 1/4 note
            else if (currentProgress < 0.50f) stutterDiv = 0.5f;   // 1/8 note
            else if (currentProgress < 0.75f) stutterDiv = 0.25f;  // 1/16 note
            else stutterDiv = 0.125f;                             // 1/32 note

            double stutterPeriodSamples = (secondsPerBeat * stutterDiv) * sampleRate;
            if (stutterPeriodSamples < 10.0) stutterPeriodSamples = 100.0;
            
            float stutterPhase = static_cast<float>(std::fmod(stutterSampleCount, stutterPeriodSamples) / stutterPeriodSamples);
            float gate = (stutterPhase < 0.65f) ? 1.0f : (1.0f - (currentProgress * 0.85f));
            stutterSampleCount++;

            left[i] = (hpL + noise) * gate;
            right[i] = (hpR + noise) * gate;
        }
    }

private:
    double sampleRate = 44100.0;
    float currentProgress = 0.0f;
    double stutterSampleCount = 0.0;
    juce::Random random;

    float lastInL = 0.0f, lastInR = 0.0f;
    float lastHpL = 0.0f, lastHpR = 0.0f;

    juce::dsp::IIR::Filter<float> hpfL, hpfR;
};
