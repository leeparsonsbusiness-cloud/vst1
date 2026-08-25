#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "ScaleManager.h"
#include <vector>
#include <algorithm>

class CounterMelodyEngine
{
public:
    CounterMelodyEngine() = default;
    ~CounterMelodyEngine() = default;

    void setSampleRate(double sRate) { sampleRate = sRate; }
    void setBpm(double bpm) { hostBpm = bpm; }

    void processMidi(juce::MidiBuffer& midiMessages, bool active, int rootKey, int scaleType, int numSamples)
    {
        if (!active)
        {
            silenceSamples = 0;
            fillStep = -1;
            lastFillNote = -1;
            return;
        }

        bool hasNoteOn = false;
        int latestNote = -1;

        for (const auto metadata : midiMessages)
        {
            auto msg = metadata.getMessage();
            if (msg.isNoteOn())
            {
                hasNoteOn = true;
                latestNote = msg.getNoteNumber();
            }
        }

        if (hasNoteOn)
        {
            lastMainMelodyNote = latestNote;
            silenceSamples = 0;
            fillStep = -1;
            if (lastFillNote != -1)
            {
                midiMessages.addEvent(juce::MidiMessage::noteOff(1, lastFillNote, 0.0f), 0);
                lastFillNote = -1;
            }
            return;
        }

        double secondsPerBeat = 60.0 / std::max(20.0, hostBpm);
        int silenceThreshold = static_cast<int>(secondsPerBeat * 0.9 * sampleRate); // ~1 beat pause
        int samplesPer16th = static_cast<int>(secondsPerBeat * 0.25 * sampleRate);
        if (samplesPer16th < 10) samplesPer16th = 1000;

        silenceSamples += numSamples;

        // Trigger Counter-Melody Fill when pause is detected
        if (silenceSamples >= silenceThreshold && fillStep == -1 && lastMainMelodyNote >= 0)
        {
            fillStep = 0;
            fillSampleCounter = 0;
        }

        if (fillStep >= 0 && fillStep < 4)
        {
            for (int s = 0; s < numSamples; ++s)
            {
                if (fillSampleCounter == 0)
                {
                    if (lastFillNote != -1)
                    {
                        midiMessages.addEvent(juce::MidiMessage::noteOff(1, lastFillNote, 0.0f), s);
                        lastFillNote = -1;
                    }

                    // 3-note melodic answer motif (e.g. +7, +5, +3 or +12, +7, +10)
                    static const int fillMotif[4] = { 12, 10, 7, -1 };
                    int offset = fillMotif[fillStep];
                    if (offset >= 0)
                    {
                        int note = lastMainMelodyNote + offset;
                        note = ScaleManager::quantizeNote(note, rootKey, scaleType);
                        note = juce::jlimit(0, 127, note);
                        midiMessages.addEvent(juce::MidiMessage::noteOn(1, note, 0.78f), s);
                        lastFillNote = note;
                    }
                }

                fillSampleCounter++;
                if (fillSampleCounter >= samplesPer16th)
                {
                    fillSampleCounter = 0;
                    fillStep++;
                    if (fillStep >= 4)
                    {
                        if (lastFillNote != -1)
                        {
                            midiMessages.addEvent(juce::MidiMessage::noteOff(1, lastFillNote, 0.0f), s);
                            lastFillNote = -1;
                        }
                    }
                }
            }
        }
    }

private:
    double sampleRate = 44100.0;
    double hostBpm = 120.0;
    int silenceSamples = 0;
    int lastMainMelodyNote = 60;
    int fillStep = -1;
    int fillSampleCounter = 0;
    int lastFillNote = -1;
};
