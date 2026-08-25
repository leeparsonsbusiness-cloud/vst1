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
        sampleRate = sRate;
        int bufSize = static_cast<int>(sampleRate * 2.0); // 2-second buffer
        ringBuffer.setSize(2, std::max(bufSize, 4096));
        ringBuffer.clear();
        writePos = 0;
        activeMode = Off;
    }

    void setBpm(double bpm) { hostBpm = bpm; }
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
        }
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        const int numSamples = buffer.getNumSamples();
        const int numChannels = buffer.getNumChannels();
        const int bufLength = ringBuffer.getNumSamples();

        if (bufLength <= 0) return;

        double secondsPerBeat = 60.0 / std::max(20.0, hostBpm);
        double secondsPer16th = secondsPerBeat * 0.25;
        int stutterLength = static_cast<int>(secondsPer16th * sampleRate);
        if (stutterLength < 64) stutterLength = 512;

        if (activeMode == Off)
        {
            // Record incoming audio into circular buffer
            for (int ch = 0; ch < numChannels; ++ch)
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
            float speedDecayPerSample = 1.0f / (stopDurationSec * static_cast<float>(sampleRate));

            for (int s = 0; s < numSamples; ++s)
            {
                tapeSpeed = std::max(0.0f, tapeSpeed - speedDecayPerSample);
                tapeReadPos += tapeSpeed;
                while (tapeReadPos >= static_cast<float>(bufLength))
                    tapeReadPos -= static_cast<float>(bufLength);

                int idx0 = static_cast<int>(tapeReadPos);
                int idx1 = (idx0 + 1) % bufLength;
                float frac = tapeReadPos - static_cast<float>(idx0);

                float gain = (tapeSpeed > 0.02f) ? 1.0f : (tapeSpeed * 50.0f);

                for (int ch = 0; ch < numChannels; ++ch)
                {
                    const auto* rPtr = ringBuffer.getReadPointer(ch);
                    float sample = (1.0f - frac) * rPtr[idx0] + frac * rPtr[idx1];
                    buffer.setSample(ch, s, sample * gain);
                }
            }
        }
        else if (activeMode == Stutter16)
        {
            for (int s = 0; s < numSamples; ++s)
            {
                int offset = static_cast<int>(effectPhase) % stutterLength;
                int readIdx = (stutterCapturePos - stutterLength + offset) % bufLength;
                if (readIdx < 0) readIdx += bufLength;

                // Anti-click window at repeat boundaries
                float window = 1.0f;
                if (offset < 48)
                    window = static_cast<float>(offset) / 48.0f;
                else if (offset > stutterLength - 48)
                    window = static_cast<float>(stutterLength - offset) / 48.0f;

                for (int ch = 0; ch < numChannels; ++ch)
                {
                    float sample = ringBuffer.getSample(ch, readIdx);
                    buffer.setSample(ch, s, sample * window);
                }

                effectPhase += 1.0f;
            }
        }
        else if (activeMode == PitchDive)
        {
            float diveDurationSec = 0.42f;
            float divePerSample = 1.0f / (diveDurationSec * static_cast<float>(sampleRate));

            for (int s = 0; s < numSamples; ++s)
            {
                effectPhase = std::min(1.0f, effectPhase + divePerSample);
                float speed = std::pow(0.25f, effectPhase); // Drops 2 octaves

                diveReadPos += speed;
                while (diveReadPos >= static_cast<float>(bufLength))
                    diveReadPos -= static_cast<float>(bufLength);

                int idx0 = static_cast<int>(diveReadPos);
                int idx1 = (idx0 + 1) % bufLength;
                float frac = diveReadPos - static_cast<float>(idx0);

                float gain = 1.0f - (effectPhase * 0.4f);

                for (int ch = 0; ch < numChannels; ++ch)
                {
                    const auto* rPtr = ringBuffer.getReadPointer(ch);
                    float sample = (1.0f - frac) * rPtr[idx0] + frac * rPtr[idx1];
                    buffer.setSample(ch, s, sample * gain);
                }
            }
        }
        else if (activeMode == ReverseSwell)
        {
            float reverseDurationSamples = static_cast<float>(sampleRate * 0.45);

            for (int s = 0; s < numSamples; ++s)
            {
                reversePlayPos -= 1.0f;
                while (reversePlayPos < 0.0f)
                    reversePlayPos += static_cast<float>(bufLength);

                int idx0 = static_cast<int>(reversePlayPos);
                int idx1 = (idx0 + 1) % bufLength;
                float frac = reversePlayPos - static_cast<float>(idx0);

                effectPhase = std::min(1.0f, effectPhase + (1.0f / reverseDurationSamples));
                float swellGain = effectPhase * effectPhase; // Exponential swell

                for (int ch = 0; ch < numChannels; ++ch)
                {
                    const auto* rPtr = ringBuffer.getReadPointer(ch);
                    float sample = (1.0f - frac) * rPtr[idx0] + frac * rPtr[idx1];
                    buffer.setSample(ch, s, sample * swellGain);
                }
            }
        }

        // Keep updating write position in ring buffer
        for (int ch = 0; ch < numChannels; ++ch)
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
    }

private:
    double sampleRate = 44100.0;
    double hostBpm = 120.0;
    GlitchMode activeMode = Off;

    juce::AudioBuffer<float> ringBuffer;
    int writePos = 0;

    float effectPhase = 0.0f;
    int triggerSamplePos = 0;
    int stutterCapturePos = 0;

    float tapeSpeed = 1.0f;
    float tapeReadPos = 0.0f;
    float diveReadPos = 0.0f;
    float reversePlayPos = 0.0f;
};
