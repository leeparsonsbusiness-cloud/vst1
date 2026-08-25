#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <random>
#include "ScaleManager.h"

class HookGeneratorEngine
{
public:
    enum Mood
    {
        RadioCatchy = 0,
        HighEnergyDrop = 1,
        EmotionalBallad = 2,
        SyncopatedPluck = 3,
        VocalTopline = 4
    };

    struct Step
    {
        bool active = false;
        int semitoneOffset = 0;
        float velocity = 0.85f;
        float duration16ths = 0.85f;
    };

    HookGeneratorEngine()
    {
        currentPhrase.resize(64); // 4 bars = 64 16th notes
        generateNewHook(RadioCatchy, 0, 0);
    }

    ~HookGeneratorEngine() = default;

    void setSampleRate(double sRate) { sampleRate = sRate; }
    void setBpm(double bpm) { hostBpm = bpm; }

    void generateNewHook(int mood, int rootKey, int scaleType)
    {
        currentMood = juce::jlimit(0, 4, mood);
        currentPhrase.assign(64, Step{ false, 0, 0.0f, 0.0f });

        juce::Random rand;
        
        // Diatonic scale degree offsets
        std::vector<int> motif16(16, -99);

        if (currentMood == RadioCatchy)
        {
            // A-A-B-A Pop Structure (16-step motif repeated with 4th bar variation)
            std::vector<int> rhythm = { 0, 3, 6, 8, 10, 12, 14 };
            for (int r : rhythm)
            {
                int deg = (rand.nextFloat() > 0.35f) ? (rand.nextInt(5) * 2) : (rand.nextInt(7));
                int offset = ScaleManager::quantizeNote(60 + rootKey + deg, rootKey, scaleType) - (60 + rootKey);
                motif16[static_cast<size_t>(r)] = offset;
            }

            for (int bar = 0; bar < 4; ++bar)
            {
                for (int s = 0; s < 16; ++s)
                {
                    int idx = bar * 16 + s;
                    int noteOff = motif16[static_cast<size_t>(s)];
                    if (noteOff != -99)
                    {
                        // 4th bar variation (high octave flip or rapid finish)
                        if (bar == 3 && s >= 8)
                            noteOff = ScaleManager::quantizeNote(60 + rootKey + noteOff + 12, rootKey, scaleType) - (60 + rootKey);

                        currentPhrase[static_cast<size_t>(idx)] = { true, noteOff, 0.82f + rand.nextFloat() * 0.16f, 0.85f };
                    }
                }
            }
        }
        else if (currentMood == HighEnergyDrop)
        {
            // Syncopated 16th rolling anthem with octave jumps
            for (int i = 0; i < 64; ++i)
            {
                int sub16 = i % 16;
                bool hit = (sub16 % 2 == 0) || (rand.nextFloat() > 0.4f);
                if (hit)
                {
                    int deg = (i % 4 == 0) ? 0 : ((i % 4 == 2) ? 12 : 7);
                    if (rand.nextFloat() > 0.6f) deg += 12;
                    int noteOff = ScaleManager::quantizeNote(60 + rootKey + deg, rootKey, scaleType) - (60 + rootKey);
                    currentPhrase[static_cast<size_t>(i)] = { true, noteOff, 0.90f + rand.nextFloat() * 0.09f, 0.65f };
                }
            }
        }
        else if (currentMood == EmotionalBallad)
        {
            // Long sustained notes with breathing spaces
            for (int bar = 0; bar < 4; ++bar)
            {
                int start1 = bar * 16;
                int start2 = bar * 16 + 8;
                int deg1 = (bar == 0) ? 0 : ((bar == 1) ? 7 : ((bar == 2) ? 9 : 5));
                int deg2 = deg1 + 2;

                int off1 = ScaleManager::quantizeNote(60 + rootKey + deg1, rootKey, scaleType) - (60 + rootKey);
                int off2 = ScaleManager::quantizeNote(60 + rootKey + deg2, rootKey, scaleType) - (60 + rootKey);

                currentPhrase[static_cast<size_t>(start1)] = { true, off1, 0.88f, 3.5f };
                currentPhrase[static_cast<size_t>(start2)] = { true, off2, 0.84f, 3.0f };
            }
        }
        else if (currentMood == SyncopatedPluck)
        {
            // Offbeat syncopated groove
            for (int i = 0; i < 64; ++i)
            {
                int sub16 = i % 16;
                if (sub16 == 0 || sub16 == 3 || sub16 == 6 || sub16 == 10 || sub16 == 12 || sub16 == 14)
                {
                    int deg = (sub16 % 6 == 0) ? 0 : ((sub16 % 4 == 2) ? 7 : 12);
                    int off = ScaleManager::quantizeNote(60 + rootKey + deg, rootKey, scaleType) - (60 + rootKey);
                    currentPhrase[static_cast<size_t>(i)] = { true, off, 0.90f, 0.50f };
                }
            }
        }
        else // VocalTopline
        {
            // Vocal call & response phrases with natural pauses
            for (int bar = 0; bar < 4; ++bar)
            {
                if (bar == 1 || bar == 3) // Leave 2nd half of bar 2 & 4 as rest
                {
                    for (int s = 0; s < 8; s += 2)
                    {
                        int off = ScaleManager::quantizeNote(60 + rootKey + s, rootKey, scaleType) - (60 + rootKey);
                        currentPhrase[static_cast<size_t>(bar * 16 + s)] = { true, off, 0.88f, 0.75f };
                    }
                }
                else
                {
                    for (int s = 0; s < 14; s += (s % 4 == 0 ? 3 : 2))
                    {
                        int off = ScaleManager::quantizeNote(60 + rootKey + (s % 7), rootKey, scaleType) - (60 + rootKey);
                        currentPhrase[static_cast<size_t>(bar * 16 + s)] = { true, off, 0.86f, 0.80f };
                    }
                }
            }
        }
    }

    void processMidi(juce::MidiBuffer& midiMessages, bool active, int rootKey, int scaleType, int numSamples)
    {
        if (!active)
        {
            if (lastTriggeredNote != -1)
            {
                midiMessages.addEvent(juce::MidiMessage::noteOff(1, lastTriggeredNote, 0.0f), 0);
                lastTriggeredNote = -1;
            }
            stepIndex = 0;
            sampleInStep = 0;
            isPlaying = false;
            return;
        }

        // Trigger on any incoming note
        for (const auto metadata : midiMessages)
        {
            auto msg = metadata.getMessage();
            if (msg.isNoteOn())
            {
                baseNote = msg.getNoteNumber();
                isPlaying = true;
                stepIndex = 0;
                sampleInStep = 0;
            }
            else if (msg.isNoteOff() && msg.getNoteNumber() == baseNote)
            {
                // Can sustain or stop
            }
        }

        if (!isPlaying) return;

        double secondsPerBeat = 60.0 / std::max(20.0, hostBpm);
        double secondsPer16th = secondsPerBeat * 0.25;
        int samplesPer16th = static_cast<int>(secondsPer16th * sampleRate);
        if (samplesPer16th < 10) samplesPer16th = 1000;

        juce::MidiBuffer outBuffer;

        for (int s = 0; s < numSamples; ++s)
        {
            if (sampleInStep == 0)
            {
                if (lastTriggeredNote != -1)
                {
                    outBuffer.addEvent(juce::MidiMessage::noteOff(1, lastTriggeredNote, 0.0f), s);
                    lastTriggeredNote = -1;
                }

                const auto& step = currentPhrase[static_cast<size_t>(stepIndex % 64)];
                if (step.active)
                {
                    int note = baseNote + step.semitoneOffset;
                    note = ScaleManager::quantizeNote(note, rootKey, scaleType);
                    note = juce::jlimit(0, 127, note);
                    outBuffer.addEvent(juce::MidiMessage::noteOn(1, note, step.velocity), s);
                    lastTriggeredNote = note;
                }
            }

            sampleInStep++;
            if (sampleInStep >= samplesPer16th)
            {
                sampleInStep = 0;
                stepIndex = (stepIndex + 1) % 64;
            }
        }

        midiMessages.swapWith(outBuffer);
    }

    bool exportHookMidi(int rootKey, int scaleType, const juce::File& destFile)
    {
        juce::MidiMessageSequence seq;
        double ticksPer16th = 960.0 / 4.0;

        for (size_t i = 0; i < currentPhrase.size(); ++i)
        {
            const auto& step = currentPhrase[i];
            if (step.active)
            {
                int note = 60 + rootKey + step.semitoneOffset;
                note = ScaleManager::quantizeNote(note, rootKey, scaleType);
                note = juce::jlimit(0, 127, note);

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
    double sampleRate = 44100.0;
    double hostBpm = 120.0;
    int currentMood = RadioCatchy;
    std::vector<Step> currentPhrase;

    bool isPlaying = false;
    int baseNote = 60;
    int stepIndex = 0;
    int sampleInStep = 0;
    int lastTriggeredNote = -1;
};
