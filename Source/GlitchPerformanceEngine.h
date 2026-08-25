#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <algorithm>
#include <cmath>

class GlitchPerformanceEngine
{
public:
    enum GlitchMode
    {
        Off = 0,
        TapeStop = 1,
        Stutter16 = 2,
        PitchDive = 3,
        ReverseSwell = 4
    };

    GlitchPerformanceEngine()
    {
        ringBuffer.setSize(2, 96000);
        ringBuffer.clear();
    }

    ~GlitchPerformanceEngine() = default;

    void prepare(double sRate)
    {
        sampleRate = (sRate > 1000.0) ? sRate : 44100.0;
        int bufSize = std::max(8192, static_cast<int>(sampleRate * 2.5));
        ringBuffer.setSize(2, bufSize);
        ringBuffer.clear();
        writePos = 0;
        activeMode = Off;
    }

    void setBpm(double bpm) { hostBpm = (bpm > 20.0) ? bpm : 120.0; }
    void setMode(int mode)
    {
        GlitchMode newMode = static_cast<GlitchMode>(juce::jlimit(0, 4, mode));
        if (newMode != activeMode)
        {
            activeMode = newMode;
            effectPhase = 0.0f;
            triggerSamplePos = writePos;
            stutterCapturePos = writePos;
            reversePlayPos = static_cast<float>(writePos);
            tapeSpeed = 1.0f;
            tapeReadPos = static_cast<float>(writePos);
        }
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        const int numSamples = buffer.getNumSamples();
        const int numChannels = buffer.getNumChannels();
        const int bufLength = ringBuffer.getNumSamples();
        const int ringChannels = ringBuffer.getNumChannels();

        if (bufLength <= 128 || ringChannels <= 0) return;

        double sRate = (sampleRate > 1000.0) ? sampleRate : 44100.0;
        double secondsPerBeat = 60.0 / std::max(20.0, hostBpm);
        double secondsPer16th = secondsPerBeat * 0.25;
        int stutterLength = static_cast<int>(secondsPer16th * sRate);
        if (stutterLength < 64) stutterLength = 512;

        int safeChannels = std::min(numChannels, ringChannels);

        if (activeMode == Off)
        {
            for (int ch = 0; ch < safeChannels; ++ch)
            {
                auto* writePtr = ringBuffer.getWritePointer(ch);
                const auto* readPtr = buffer.getReadPointer(ch);

                for (int s = 0; s < numSamples; ++s)
                {
                    int idx = (writePos + s) % bufLength;
                    writePtr[idx] = readPtr[s];
                }
            }
            writePos = (writePos + numSamples) % bufLength;
            return;
        }

        // Process Glitch Effects
        if (activeMode == TapeStop)
        {
            float stopDurationSec = 0.38f;
            float speedDecayPerSample = 1.0f / (stopDurationSec * static_cast<float>(sRate));

            for (int s = 0; s < numSamples; ++s)
            {
                tapeSpeed = std::max(0.0f, tapeSpeed - speedDecayPerSample);
                tapeReadPos += tapeSpeed;
                while (tapeReadPos >= static_cast<float>(bufLength))
                    tapeReadPos -= static_cast<float>(bufLength);

                int idx0 = static_cast<int>(tapeReadPos);
                int idx1 = (idx0 + 1) % bufLength;
                float frac = tapeReadPos - static_cast<float>(idx0);

                for (int ch = 0; ch < numChannels; ++ch)
                {
                    int rCh = std::min(ch, ringChannels - 1);
                    const auto* ringPtr = ringBuffer.getReadPointer(rCh);
                    float samp = (1.0f - frac) * ringPtr[idx0] + frac * ringPtr[idx1];
                    buffer.setSample(ch, s, samp * (tapeSpeed > 0.01f ? 1.0f : 0.0f));
                }
            }
        }
        else if (activeMode == Stutter16)
        {
            for (int s = 0; s < numSamples; ++s)
            {
                int readOffset = (stutterPlayPos++) % stutterLength;
                int readIdx = (stutterCapturePos - stutterLength + readOffset);
                while (readIdx < 0) readIdx += bufLength;
                readIdx %= bufLength;

                float env = 1.0f;
                int fadeLen = std::min(64, stutterLength / 4);
                if (readOffset < fadeLen)
                    env = static_cast<float>(readOffset) / static_cast<float>(fadeLen);
                else if (readOffset > stutterLength - fadeLen)
                    env = static_cast<float>(stutterLength - readOffset) / static_cast<float>(fadeLen);

                for (int ch = 0; ch < numChannels; ++ch)
                {
                    int rCh = std::min(ch, ringChannels - 1);
                    float samp = ringBuffer.getSample(rCh, readIdx) * env;
                    buffer.setSample(ch, s, samp);
                }
            }
        }
        else if (activeMode == PitchDive)
        {
            float diveDurationSec = 0.50f;
            float diveStep = 1.0f / (diveDurationSec * static_cast<float>(sRate));

            for (int s = 0; s < numSamples; ++s)
            {
                diveProgress = std::min(1.0f, diveProgress + diveStep);
                float speed = std::pow(0.25f, diveProgress); // Drop 2 octaves

                tapeReadPos += speed;
                while (tapeReadPos >= static_cast<float>(bufLength))
                    tapeReadPos -= static_cast<float>(bufLength);

                int idx0 = static_cast<int>(tapeReadPos);
                int idx1 = (idx0 + 1) % bufLength;
                float frac = tapeReadPos - static_cast<float>(idx0);

                float gain = 1.0f - (diveProgress * 0.6f);

                for (int ch = 0; ch < numChannels; ++ch)
                {
                    int rCh = std::min(ch, ringChannels - 1);
                    const auto* ringPtr = ringBuffer.getReadPointer(rCh);
                    float samp = ((1.0f - frac) * ringPtr[idx0] + frac * ringPtr[idx1]) * gain;
                    buffer.setSample(ch, s, samp);
                }
            }
        }
        else if (activeMode == ReverseSwell)
        {
            for (int s = 0; s < numSamples; ++s)
            {
                reversePlayPos -= 1.0f;
                while (reversePlayPos < 0.0f)
                    reversePlayPos += static_cast<float>(bufLength);

                int idx0 = static_cast<int>(reversePlayPos);
                int idx1 = (idx0 + 1) % bufLength;
                float frac = reversePlayPos - static_cast<float>(idx0);

                effectPhase = std::min(1.0f, effectPhase + 0.0001f);
                float gain = std::pow(effectPhase, 1.5f);

                for (int ch = 0; ch < numChannels; ++ch)
                {
                    int rCh = std::min(ch, ringChannels - 1);
                    const auto* ringPtr = ringBuffer.getReadPointer(rCh);
                    float samp = ((1.0f - frac) * ringPtr[idx0] + frac * ringPtr[idx1]) * gain;
                    buffer.setSample(ch, s, samp);
                }
            }
        }
    }

private:
    double sampleRate = 44100.0;
    double hostBpm = 120.0;
    GlitchMode activeMode = Off;

    juce::AudioBuffer<float> ringBuffer;
    int writePos = 0;
    int triggerSamplePos = 0;
    int stutterCapturePos = 0;
    int stutterPlayPos = 0;

    float tapeSpeed = 1.0f;
    float tapeReadPos = 0.0f;
    float diveProgress = 0.0f;
    float reversePlayPos = 0.0f;
    float effectPhase = 0.0f;
};
