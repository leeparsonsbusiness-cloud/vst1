#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <random>
#include <cmath>

class SampleLayer
{
public:
    enum NoiseType { WhiteNoise = 0, PinkNoise, VinylCrackle, VocalSnap, CustomWav };

    SampleLayer()
    {
        formatManager.registerBasicFormats();
    }

    ~SampleLayer() = default;

    void setSampleRate(double sRate) { sampleRate = sRate; }

    bool loadWavFile(const juce::File& file)
    {
        if (!file.existsAsFile())
            return false;

        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
        if (reader != nullptr)
        {
            customBuffer.setSize(static_cast<int>(reader->numChannels), static_cast<int>(reader->lengthInSamples));
            reader->read(&customBuffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
            hasCustomSample = true;
            return true;
        }
        return false;
    }

    void trigger(float vel)
    {
        samplePosition = 0;
        isTriggered = true;
        velocity = vel;
        b0 = b1 = b2 = b3 = b4 = b5 = b6 = 0.0f;
    }

    float getNextSample(int type, float level, float decayTimeMs)
    {
        if (!isTriggered || level <= 0.001f)
            return 0.0f;

        float dt = 1.0f / static_cast<float>(sampleRate);
        float envDecay = std::exp(-dt / (std::max(0.005f, decayTimeMs * 0.001f)));
        decayEnv *= envDecay;

        float rawSample = 0.0f;

        if (type == CustomWav && hasCustomSample && customBuffer.getNumSamples() > 0)
        {
            if (samplePosition < customBuffer.getNumSamples())
            {
                rawSample = customBuffer.getSample(0, samplePosition);
                samplePosition++;
            }
            else
            {
                isTriggered = false;
            }
        }
        else if (type == WhiteNoise)
        {
            rawSample = (randDist(gen) * 2.0f - 1.0f);
        }
        else if (type == PinkNoise)
        {
            float white = (randDist(gen) * 2.0f - 1.0f);
            b0 = 0.99886f * b0 + white * 0.0555179f;
            b1 = 0.99332f * b1 + white * 0.0750759f;
            b2 = 0.96900f * b2 + white * 0.1538520f;
            b3 = 0.86650f * b3 + white * 0.3104856f;
            b4 = 0.55000f * b4 + white * 0.5329522f;
            b5 = -0.7616f * b5 - white * 0.0168980f;
            rawSample = (b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362f) * 0.11f;
            b6 = white * 0.115926f;
        }
        else if (type == VinylCrackle)
        {
            float white = (randDist(gen) * 2.0f - 1.0f);
            if (randDist(gen) > 0.985f)
                rawSample = white * 2.5f;
            else
                rawSample = white * 0.05f;
        }
        else if (type == VocalSnap)
        {
            float freq = 800.0f + std::sin(samplePosition * 0.05f) * 400.0f;
            rawSample = std::sin(samplePosition * 2.0f * juce::MathConstants<float>::pi * freq / static_cast<float>(sampleRate));
            samplePosition++;
            if (samplePosition > sampleRate * 0.04) isTriggered = false;
        }

        return rawSample * level * velocity;
    }

private:
    double sampleRate = 44100.0;
    juce::AudioFormatManager formatManager;
    juce::AudioBuffer<float> customBuffer;
    bool hasCustomSample = false;

    bool isTriggered = false;
    int samplePosition = 0;
    float velocity = 1.0f;
    float decayEnv = 1.0f;

    // Pink noise filter state
    float b0 = 0, b1 = 0, b2 = 0, b3 = 0, b4 = 0, b5 = 0, b6 = 0;
    std::mt19937 gen { 1337 };
    std::uniform_real_distribution<float> randDist { 0.0f, 1.0f };
};
