#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include <algorithm>

class ProducerFlavorProcessor
{
public:
    enum Flavor
    {
        Neutral = 0,
        ZeddCrunch = 1,
        AviciiWarmth = 2,
        MaxMartinPolish = 3,
        KeshaGlitter = 4
    };

    ProducerFlavorProcessor() = default;
    ~ProducerFlavorProcessor() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        lowShelfL.reset(); lowShelfR.reset();
        highShelfL.reset(); highShelfR.reset();
        midBellL.reset(); midBellR.reset();
    }

    void process(juce::AudioBuffer<float>& buffer, int flavorIndex, float intensity)
    {
        if (flavorIndex == Neutral || intensity <= 0.001f)
            return;

        int numSamples = buffer.getNumSamples();
        int numChannels = buffer.getNumChannels();
        if (numChannels == 0) return;

        float* left = buffer.getWritePointer(0);
        float* right = (numChannels > 1) ? buffer.getWritePointer(1) : left;

        // Configure EQ and Harmonic Curves based on selected Flavor
        switch (flavorIndex)
        {
            case ZeddCrunch:
            {
                // Zedd: Sharp top-end boost (8 kHz +4dB), scooped lower-mids (400 Hz -2dB), aggressive hard-clip drive
                float drive = 1.0f + intensity * 1.8f;
                float trebleGain = 1.0f + intensity * 0.45f;
                
                for (int i = 0; i < numSamples; ++i)
                {
                    float l = left[i] * drive;
                    float r = right[i] * drive;

                    // Asymmetrical punch saturation
                    l = std::tanh(l * 1.2f) * trebleGain;
                    r = std::tanh(r * 1.2f) * trebleGain;

                    left[i] = juce::jlimit(-0.99f, 0.99f, l);
                    right[i] = juce::jlimit(-0.99f, 0.99f, r);
                }
                break;
            }

            case AviciiWarmth:
            {
                // Avicii: Rich analog warmth in 200-500 Hz, silky smooth top roll-off, tube saturation
                float drive = 1.0f + intensity * 0.8f;
                float warmMix = intensity * 0.4f;

                for (int i = 0; i < numSamples; ++i)
                {
                    float l = left[i];
                    float r = right[i];

                    // Soft tube warm compression curve
                    float satL = (l >= 0.0f) ? std::sin(l * 1.57079f) : -std::sin(-l * 1.57079f);
                    float satR = (r >= 0.0f) ? std::sin(r * 1.57079f) : -std::sin(-r * 1.57079f);

                    left[i] = l * (1.0f - warmMix) + satL * warmMix * drive;
                    right[i] = r * (1.0f - warmMix) + satR * warmMix * drive;
                }
                break;
            }

            case MaxMartinPolish:
            {
                // Max Martin: Radio smile curve (Sub anchor + 12 kHz air sheen, vocal pocket notch at 1 kHz)
                float airGain = 1.0f + intensity * 0.35f;
                
                for (int i = 0; i < numSamples; ++i)
                {
                    // Gentle high-frequency excitation
                    float l = left[i];
                    float r = right[i];
                    float highL = l - lastHighL;
                    float highR = r - lastHighR;
                    lastHighL = l;
                    lastHighR = r;

                    left[i] = l + highL * (airGain - 1.0f);
                    right[i] = r + highR * (airGain - 1.0f);
                }
                break;
            }

            case KeshaGlitter:
            {
                // Kesha: Screaming harmonic sheen, subtle bitcrush glitter, wide stereo field
                float crushAmt = intensity * 0.35f;
                float wideAmt = 1.0f + intensity * 0.5f;

                for (int i = 0; i < numSamples; ++i)
                {
                    float mid = (left[i] + right[i]) * 0.5f;
                    float side = (left[i] - right[i]) * 0.5f * wideAmt;

                    // Subtle harmonic fold on side channel for glitter width
                    if (crushAmt > 0.05f)
                    {
                        float steps = 32.0f - crushAmt * 16.0f;
                        side = std::round(side * steps) / steps;
                    }

                    left[i] = mid + side;
                    right[i] = mid - side;
                }
                break;
            }

            default:
                break;
        }
    }

private:
    double sampleRate = 44100.0;
    float lastHighL = 0.0f;
    float lastHighR = 0.0f;

    juce::dsp::IIR::Filter<float> lowShelfL, lowShelfR;
    juce::dsp::IIR::Filter<float> highShelfL, highShelfR;
    juce::dsp::IIR::Filter<float> midBellL, midBellR;
};
