#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <cmath>
#include <algorithm>

class GlitterCloudReverb
{
public:
    GlitterCloudReverb()
    {
        grainBufferL.assign(32768, 0.0f);
        grainBufferR.assign(32768, 0.0f);
        diffuserL.assign(131072, 0.0f);
        diffuserR.assign(131072, 0.0f);
    }

    ~GlitterCloudReverb() = default;

    void prepare(double sRate)
    {
        sampleRate = (sRate > 1000.0) ? sRate : 44100.0;
        int gSize = std::max(16384, static_cast<int>(sampleRate * 0.4));
        int dSize = std::max(65536, static_cast<int>(sampleRate * 1.5));
        grainBufferL.assign(static_cast<size_t>(gSize), 0.0f);
        grainBufferR.assign(static_cast<size_t>(gSize), 0.0f);
        diffuserL.assign(static_cast<size_t>(dSize), 0.0f);
        diffuserR.assign(static_cast<size_t>(dSize), 0.0f);
        writePos = 0;
        diffWritePos = 0;
    }

    void process(juce::AudioBuffer<float>& buffer, float glitterMix, float grainSizeMs)
    {
        if (glitterMix < 0.001f)
            return;

        const int numSamples = buffer.getNumSamples();
        const int numChannels = buffer.getNumChannels();
        const long long gBufLen = static_cast<long long>(grainBufferL.size());
        const long long dBufLen = static_cast<long long>(diffuserL.size());

        if (gBufLen <= 128 || dBufLen <= 128) return;

        double sRate = (sampleRate > 1000.0) ? sampleRate : 44100.0;
        float grainLenSamples = (grainSizeMs * 0.001f) * static_cast<float>(sRate);
        if (grainLenSamples < 64.0f) grainLenSamples = 64.0f;

        auto* lOut = buffer.getWritePointer(0);
        auto* rOut = (numChannels > 1) ? buffer.getWritePointer(1) : lOut;

        for (int s = 0; s < numSamples; ++s)
        {
            float inL = lOut[s];
            float inR = rOut[s];

            // 1. Write incoming dry audio to pitch-shift buffer
            grainBufferL[static_cast<size_t>(writePos % gBufLen)] = inL;
            grainBufferR[static_cast<size_t>(writePos % gBufLen)] = inR;

            // 2. Dual pitch-shifted grain streams (+12 semitones = 2x speed)
            grainPhase1 += 2.0f;
            if (grainPhase1 >= grainLenSamples) grainPhase1 -= grainLenSamples;

            grainPhase2 += 2.0f;
            if (grainPhase2 >= grainLenSamples) grainPhase2 -= grainLenSamples;

            float w1 = 0.5f * (1.0f - std::cos(2.0f * 3.14159265f * (grainPhase1 / grainLenSamples)));
            float w2 = 0.5f * (1.0f - std::cos(2.0f * 3.14159265f * (grainPhase2 / grainLenSamples)));

            long long readPos1 = static_cast<long long>(writePos - static_cast<int>(grainPhase1));
            while (readPos1 < 0) readPos1 += gBufLen;
            long long readPos2 = static_cast<long long>(writePos - static_cast<int>(grainPhase2));
            while (readPos2 < 0) readPos2 += gBufLen;

            size_t idx1 = static_cast<size_t>(readPos1 % gBufLen);
            size_t idx2 = static_cast<size_t>(readPos2 % gBufLen);

            float shimmerL = grainBufferL[idx1] * w1 + grainBufferL[idx2] * w2;
            float shimmerR = grainBufferR[idx1] * w1 + grainBufferR[idx2] * w2;

            // 3. Stereo Diffusion & Shimmer Tail (safe signed offsets)
            long long tap1 = diffWritePos - static_cast<long long>(sRate * 0.07);
            while (tap1 < 0) tap1 += dBufLen;
            long long tap2 = diffWritePos - static_cast<long long>(sRate * 0.11);
            while (tap2 < 0) tap2 += dBufLen;
            long long tap3 = diffWritePos - static_cast<long long>(sRate * 0.17);
            while (tap3 < 0) tap3 += dBufLen;
            long long tap4 = diffWritePos - static_cast<long long>(sRate * 0.23);
            while (tap4 < 0) tap4 += dBufLen;

            size_t delayL1 = static_cast<size_t>(tap1 % dBufLen);
            size_t delayR1 = static_cast<size_t>(tap2 % dBufLen);
            size_t delayL2 = static_cast<size_t>(tap3 % dBufLen);
            size_t delayR2 = static_cast<size_t>(tap4 % dBufLen);

            float wetL = shimmerL * 0.6f + diffuserL[delayL1] * 0.40f + diffuserR[delayR2] * 0.30f;
            float wetR = shimmerR * 0.6f + diffuserR[delayR1] * 0.40f + diffuserL[delayL2] * 0.30f;

            // Damping & feedback
            diffuserL[static_cast<size_t>(diffWritePos % dBufLen)] = wetL * 0.68f;
            diffuserR[static_cast<size_t>(diffWritePos % dBufLen)] = wetR * 0.68f;

            // 4. Mix with dry audio
            lOut[s] = inL * (1.0f - glitterMix * 0.25f) + wetL * (glitterMix * 0.65f);
            rOut[s] = inR * (1.0f - glitterMix * 0.25f) + wetR * (glitterMix * 0.65f);

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

    long long writePos = 0;
    long long diffWritePos = 0;
    float grainPhase1 = 0.0f;
    float grainPhase2 = 0.0f;
};
