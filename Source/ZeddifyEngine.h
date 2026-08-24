#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <algorithm>
#include <cmath>

class ZeddifyEngine
{
public:
    struct Step
    {
        bool active = false;
        int semitoneOffset = 0;
        float velocity = 0.8f;
        float duration16ths = 0.85f;
    };

    ZeddifyEngine()
    {
        initPattern();
    }

    ~ZeddifyEngine() = default;

    void setSampleRate(double sRate) { sampleRate = sRate; }
    void setBpm(double bpm) { hostBpm = bpm; }

    void processMidiBlock(juce::MidiBuffer& midiMessages, bool zeddifyActive, int numSamples)
    {
        if (!zeddifyActive)
        {
            playingActive = false;
            return;
        }

        // Intercept single note-on triggers
        for (const auto metadata : midiMessages)
        {
            auto msg = metadata.getMessage();
            if (msg.isNoteOn())
            {
                currentRootNote = msg.getNoteNumber();
                playingActive = true;
                currentStepIndex = 0;
                samplesInCurrentStep = 0;
                lastTriggeredNote = -1;
            }
            else if (msg.isNoteOff() && msg.getNoteNumber() == currentRootNote)
            {
                // Sequence completes pattern or stops when key is released
                playingActive = false;
                if (lastTriggeredNote != -1)
                {
                    midiMessages.addEvent(juce::MidiMessage::noteOff(1, lastTriggeredNote, 0.0f), metadata.samplePosition);
                    lastTriggeredNote = -1;
                }
            }
        }

        if (!playingActive)
            return;

        juce::MidiBuffer zeddifiedBuffer;
        
        // 16th note length in samples
        double secondsPerBeat = 60.0 / std::max(20.0, hostBpm);
        double secondsPer16th = secondsPerBeat * 0.25;
        int samplesPer16th = static_cast<int>(secondsPer16th * sampleRate);
        if (samplesPer16th < 10) samplesPer16th = 1000;

        for (int s = 0; s < numSamples; ++s)
        {
            if (samplesInCurrentStep == 0)
            {
                // Step start: Send Note Off for previous step if active
                if (lastTriggeredNote != -1)
                {
                    zeddifiedBuffer.addEvent(juce::MidiMessage::noteOff(1, lastTriggeredNote, 0.0f), s);
                    lastTriggeredNote = -1;
                }

                // Trigger current step
                const auto& step = pattern[static_cast<size_t>(currentStepIndex % 64)];
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

        // Swap original buffer with zeddified MIDI pattern
        midiMessages.swapWith(zeddifiedBuffer);
    }

    bool exportToMidiFile(int rootNote, const juce::File& destFile)
    {
        juce::MidiMessageSequence seq;
        double ticksPer16th = 960.0 / 4.0; // 240 ticks per 16th note

        for (int i = 0; i < 64; ++i)
        {
            const auto& step = pattern[static_cast<size_t>(i)];
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
    void initPattern()
    {
        pattern.resize(64);

        // 4-Bar Syncopated Zedd Electro-Pop Riff (64 16th steps)
        // Bar 1: Octave bounce drive
        pattern[0]  = { true,  0,  0.95f, 0.8f };
        pattern[1]  = { true, 12,  0.75f, 0.8f };
        pattern[2]  = { true,  0,  0.85f, 0.8f };
        pattern[3]  = { true, 24,  0.90f, 0.8f };
        pattern[4]  = { true,  7,  0.80f, 0.8f };
        pattern[5]  = { false, 0,  0.00f, 0.0f }; // Syncopated rest
        pattern[6]  = { true, 12,  0.85f, 0.8f };
        pattern[7]  = { true, 24,  0.95f, 0.8f };
        pattern[8]  = { true,  0,  0.90f, 0.8f };
        pattern[9]  = { true, 12,  0.75f, 0.8f };
        pattern[10] = { true,  7,  0.85f, 0.8f };
        pattern[11] = { true, 19,  0.85f, 0.8f };
        pattern[12] = { true, 12,  0.90f, 0.8f };
        pattern[13] = { false, 0,  0.00f, 0.0f };
        pattern[14] = { true, 24,  0.95f, 0.8f };
        pattern[15] = { true, 12,  0.80f, 0.8f };

        // Bar 2: Melodic lift (Minor/Major 3rd & 7th accents)
        pattern[16] = { true,  3,  0.95f, 0.8f };
        pattern[17] = { true, 15,  0.75f, 0.8f };
        pattern[18] = { true,  3,  0.85f, 0.8f };
        pattern[19] = { true, 27,  0.90f, 0.8f };
        pattern[20] = { true, 10,  0.80f, 0.8f };
        pattern[21] = { false, 0,  0.00f, 0.0f };
        pattern[22] = { true, 15,  0.85f, 0.8f };
        pattern[23] = { true, 27,  0.95f, 0.8f };
        pattern[24] = { true,  3,  0.90f, 0.8f };
        pattern[25] = { true, 15,  0.75f, 0.8f };
        pattern[26] = { true, 10,  0.85f, 0.8f };
        pattern[27] = { true, 22,  0.85f, 0.8f };
        pattern[28] = { true, 15,  0.90f, 0.8f };
        pattern[29] = { false, 0,  0.00f, 0.0f };
        pattern[30] = { true, 27,  0.95f, 0.8f };
        pattern[31] = { true, 15,  0.80f, 0.8f };

        // Bar 3: Sub-octave dive & high syncopation
        pattern[32] = { true,  5,  0.95f, 0.8f };
        pattern[33] = { true, 17,  0.75f, 0.8f };
        pattern[34] = { true,  5,  0.85f, 0.8f };
        pattern[35] = { true, 29,  0.90f, 0.8f };
        pattern[36] = { true, 12,  0.80f, 0.8f };
        pattern[37] = { false, 0,  0.00f, 0.0f };
        pattern[38] = { true, 17,  0.85f, 0.8f };
        pattern[39] = { true, 29,  0.95f, 0.8f };
        pattern[40] = { true,  5,  0.90f, 0.8f };
        pattern[41] = { true, 17,  0.75f, 0.8f };
        pattern[42] = { true, 12,  0.85f, 0.8f };
        pattern[43] = { true, 24,  0.85f, 0.8f };
        pattern[44] = { true, 17,  0.90f, 0.8f };
        pattern[45] = { false, 0,  0.00f, 0.0f };
        pattern[46] = { true, 29,  0.95f, 0.8f };
        pattern[47] = { true, 17,  0.80f, 0.8f };

        // Bar 4: Turnaround roll (Rapid arpeggiated fill)
        pattern[48] = { true,  7,  0.95f, 0.8f };
        pattern[49] = { true, 19,  0.85f, 0.8f };
        pattern[50] = { true, 12,  0.90f, 0.8f };
        pattern[51] = { true, 24,  0.90f, 0.8f };
        pattern[52] = { true, 15,  0.95f, 0.8f };
        pattern[53] = { true, 27,  0.95f, 0.8f };
        pattern[54] = { true, 19,  0.95f, 0.8f };
        pattern[55] = { true, 31,  1.00f, 0.8f };
        pattern[56] = { true, 24,  1.00f, 0.7f };
        pattern[57] = { true, 22,  0.95f, 0.7f };
        pattern[58] = { true, 19,  0.90f, 0.7f };
        pattern[59] = { true, 17,  0.85f, 0.7f };
        pattern[60] = { true, 15,  0.80f, 0.7f };
        pattern[61] = { true, 12,  0.75f, 0.7f };
        pattern[62] = { true,  7,  0.70f, 0.7f };
        pattern[63] = { true,  0,  0.65f, 0.7f };
    }

    double sampleRate = 44100.0;
    double hostBpm = 120.0;
    std::vector<Step> pattern;

    bool playingActive = false;
    int currentRootNote = 60;
    int currentStepIndex = 0;
    int samplesInCurrentStep = 0;
    int lastTriggeredNote = -1;
};
