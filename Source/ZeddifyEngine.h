#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include <random>

class ZeddifyEngine
{
public:
    enum PatternStyle
    {
        Zedd16thBounce = 0,
        AviciiAnthem = 1,
        EurodanceRiff = 2,
        SynthwaveRoll = 3,
        TropicalStrum = 4,
        NuDiscoStabs = 5,
        FutureRave = 6,
        SlapHouseTriplet = 7,
        HyperpopGlitch = 8,
        TrapHalfTime = 9,
        Acid303Roll = 10,
        StadiumStabs = 11
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
    void setPatternStyle(int style) { currentStyle = juce::jlimit(0, 11, style); }

    void mutateCurrentPattern()
    {
        auto& activePattern = patterns[static_cast<size_t>(currentStyle)];
        juce::Random rand;

        for (int i = 0; i < 64; ++i)
        {
            auto& step = activePattern[static_cast<size_t>(i)];
            
            // 4th bar (steps 48-63): Injcet octave jumps or fills
            if (i >= 48)
            {
                if (rand.nextFloat() > 0.4f)
                {
                    step.active = true;
                    step.semitoneOffset = (rand.nextFloat() > 0.5f) ? 24 : 12;
                    step.velocity = 0.95f;
                }
            }
            // Passing notes & variations
            else if (rand.nextFloat() > 0.75f)
            {
                if (!step.active)
                {
                    step.active = true;
                    step.semitoneOffset = (rand.nextFloat() > 0.5f) ? 7 : 12;
                    step.velocity = 0.75f;
                }
                else
                {
                    step.velocity = juce::jlimit(0.5f, 1.0f, step.velocity + rand.nextFloat() * 0.2f - 0.1f);
                }
            }
        }
    }

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
        patterns.resize(12);
        for (int p = 0; p < 12; ++p)
            patterns[static_cast<size_t>(p)].resize(64);

        // 0. Zedd 16th Bounce
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

        // 1. Avicii Progressive Anthem
        auto& avicii = patterns[1];
        static const int aviciiOffsets[16] = { 0, 0, 7, 7, 12, 12, 10, 7, 5, 5, 7, 7, 12, 15, 12, 7 };
        for (int i = 0; i < 64; ++i)
            avicii[static_cast<size_t>(i)] = { true, aviciiOffsets[i % 16], 0.88f, 0.85f };

        // 2. Eurodance Offbeat Riff
        auto& euro = patterns[2];
        for (int i = 0; i < 64; ++i)
        {
            int sub16 = i % 4;
            if (sub16 == 0) euro[static_cast<size_t>(i)] = { true, 0, 0.95f, 0.75f };
            else if (sub16 == 2) euro[static_cast<size_t>(i)] = { true, 12, 0.90f, 0.75f };
            else euro[static_cast<size_t>(i)] = { false, 0, 0.0f, 0.0f };
        }

        // 3. Synthwave 16th Roll
        auto& synthwave = patterns[3];
        for (int i = 0; i < 64; ++i)
        {
            int sub16 = i % 8;
            int oct = (sub16 == 0 || sub16 == 4) ? 0 : 12;
            synthwave[static_cast<size_t>(i)] = { true, oct, (sub16 == 0) ? 0.95f : 0.78f, 0.70f };
        }

        // 4. Tropical Pop Strum
        auto& trop = patterns[4];
        static const int tropOffsets[16] = { 0, 7, 12, 15, -1, 0, 7, 12, -1, -1, 0, 7, 12, 15, 12, 7 };
        for (int i = 0; i < 64; ++i)
        {
            int off = tropOffsets[i % 16];
            trop[static_cast<size_t>(i)] = (off >= 0) ? Step{ true, off, 0.85f, 0.60f } : Step{ false, 0, 0.0f, 0.0f };
        }

        // 5. Nu-Disco Funk Stabs (Dua Lipa)
        auto& disco = patterns[5];
        static const int discoOffsets[16] = { 0, -1, 12, 0, -1, 12, -1, 0, -1, 12, 12, -1, 0, 12, -1, 0 };
        for (int i = 0; i < 64; ++i)
        {
            int off = discoOffsets[i % 16];
            disco[static_cast<size_t>(i)] = (off >= 0) ? Step{ true, off, 0.92f, 0.50f } : Step{ false, 0, 0.0f, 0.0f };
        }

        // 6. Future Rave Gallop (Guetta / Morten)
        auto& rave = patterns[6];
        static const int raveOffsets[16] = { 0, 0, 12, 0, 0, 12, 0, 0, 12, 0, 12, 12, 0, 0, 12, 24 };
        for (int i = 0; i < 64; ++i)
            rave[static_cast<size_t>(i)] = { true, raveOffsets[i % 16], 0.92f, 0.65f };

        // 7. Slap House Triplet Groove
        auto& slap = patterns[7];
        static const int slapOffsets[16] = { 0, -1, 0, -1, 12, -1, 0, 12, -1, 0, -1, 12, 0, -1, 12, -1 };
        for (int i = 0; i < 64; ++i)
        {
            int off = slapOffsets[i % 16];
            slap[static_cast<size_t>(i)] = (off >= 0) ? Step{ true, off, 0.94f, 0.45f } : Step{ false, 0, 0.0f, 0.0f };
        }

        // 8. Hyperpop Glitch 32nds
        auto& hyper = patterns[8];
        for (int i = 0; i < 64; ++i)
        {
            int oct = (i % 2 == 0) ? ((i % 4 == 0) ? 0 : 12) : 24;
            hyper[static_cast<size_t>(i)] = { true, oct, 0.88f, 0.40f };
        }

        // 9. Trap Half-Time Roll
        auto& trap = patterns[9];
        static const int trapOffsets[16] = { 0, -1, -1, -1, 7, -1, -1, -1, 12, -1, -1, -1, 15, -1, 12, -1 };
        for (int i = 0; i < 64; ++i)
        {
            int off = trapOffsets[i % 16];
            trap[static_cast<size_t>(i)] = (off >= 0) ? Step{ true, off, 0.85f, 0.70f } : Step{ false, 0, 0.0f, 0.0f };
        }

        // 10. Acid 303 Rolling Bass
        auto& acid = patterns[10];
        static const int acidOffsets[16] = { 0, 12, 0, 0, 12, 0, 7, 12, 0, 0, 12, 19, 0, 12, 0, 24 };
        for (int i = 0; i < 64; ++i)
            acid[static_cast<size_t>(i)] = { true, acidOffsets[i % 16], (i % 4 == 0) ? 0.98f : 0.80f, 0.75f };

        // 11. Stadium Anthem Stabs
        auto& stadium = patterns[11];
        static const int stadiumOffsets[16] = { 0, -1, -1, 0, -1, -1, 0, -1, -1, 0, -1, 0, -1, 0, -1, -1 };
        for (int i = 0; i < 64; ++i)
        {
            int off = stadiumOffsets[i % 16];
            stadium[static_cast<size_t>(i)] = (off >= 0) ? Step{ true, off, 0.98f, 0.85f } : Step{ false, 0, 0.0f, 0.0f };
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
