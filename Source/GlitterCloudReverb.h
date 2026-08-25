#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <cmath>
#include <random>

class GlitterCloudReverb
{
public:
    GlitterCloudReverb()
    {
        grainBufferL.resize(8192, 0.0f);
        grainBufferR.resize(8192, 0.0f);
        diffuserL.resize(16384, 0.0f);
        diffuserR.resize(16384, 0.0f);
    }

    ~GlitterCloudReverb() = default;

    void prepare(double sRate)
    {
        sampleRate = sRate;
        grainBufferL.assign(static_cast<size_t>(sampleRate * 0.2), 0.0f);
        grainBufferR.assign(static_cast<size_t>(sampleRate * 0.2), 0.0f);
        diffuserL.assign(static_cast<size_t>(sampleRate * 0.6), 0.0f);
        diffuserR.assign(static_cast<size_t>(sampleRate * 0.6), 0.0f);
        writePos = 0;
        diffWritePos = 0;
    }

    void process(juce::AudioBuffer<float>& buffer, float glitterMix, float grainSizeMs)
    {
        if (glitterMix < 0.001f)
            return;

        const int numSamples = buffer.getNumSamples();
        const int numChannels = buffer.getNumChannels();
        const size_t gBufLen = grainBufferL.size();
        const size_t dBufLen = diffuserL.size();

        if (gBufLen == 0 || dBufLen == 0) return;

        float grainLenSamples = (grainSizeMs * 0.001f) * static_cast<float>(sampleRate);
        if (grainLenSamples < 64.0f) grainLenSamples = 64.0f;

        auto* lOut = buffer.getWritePointer(0);
        auto* rOut = (numChannels > 1) ? buffer.getWritePointer(1) : lOut;

        for (int s = 0; s < numSamples; ++s)
        {
            float inL = lOut[s];
            float inR = rOut[s];

            // 1. Write incoming dry audio to pitch-shift buffer
            grainBufferL[writePos] = inL;
            grainBufferR[writePos] = inR;

            // 2. Dual pitch-shifted grain streams (+12 semitones = 2x speed)
            grainPhase1 += 2.0f;
            if (grainPhase1 >= grainLenSamples) grainPhase1 -= grainLenSamples;

            grainPhase2 += 2.0f;
            if (grainPhase2 >= grainLenSamples) grainPhase2 -= grainLenSamples;

            float w1 = 0.5f * (1.0f - std::cos(2.0f * 3.14159265f * (grainPhase1 / grainLenSamples)));
            float w2 = 0.5f * (1.0f - std::cos(2.0f * 3.14159265f * (grainPhase2 / grainLenSamples)));

            float readPos1 = static_cast<float>(writePos) - grainPhase1;
            while (readPos1 < 0.0f) readPos1 += static_cast<float>(gBufLen);

            float readPos2 = static_cast<float>(writePos) - grainPhase2;
            while (readPos2 < 0.0f) readPos2 += static_cast<float>(gBufLen);

            size_t idx1 = static_cast<size_t>(readPos1) % gBufLen;
            size_t idx2 = static_cast<size_t>(readPos2) % gBufLen;

            float shimmerL = grainBufferL[idx1] * w1 + grainBufferL[idx2] * w2;
            float shimmerR = grainBufferR[idx1] * w1 + grainBufferR[idx2] * w2;

            // 3. Stereo Diffusion & Shimmer Tail
            size_t delayL1 = (diffWritePos + dBufLen - static_cast<size_t>(sampleRate * 0.13)) % dBufLen;
            size_t delayR1 = (diffWritePos + dBufLen - static_cast<size_t>(sampleRate * 0.17)) % dBufLen;
            size_t delayL2 = (diffWritePos + dBufLen - static_cast<size_t>(sampleRate * 0.23)) % dBufLen;
            size_t delayR2 = (diffWritePos + dBufLen - static_cast<size_t>(sampleRate * 0.29)) % dBufLen;

            float wetL = shimmerL * 0.6f + diffuserL[delayL1] * 0.45f + diffuserR[delayR2] * 0.35f;
            float wetR = shimmerR * 0.6f + diffuserR[delayR1] * 0.45f + diffuserL[delayL2] * 0.35f;

            // Damping & feedback
            diffuserL[diffWritePos] = wetL * 0.72f;
            diffuserR[diffWritePos] = wetR * 0.72f;

            // 4. Mix with dry audio
            lOut[s] = inL * (1.0f - glitterMix * 0.3f) + wetL * (glitterMix * 0.75f);
            rOut[s] = inR * (1.0f - glitterMix * 0.3f) + wetR * (glitterMix * 0.75f);

            writePos = (writePos + 1) % gBufLen;
            diffWritePos = (diffWritePos + 1) % dBufLen;
        }
    }

private:
    double sampleRate = 44100.0;
    std::vector<float> grainBufferL;
    std::vector<float> grainBufferR;
    std::vector<float> diffuserL;
    std::vector<float> diffuserR;

    size_t writePos = 0;
    size_t diffWritePos = 0;
    float grainPhase1 = 0.0f;
    float grainPhase2 = 0.0f;
};
