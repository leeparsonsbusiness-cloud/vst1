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
        delayBufferL.assign(131072, 0.0f);
        delayBufferR.assign(131072, 0.0f);
    }

    ~FLEchoDelayEngine() = default;

    void prepare(double sRate)
    {
        sampleRate = (sRate > 1000.0) ? sRate : 44100.0;
        int bufLen = std::max(65536, static_cast<int>(sampleRate * 3.0));
        delayBufferL.assign(static_cast<size_t>(bufLen), 0.0f);
        delayBufferR.assign(static_cast<size_t>(bufLen), 0.0f);
        writePos = 0;
    }

    void process(juce::AudioBuffer<float>& buffer, float feed, float timeMs, float panSpread,
                 float pitchSemitones, int echoCount, bool pingPong, bool fatMode)
    {
        if (echoCount <= 0 || feed < 0.001f)
            return;

        const int numSamples = buffer.getNumSamples();
        const int numChannels = buffer.getNumChannels();
        const long long bufLen = static_cast<long long>(delayBufferL.size());
        if (bufLen <= 512) return;

        double sRate = (sampleRate > 1000.0) ? sampleRate : 44100.0;
        float baseDelaySamples = (timeMs * 0.001f) * static_cast<float>(sRate);
        if (baseDelaySamples < 64.0f) baseDelaySamples = 64.0f;

        auto* lOut = buffer.getWritePointer(0);
        auto* rOut = (numChannels > 1) ? buffer.getWritePointer(1) : lOut;

        float pitchRatio = std::pow(2.0f, pitchSemitones / 12.0f);

        for (int s = 0; s < numSamples; ++s)
        {
            float inL = lOut[s];
            float inR = rOut[s];

            float echoAccumL = 0.0f;
            float echoAccumR = 0.0f;

            for (int tap = 1; tap <= std::min(10, echoCount); ++tap)
            {
                float tapGain = std::pow(feed, static_cast<float>(tap));
                if (tapGain < 0.005f) break;

                float tapDelay = baseDelaySamples * static_cast<float>(tap);
                if (std::abs(pitchSemitones) > 0.05f)
                    tapDelay = tapDelay / std::pow(pitchRatio, static_cast<float>(tap));

                float tapPan = (panSpread * static_cast<float>(tap)) / static_cast<float>(echoCount);
                if (pingPong)
                    tapPan = (tap % 2 == 1) ? -std::abs(tapPan) - 0.5f : std::abs(tapPan) + 0.5f;
                tapPan = juce::jlimit(-1.0f, 1.0f, tapPan);

                float panL = std::cos(0.5f * 3.14159265f * (tapPan * 0.5f + 0.5f));
                float panR = std::sin(0.5f * 3.14159265f * (tapPan * 0.5f + 0.5f));

                long long rPos = writePos - static_cast<long long>(tapDelay);
                while (rPos < 0) rPos += bufLen;

                size_t idx0 = static_cast<size_t>(rPos % bufLen);
                size_t idx1 = static_cast<size_t>((idx0 + 1) % static_cast<size_t>(bufLen));
                float frac = (tapDelay - static_cast<float>(static_cast<long long>(tapDelay)));

                float tapSampL = (1.0f - frac) * delayBufferL[idx0] + frac * delayBufferL[idx1];
                float tapSampR = (1.0f - frac) * delayBufferR[idx0] + frac * delayBufferR[idx1];

                if (fatMode)
                {
                    size_t fatIdx = (idx0 + static_cast<size_t>(sRate * 0.008)) % static_cast<size_t>(bufLen);
                    tapSampL += delayBufferL[fatIdx] * 0.35f;
                    tapSampR += delayBufferR[fatIdx] * 0.35f;
                }

                float monoTap = (tapSampL + tapSampR) * 0.5f * tapGain;
                echoAccumL += monoTap * panL;
                echoAccumR += monoTap * panR;
            }

            delayBufferL[static_cast<size_t>(writePos % bufLen)] = inL + echoAccumL * 0.25f;
            delayBufferR[static_cast<size_t>(writePos % bufLen)] = inR + echoAccumR * 0.25f;

            lOut[s] += echoAccumL;
            rOut[s] += echoAccumR;

            writePos = (writePos + 1) % bufLen;
        }
    }

private:
    double sampleRate = 44100.0;
    std::vector<float> delayBufferL;
    std::vector<float> delayBufferR;
    long long writePos = 0;
};
