#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <array>
#include <vector>
#include <algorithm>

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

    static int quantizeNote(int midiNote, int rootKey, int scaleType)
    {
        if (scaleType == Off) return midiNote;

        int noteInOctave = (midiNote - rootKey) % 12;
        if (noteInOctave < 0) noteInOctave += 12;
        int octaveBase = midiNote - noteInOctave;

        const auto& scaleMask = getScaleMask(scaleType);
        if (scaleMask[static_cast<size_t>(noteInOctave)])
            return midiNote;

        // Find closest in-scale note
        int closestOffset = 0;
        int minDistance = 99;

        for (int offset = -2; offset <= 2; ++offset)
        {
            if (offset == 0) continue;
            int check = (noteInOctave + offset + 12) % 12;
            if (scaleMask[static_cast<size_t>(check)])
            {
                if (std::abs(offset) < minDistance)
                {
                    minDistance = std::abs(offset);
                    closestOffset = offset;
                }
            }
        }

        int finalNote = octaveBase + noteInOctave + closestOffset;
        return juce::jlimit(0, 127, finalNote);
    }

    void processMidi(juce::MidiBuffer& midiMessages, int rootKey, int scaleType, float strumMs, double sampleRate, int numSamples)
    {
        if (scaleType == Off && strumMs <= 0.5f)
            return;

        juce::MidiBuffer outputBuffer;
        int chordNoteCounter = 0;
        int lastSamplePos = -1;

        for (const auto metadata : midiMessages)
        {
            auto msg = metadata.getMessage();
            int samplePos = metadata.samplePosition;

            if (msg.isNoteOn())
            {
                int quantized = quantizeNote(msg.getNoteNumber(), rootKey, scaleType);
                auto newMsg = juce::MidiMessage::noteOn(msg.getChannel(), quantized, msg.getFloatVelocity());

                // Calculate strum offset if multiple notes hit close together
                if (std::abs(samplePos - lastSamplePos) < 64)
                    chordNoteCounter++;
                else
                    chordNoteCounter = 0;

                lastSamplePos = samplePos;

                int strumDelaySamples = static_cast<int>((strumMs * 0.001f * sampleRate * 0.5f) * chordNoteCounter);
                int delayedPos = juce::jmin(numSamples - 1, samplePos + strumDelaySamples);

                outputBuffer.addEvent(newMsg, delayedPos);
            }
            else if (msg.isNoteOff())
            {
                int quantized = quantizeNote(msg.getNoteNumber(), rootKey, scaleType);
                auto newMsg = juce::MidiMessage::noteOff(msg.getChannel(), quantized, msg.getFloatVelocity());
                outputBuffer.addEvent(newMsg, samplePos);
            }
            else
            {
                outputBuffer.addEvent(msg, samplePos);
            }
        }

        midiMessages.swapWith(outputBuffer);
    }

private:
    static std::array<bool, 12> getScaleMask(int scaleType)
    {
        // 0: C, 1: C#, 2: D, 3: D#, 4: E, 5: F, 6: F#, 7: G, 8: G#, 9: A, 10: A#, 11: B
        switch (scaleType)
        {
            case Major:          // 1, 2, 3, 4, 5, 6, 7 (W-W-H-W-W-W-H)
                return { true, false, true, false, true, true, false, true, false, true, false, true };
            case NaturalMinor:   // 1, 2, b3, 4, 5, b6, b7
                return { true, false, true, true, false, true, false, true, true, false, true, false };
            case HarmonicMinor:  // 1, 2, b3, 4, 5, b6, 7
                return { true, false, true, true, false, true, false, true, true, false, false, true };
            case Dorian:         // 1, 2, b3, 4, 5, 6, b7
                return { true, false, true, true, false, true, false, true, false, true, true, false };
            case Mixolydian:     // 1, 2, 3, 4, 5, 6, b7
                return { true, false, true, false, true, true, false, true, false, true, true, false };
            case Pentatonic:     // 1, b3, 4, 5, b7
                return { true, false, false, true, false, true, false, true, false, false, true, false };
            default:
                return { true, true, true, true, true, true, true, true, true, true, true, true };
        }
    }
};
