#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <algorithm>
#include <cmath>
#include <vector>

class ZeddifyEngine
{
public:
    enum PatternStyle
    {
        Zedd16thBounce = 0,
        AviciiAnthem = 1,
        EurodanceRiff = 2,
        SynthwaveRoll = 3,
        TropicalStrum = 4
    };

    struct Step
    {
        bool active = false;
        int semitoneOffset = 0;
        float velocity = 0.8f;
        float duration16ths = 0.85f;
    };

    ZeddifyEngine()
    {
        initPatterns();
    }

    ~ZeddifyEngine() = default;

    void setSampleRate(double sRate) { sampleRate = sRate; }
    void setBpm(double bpm) { hostBpm = bpm; }
    void setPatternStyle(int style) { currentStyle = juce::jlimit(0, 4, style); }

    void processMidiBlock(juce::MidiBuffer& midiMessages, bool zeddifyActive, int numSamples)
    {
        if (!zeddifyActive)
        {
            if (wasActive && lastTriggeredNote != -1)
            {
                juce::MidiBuffer cleanBuffer;
                cleanBuffer.addEvent(juce::MidiMessage::noteOff(1, lastTriggeredNote, 0.0f), 0);
                for (const auto metadata : midiMessages)
                    cleanBuffer.addEvent(metadata.getMessage(), metadata.samplePosition);
                midiMessages.swapWith(cleanBuffer);
                lastTriggeredNote = -1;
            }
            wasActive = false;
            playingActive = false;
            currentStepIndex = 0;
            samplesInCurrentStep = 0;
            return;
        }
        wasActive = true;

        int newRootNote = -1;
        bool rootReleased = false;
        int releasePosition = 0;

        for (const auto metadata : midiMessages)
        {
            auto msg = metadata.getMessage();
            if (msg.isNoteOn())
            {
                newRootNote = msg.getNoteNumber();
            }
            else if (msg.isNoteOff() && msg.getNoteNumber() == currentRootNote)
            {
                rootReleased = true;
                releasePosition = metadata.samplePosition;
            }
        }

        if (newRootNote >= 0)
        {
            currentRootNote = newRootNote;
            if (!playingActive)
            {
                playingActive = true;
                currentStepIndex = 0;
                samplesInCurrentStep = 0;
            }
        }

        juce::MidiBuffer zeddifiedBuffer;

        if (rootReleased && newRootNote < 0)
        {
            playingActive = false;
            if (lastTriggeredNote != -1)
            {
                zeddifiedBuffer.addEvent(juce::MidiMessage::noteOff(1, lastTriggeredNote, 0.0f), releasePosition);
                lastTriggeredNote = -1;
            }
            midiMessages.swapWith(zeddifiedBuffer);
            return;
        }

        if (!playingActive)
        {
            midiMessages.swapWith(zeddifiedBuffer);
            return;
        }

        // 16th note length in samples
        double secondsPerBeat = 60.0 / std::max(20.0, hostBpm);
        double secondsPer16th = secondsPerBeat * 0.25;
        int samplesPer16th = static_cast<int>(secondsPer16th * sampleRate);
        if (samplesPer16th < 10) samplesPer16th = 1000;

        const auto& activePattern = patterns[static_cast<size_t>(currentStyle)];

        for (int s = 0; s < numSamples; ++s)
        {
            if (samplesInCurrentStep == 0)
            {
                if (lastTriggeredNote != -1)
                {
                    zeddifiedBuffer.addEvent(juce::MidiMessage::noteOff(1, lastTriggeredNote, 0.0f), s);
                    lastTriggeredNote = -1;
                }

                const auto& step = activePattern[static_cast<size_t>(currentStepIndex % 64)];
                if (step.active)
                {
                    int noteToPlay = currentRootNote + step.semitoneOffset;
                    noteToPlay = juce::jlimit(0, 127, noteToPlay);
                    zeddifiedBuffer.addEvent(juce::MidiMessage::noteOn(1, noteToPlay, step.velocity), s);
                    lastTriggeredNote = noteToPlay;
                }
            }

            samplesInCurrentStep++;
            if (samplesInCurrentStep >= samplesPer16th)
            {
                samplesInCurrentStep = 0;
                currentStepIndex = (currentStepIndex + 1) % 64;
            }
        }

        midiMessages.swapWith(zeddifiedBuffer);
    }

    bool exportToMidiFile(int rootNote, const juce::File& destFile)
    {
        juce::MidiMessageSequence seq;
        double ticksPer16th = 960.0 / 4.0;

        const auto& activePattern = patterns[static_cast<size_t>(currentStyle)];

        for (int i = 0; i < 64; ++i)
        {
            const auto& step = activePattern[static_cast<size_t>(i)];
            if (step.active)
            {
                int note = juce::jlimit(0, 127, rootNote + step.semitoneOffset);
                double startTick = i * ticksPer16th;
                double durationTicks = step.duration16ths * ticksPer16th;
                double endTick = startTick + durationTicks;

                seq.addEvent(juce::MidiMessage::noteOn(1, note, step.velocity), startTick);
                seq.addEvent(juce::MidiMessage::noteOff(1, note, 0.0f), endTick);
            }
        }

        seq.updateMatchedPairs();

        juce::MidiFile midiFile;
        midiFile.setTicksPerQuarterNote(960);
        midiFile.addTrack(seq);

        juce::FileOutputStream stream(destFile);
        if (stream.openedOk())
        {
            stream.setPosition(0);
            stream.truncate();
            return midiFile.writeTo(stream);
        }
        return false;
    }

private:
    void initPatterns()
    {
        patterns.resize(5);
        for (int p = 0; p < 5; ++p)
            patterns[static_cast<size_t>(p)].resize(64);

        // Pattern 0: Zedd 16th Octave Bounce
        auto& zedd = patterns[0];
        for (int i = 0; i < 64; ++i)
        {
            int sub16 = i % 16;
            if (sub16 == 0) zedd[static_cast<size_t>(i)] = { true, 0, 0.95f, 0.8f };
            else if (sub16 == 1) zedd[static_cast<size_t>(i)] = { true, 12, 0.75f, 0.8f };
            else if (sub16 == 2) zedd[static_cast<size_t>(i)] = { true, 0, 0.85f, 0.8f };
            else if (sub16 == 3) zedd[static_cast<size_t>(i)] = { true, 24, 0.90f, 0.8f };
            else if (sub16 == 4) zedd[static_cast<size_t>(i)] = { true, 7, 0.80f, 0.8f };
            else if (sub16 == 5) zedd[static_cast<size_t>(i)] = { false, 0, 0.0f, 0.0f };
            else if (sub16 == 6) zedd[static_cast<size_t>(i)] = { true, 12, 0.85f, 0.8f };
            else if (sub16 == 7) zedd[static_cast<size_t>(i)] = { true, 24, 0.95f, 0.8f };
            else if (sub16 == 8) zedd[static_cast<size_t>(i)] = { true, 0, 0.90f, 0.8f };
            else if (sub16 == 9) zedd[static_cast<size_t>(i)] = { true, 12, 0.75f, 0.8f };
            else if (sub16 == 10) zedd[static_cast<size_t>(i)] = { true, 7, 0.85f, 0.8f };
            else if (sub16 == 11) zedd[static_cast<size_t>(i)] = { true, 19, 0.85f, 0.8f };
            else if (sub16 == 12) zedd[static_cast<size_t>(i)] = { true, 12, 0.90f, 0.8f };
            else if (sub16 == 13) zedd[static_cast<size_t>(i)] = { false, 0, 0.0f, 0.0f };
            else if (sub16 == 14) zedd[static_cast<size_t>(i)] = { true, 24, 0.95f, 0.8f };
            else if (sub16 == 15) zedd[static_cast<size_t>(i)] = { true, 12, 0.80f, 0.8f };
        }

        // Pattern 1: Avicii Progressive Anthem
        auto& avicii = patterns[1];
        static const int aviciiOffsets[16] = { 0, 0, 7, 7, 12, 12, 10, 7, 5, 5, 7, 7, 12, 15, 12, 7 };
        for (int i = 0; i < 64; ++i)
        {
            int sub16 = i % 16;
            avicii[static_cast<size_t>(i)] = { true, aviciiOffsets[sub16], 0.88f, 0.85f };
        }

        // Pattern 2: Eurodance Offbeat Riff
        auto& euro = patterns[2];
        for (int i = 0; i < 64; ++i)
        {
            int sub16 = i % 4;
            if (sub16 == 0) euro[static_cast<size_t>(i)] = { true, 0, 0.95f, 0.75f };
            else if (sub16 == 2) euro[static_cast<size_t>(i)] = { true, 12, 0.90f, 0.75f };
            else euro[static_cast<size_t>(i)] = { false, 0, 0.0f, 0.0f };
        }

        // Pattern 3: Synthwave 16th Roll
        auto& synthwave = patterns[3];
        for (int i = 0; i < 64; ++i)
        {
            int sub16 = i % 8;
            int oct = (sub16 == 0 || sub16 == 4) ? 0 : 12;
            float vel = (sub16 == 0 || sub16 == 4) ? 0.95f : 0.78f;
            synthwave[static_cast<size_t>(i)] = { true, oct, vel, 0.70f };
        }

        // Pattern 4: Tropical Pop Strum
        auto& trop = patterns[4];
        static const int tropOffsets[16] = { 0, 7, 12, 15, -1, 0, 7, 12, -1, -1, 0, 7, 12, 15, 12, 7 };
        for (int i = 0; i < 64; ++i)
        {
            int sub16 = i % 16;
            if (tropOffsets[sub16] >= 0)
                trop[static_cast<size_t>(i)] = { true, tropOffsets[sub16], 0.85f, 0.60f };
            else
                trop[static_cast<size_t>(i)] = { false, 0, 0.0f, 0.0f };
        }
    }

    double sampleRate = 44100.0;
    double hostBpm = 120.0;
    int currentStyle = 0;
    std::vector<std::vector<Step>> patterns;

    bool wasActive = false;
    bool playingActive = false;
    int currentRootNote = 60;
    int currentStepIndex = 0;
    int samplesInCurrentStep = 0;
    int lastTriggeredNote = -1;
};
