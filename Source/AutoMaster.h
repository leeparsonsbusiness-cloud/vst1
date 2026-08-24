#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <algorithm>
#include <cmath>

class AutoMaster
{
public:
    AutoMaster();
    ~AutoMaster() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    void process(juce::AudioBuffer<float>& buffer, bool active, float intensity);

private:
    struct BandCompressor
    {
        float thresholdDB = -18.0f;
        float ratio = 3.0f;
        float env = 0.0f;

        float processSample(float in, float sRate)
        {
            float absVal = std::abs(in);
            float alpha = std::exp(-1.0f / (0.010f * sRate)); // 10ms attack/release
            env = env * alpha + absVal * (1.0f - alpha);

            float envDB = juce::Decibels::gainToDecibels(env + 1e-6f);
            if (envDB > thresholdDB)
            {
                float excess = envDB - thresholdDB;
                float compressedDB = thresholdDB + excess / ratio;
                float gainReduce = juce::Decibels::decibelsToGain(compressedDB - envDB);
                return in * gainReduce;
            }
            else
            {
                // Upward expansion for low levels
                float deficit = thresholdDB - envDB;
                if (deficit < 12.0f)
                {
                    float boostDB = deficit * 0.2f;
                    return in * juce::Decibels::decibelsToGain(boostDB);
                }
            }
            return in;
        }
    };

    struct FirstOrderFilter
    {
        float y1 = 0.0f;
        void reset() { y1 = 0.0f; }

        float processLP(float in, float alpha)
        {
            float out = in * (1.0f - alpha) + y1 * alpha;
            y1 = out;
            return out;
        }
    };

    struct SubLockHPF
    {
        float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f, a1 = 0.0f, a2 = 0.0f;
        float x1 = 0.0f, x2 = 0.0f, y1 = 0.0f, y2 = 0.0f;
        void reset() { x1 = x2 = y1 = y2 = 0.0f; }

        void setHPF(float sRate, float frequency)
        {
            float w0 = 2.0f * juce::MathConstants<float>::pi * frequency / sRate;
            float cosw0 = std::cos(w0);
            float alpha = std::sin(w0) / (2.0f * 0.707f);

            float a0 = 1.0f + alpha;
            b0 = ((1.0f + cosw0) / 2.0f) / a0;
            b1 = -(1.0f + cosw0) / a0;
            b2 = ((1.0f + cosw0) / 2.0f) / a0;
            a1 = (-2.0f * cosw0) / a0;
            a2 = (1.0f - alpha) / a0;
        }

        float process(float in)
        {
            float out = b0 * in + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
            x2 = x1; x1 = in;
            y2 = y1; y1 = out;
            return out;
        }
    };

    double sampleRate = 44100.0;
    float smoothedGain = 1.0f;

    // 3-Band Crossover Filters
    FirstOrderFilter lowLP_L, lowLP_R;
    FirstOrderFilter highLP_L, highLP_R;

    // Band Compressors (Low, Mid, High for L & R)
    BandCompressor compLowL, compLowR;
    BandCompressor compMidL, compMidR;
    BandCompressor compHighL, compHighR;

    // Sub Lock
    SubLockHPF subLockHPF;

    // 4x Oversampling engine for anti-aliased soft clipping
    juce::dsp::Oversampling<float> oversampling;
};
