#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "ScaleManager.h"
#include <vector>

class HarmonizerEngine
{
public:
    enum HarmonyMode
    {
        Off = 0,
        Diatonic3rd = 1,
        Stadium5th = 2,
        OctaveSpread = 3,
        PopVocalStack = 4
    };

    HarmonizerEngine() = default;
    ~HarmonizerEngine() = default;

    void processMidi(juce::MidiBuffer& midiMessages, int harmMode, int rootKey, int scaleType)
    {
        if (harmMode == Off)
            return;

        juce::MidiBuffer outBuffer;

        for (const auto metadata : midiMessages)
        {
            auto msg = metadata.getMessage();
            int samplePos = metadata.samplePosition;

            if (msg.isNoteOn() || msg.isNoteOff())
            {
                int baseNote = msg.getNoteNumber();
                float vel = msg.getFloatVelocity();
                bool isNoteOn = msg.isNoteOn();

                // Add original note
                outBuffer.addEvent(msg, samplePos);

                std::vector<int> harmonyOffsets;
                if (harmMode == Diatonic3rd)
                {
                    harmonyOffsets = { 4 }; // 3rd interval
                }
                else if (harmMode == Stadium5th)
                {
                    harmonyOffsets = { 7 }; // 5th interval
                }
                else if (harmMode == OctaveSpread)
                {
                    harmonyOffsets = { -12, 12 }; // Dual octaves
                }
                else if (harmMode == PopVocalStack)
                {
                    harmonyOffsets = { 4, 7, 11 }; // 3rd, 5th, 7th
                }

                for (int offset : harmonyOffsets)
                {
                    int harmNote = baseNote + offset;
                    harmNote = ScaleManager::quantizeNote(harmNote, rootKey, scaleType);
                    harmNote = juce::jlimit(0, 127, harmNote);

                    if (isNoteOn)
                        outBuffer.addEvent(juce::MidiMessage::noteOn(msg.getChannel(), harmNote, vel * 0.88f), samplePos);
                    else
                        outBuffer.addEvent(juce::MidiMessage::noteOff(msg.getChannel(), harmNote, vel), samplePos);
                }
            }
            else
            {
                outBuffer.addEvent(msg, samplePos);
            }
        }

        midiMessages.swapWith(outBuffer);
    }
};
