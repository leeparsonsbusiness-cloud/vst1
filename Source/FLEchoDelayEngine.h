#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <cmath>
#include <algorithm>

class FLEchoDelayEngine
{
public:
    FLEchoDelayEngine()
    {
        delayBufferL.resize(96000, 0.0f);
        delayBufferR.resize(96000, 0.0f);
    }

    ~FLEchoDelayEngine() = default;

    void prepare(double sRate)
    {
        sampleRate = sRate;
        int bufLen = static_cast<int>(sampleRate * 3.0); // 3 seconds buffer
        delayBufferL.assign(static_cast<size_t>(std::max(bufLen, 8192)), 0.0f);
        delayBufferR.assign(static_cast<size_t>(std::max(bufLen, 8192)), 0.0f);
        writePos = 0;
    }

    void process(juce::AudioBuffer<float>& buffer, float feed, float timeMs, float panSpread,
                 float pitchSemitones, int echoCount, bool pingPong, bool fatMode)
    {
        if (echoCount <= 0 || feed < 0.001f)
            return;

        const int numSamples = buffer.getNumSamples();
        const int numChannels = buffer.getNumChannels();
        const size_t bufLen = delayBufferL.size();
        if (bufLen == 0) return;

        float baseDelaySamples = (timeMs * 0.001f) * static_cast<float>(sampleRate);
        if (baseDelaySamples < 64.0f) baseDelaySamples = 64.0f;

        auto* lOut = buffer.getWritePointer(0);
        auto* rOut = (numChannels > 1) ? buffer.getWritePointer(1) : lOut;

        // Pitch shift factor = 2^(semitones / 12)
        float pitchRatio = std::pow(2.0f, pitchSemitones / 12.0f);

        for (int s = 0; s < numSamples; ++s)
        {
            float inL = lOut[s];
            float inR = rOut[s];

            // Accumulate echo taps
            float echoAccumL = 0.0f;
            float echoAccumR = 0.0f;

            for (int tap = 1; tap <= echoCount; ++tap)
            {
                float tapGain = std::pow(feed, static_cast<float>(tap));
                if (tapGain < 0.005f) break;

                float tapDelay = baseDelaySamples * static_cast<float>(tap);
                
                // Pitch shift modulation
                if (std::abs(pitchSemitones) > 0.05f)
                {
                    tapDelay = tapDelay / std::pow(pitchRatio, static_cast<float>(tap));
                }

                // Ping-pong pan
                float tapPan = (panSpread * static_cast<float>(tap)) / static_cast<float>(echoCount);
                if (pingPong)
                {
                    tapPan = (tap % 2 == 1) ? -std::abs(tapPan) - 0.5f : std::abs(tapPan) + 0.5f;
                }
                tapPan = juce::jlimit(-1.0f, 1.0f, tapPan);

                float panL = std::cos(0.5f * 3.14159265f * (tapPan * 0.5f + 0.5f));
                float panR = std::sin(0.5f * 3.14159265f * (tapPan * 0.5f + 0.5f));

                float rPos = static_cast<float>(writePos) - tapDelay;
                while (rPos < 0.0f) rPos += static_cast<float>(bufLen);

                size_t idx0 = static_cast<size_t>(rPos) % bufLen;
                size_t idx1 = (idx0 + 1) % bufLen;
                float frac = rPos - static_cast<float>(idx0);

                float tapSampL = (1.0f - frac) * delayBufferL[idx0] + frac * delayBufferL[idx1];
                float tapSampR = (1.0f - frac) * delayBufferR[idx0] + frac * delayBufferR[idx1];

                // Fat mode stereo doubling
                if (fatMode)
                {
                    size_t fatIdx = (idx0 + static_cast<size_t>(sampleRate * 0.008)) % bufLen;
                    tapSampL += delayBufferL[fatIdx] * 0.35f;
                    tapSampR += delayBufferR[fatIdx] * 0.35f;
                }

                float monoTap = (tapSampL + tapSampR) * 0.5f * tapGain;
                echoAccumL += monoTap * panL;
                echoAccumR += monoTap * panR;
            }

            // Write current input + slight feedback into buffer
            delayBufferL[writePos] = inL + echoAccumL * 0.3f;
            delayBufferR[writePos] = inR + echoAccumR * 0.3f;

            lOut[s] += echoAccumL;
            rOut[s] += echoAccumR;

            writePos = (writePos + 1) % bufLen;
        }
    }

private:
    double sampleRate = 44100.0;
    std::vector<float> delayBufferL;
    std::vector<float> delayBufferR;
    size_t writePos = 0;
};
