#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <array>
#include <cmath>

class ScaleManager
{
public:
    enum ScaleType
    {
        Off = 0,
        Major = 1,
        NaturalMinor = 2,
        HarmonicMinor = 3,
        Dorian = 4,
        Mixolydian = 5,
        Pentatonic = 6
    };

    ScaleManager() = default;
    ~ScaleManager() = default;

    static int quantizeNote(int noteNumber, int rootKey, int scaleType)
    {
        if (scaleType == Off)
            return noteNumber;

        const auto& scaleIntervals = getScaleIntervals(scaleType);
        if (scaleIntervals.empty())
            return noteNumber;

        int octave = noteNumber / 12;
        int noteInOctave = noteNumber % 12;
        int shiftedNote = (noteInOctave - rootKey + 12) % 12;

        int closestInterval = scaleIntervals[0];
        int minDistance = 999;

        for (int interval : scaleIntervals)
        {
            int dist = std::abs(shiftedNote - interval);
            if (dist < minDistance)
            {
                minDistance = dist;
                closestInterval = interval;
            }
        }

        int quantizedNoteInOctave = (rootKey + closestInterval) % 12;
        int quantizedNote = octave * 12 + quantizedNoteInOctave;

        return juce::jlimit(0, 127, quantizedNote);
    }

    static int remapEasyKey(int noteNumber, int rootKey, int scaleType)
    {
        if (scaleType == Off)
            return noteNumber;

        const auto& scaleIntervals = getScaleIntervals(scaleType);
        if (scaleIntervals.empty())
            return noteNumber;

        int octave = noteNumber / 12;
        int noteInOctave = noteNumber % 12;

        // Map white keys (0, 2, 4, 5, 7, 9, 11) to scale degrees 0..6
        int whiteDegree = -1;
        switch (noteInOctave)
        {
            case 0: whiteDegree = 0; break; // C
            case 2: whiteDegree = 1; break; // D
            case 4: whiteDegree = 2; break; // E
            case 5: whiteDegree = 3; break; // F
            case 7: whiteDegree = 4; break; // G
            case 9: whiteDegree = 5; break; // A
            case 11: whiteDegree = 6; break; // B
            default: whiteDegree = -1; break; // Black keys
        }

        if (whiteDegree >= 0)
        {
            int scaleIdx = whiteDegree % static_cast<int>(scaleIntervals.size());
            int interval = scaleIntervals[static_cast<size_t>(scaleIdx)];
            int targetNote = octave * 12 + rootKey + interval;
            return juce::jlimit(0, 127, targetNote);
        }

        return quantizeNote(noteNumber, rootKey, scaleType);
    }

    void processMidi(juce::MidiBuffer& midiMessages, int rootKey, int scaleType, float strumMs, 
                     bool easyKeyActive, double sampleRate, int numSamples)
    {
        if (scaleType == Off && strumMs < 0.001f && !easyKeyActive)
            return;

        juce::MidiBuffer quantizedBuffer;
        int chordNoteCounter = 0;
        int strumSampleDelay = static_cast<int>((strumMs * 0.001f) * sampleRate);

        for (const auto metadata : midiMessages)
        {
            auto msg = metadata.getMessage();
            int samplePos = metadata.samplePosition;

            if (msg.isNoteOn() || msg.isNoteOff())
            {
                int originalNote = msg.getNoteNumber();
                int finalNote = easyKeyActive ? remapEasyKey(originalNote, rootKey, scaleType)
                                              : quantizeNote(originalNote, rootKey, scaleType);

                int delayedPos = samplePos;
                if (msg.isNoteOn() && strumSampleDelay > 0)
                {
                    delayedPos = std::min(numSamples - 1, samplePos + (chordNoteCounter * strumSampleDelay));
                    chordNoteCounter++;
                }

                if (msg.isNoteOn())
                    quantizedBuffer.addEvent(juce::MidiMessage::noteOn(msg.getChannel(), finalNote, msg.getFloatVelocity()), delayedPos);
                else
                    quantizedBuffer.addEvent(juce::MidiMessage::noteOff(msg.getChannel(), finalNote, msg.getFloatVelocity()), samplePos);
            }
            else
            {
                quantizedBuffer.addEvent(msg, samplePos);
            }
        }

        midiMessages.swapWith(quantizedBuffer);
    }

private:
    static std::vector<int> getScaleIntervals(int scaleType)
    {
        switch (scaleType)
        {
            case Major:          return { 0, 2, 4, 5, 7, 9, 11 };
            case NaturalMinor:   return { 0, 2, 3, 5, 7, 8, 10 };
            case HarmonicMinor:  return { 0, 2, 3, 5, 7, 8, 11 };
            case Dorian:         return { 0, 2, 3, 5, 7, 9, 10 };
            case Mixolydian:     return { 0, 2, 4, 5, 7, 9, 10 };
            case Pentatonic:     return { 0, 2, 4, 7, 9 };
            default:             return {};
        }
    }
};
