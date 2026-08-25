#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <array>
#include <string>
#include "ScaleManager.h"

class ChordProgressionEngine
{
public:
    enum ProgressionStyle
    {
        Off = 0,
        PopAxis = 1,       // I - V - vi - IV
        DarkEdm = 2,       // i - VI - III - VII
        EmotionalPop = 3,  // vi - IV - I - V
        FutureBass = 4,    // IV - I - vi - V
        Synthwave80s = 5,  // i - VII - VI - VII
        DooWop50s = 6,     // I - vi - IV - V
        RoyalRoad = 7,     // IV - V - iii - vi
        KeshaDance = 8     // I - bVII - IV - I
    };

    struct ChordVoicing
    {
        int rootOffset; // Semitones from root key
        std::vector<int> intervals; // e.g. { 0, 4, 7, 12 } for Major triad with octave
    };

    ChordProgressionEngine() = default;
    ~ChordProgressionEngine() = default;

    void processMidi(juce::MidiBuffer& midiMessages, int progIndex, int rootKey, int scaleType, int /*numSamples*/)
    {
        if (progIndex == Off)
            return;

        const auto& chords = getProgressionChords(progIndex);
        if (chords.empty()) return;

        juce::MidiBuffer outBuffer;

        for (const auto metadata : midiMessages)
        {
            auto msg = metadata.getMessage();
            int samplePos = metadata.samplePosition;

            if (msg.isNoteOn())
            {
                int inputNote = msg.getNoteNumber();
                int chordStep = (inputNote) % static_cast<int>(chords.size());
                if (chordStep < 0) chordStep += static_cast<int>(chords.size());

                const auto& chord = chords[static_cast<size_t>(chordStep)];
                int chordRoot = 48 + rootKey + chord.rootOffset; // Octave 3 base
                float vel = msg.getFloatVelocity();

                // Add Bass Root (Octave 1/2)
                int bassNote = chordRoot - 12;
                bassNote = juce::jlimit(0, 127, bassNote);
                outBuffer.addEvent(juce::MidiMessage::noteOn(msg.getChannel(), bassNote, vel * 0.95f), samplePos);

                // Add Chord Voicing Notes
                for (int interval : chord.intervals)
                {
                    int note = chordRoot + interval;
                    note = ScaleManager::quantizeNote(note, rootKey, scaleType);
                    note = juce::jlimit(0, 127, note);
                    outBuffer.addEvent(juce::MidiMessage::noteOn(msg.getChannel(), note, vel), samplePos);
                }
            }
            else if (msg.isNoteOff())
            {
                int inputNote = msg.getNoteNumber();
                int chordStep = (inputNote) % static_cast<int>(chords.size());
                if (chordStep < 0) chordStep += static_cast<int>(chords.size());

                const auto& chord = chords[static_cast<size_t>(chordStep)];
                int chordRoot = 48 + rootKey + chord.rootOffset;

                int bassNote = chordRoot - 12;
                bassNote = juce::jlimit(0, 127, bassNote);
                outBuffer.addEvent(juce::MidiMessage::noteOff(msg.getChannel(), bassNote), samplePos);

                for (int interval : chord.intervals)
                {
                    int note = chordRoot + interval;
                    note = ScaleManager::quantizeNote(note, rootKey, scaleType);
                    note = juce::jlimit(0, 127, note);
                    outBuffer.addEvent(juce::MidiMessage::noteOff(msg.getChannel(), note), samplePos);
                }
            }
            else
            {
                outBuffer.addEvent(msg, samplePos);
            }
        }

        midiMessages.swapWith(outBuffer);
    }

    bool exportProgressionMidi(int progIndex, int rootKey, const juce::File& destFile)
    {
        if (progIndex == Off) progIndex = PopAxis;
        const auto& chords = getProgressionChords(progIndex);
        if (chords.empty()) return false;

        juce::MidiMessageSequence seq;
        double ticksPerBar = 960.0 * 4.0; // 4/4 bar

        for (size_t bar = 0; bar < chords.size(); ++bar)
        {
            const auto& chord = chords[bar];
            int chordRoot = 48 + rootKey + chord.rootOffset;
            double startTick = bar * ticksPerBar;
            double endTick = startTick + ticksPerBar * 0.95;

            // Bass note
            int bass = chordRoot - 12;
            seq.addEvent(juce::MidiMessage::noteOn(1, bass, 0.95f), startTick);
            seq.addEvent(juce::MidiMessage::noteOff(1, bass, 0.0f), endTick);

            // Voicing notes
            for (int interval : chord.intervals)
            {
                int note = chordRoot + interval;
                seq.addEvent(juce::MidiMessage::noteOn(1, note, 0.85f), startTick);
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
    static std::vector<ChordVoicing> getProgressionChords(int progIndex)
    {
        switch (progIndex)
        {
            case PopAxis: // I - V - vi - IV (C - G - Am - F)
                return {
                    { 0, { 0, 4, 7, 12 } },   // I Major
                    { 7, { -5, -1, 2, 7 } },  // V Major
                    { 9, { -3, 0, 4, 9 } },   // vi Minor
                    { 5, { -7, -3, 0, 5 } }   // IV Major
                };

            case DarkEdm: // i - VI - III - VII (Am - F - C - G)
                return {
                    { 0, { 0, 3, 7, 12 } },   // i Minor
                    { 8, { -4, 0, 3, 8 } },   // VI Major
                    { 3, { -9, -5, -2, 3 } }, // III Major
                    { 10, { -2, 2, 5, 10 } }  // VII Major
                };

            case EmotionalPop: // vi - IV - I - V (Am - F - C - G)
                return {
                    { 9, { 0, 4, 9, 12 } },
                    { 5, { 0, 5, 9, 12 } },
                    { 0, { 0, 4, 7, 12 } },
                    { 7, { -1, 2, 7, 11 } }
                };

            case FutureBass: // IV - I - vi - V (F - C - Am - G)
                return {
                    { 5, { 0, 4, 7, 11, 14 } }, // Major 9th
                    { 0, { 0, 4, 7, 11 } },     // Major 7th
                    { 9, { 0, 3, 7, 10, 14 } }, // Minor 9th
                    { 7, { -1, 2, 7, 10 } }     // Dominant 7th
                };

            case Synthwave80s: // i - VII - VI - VII (Am - G - F - G)
                return {
                    { 0, { 0, 3, 7, 12 } },
                    { 10, { -2, 2, 5, 10 } },
                    { 8, { -4, 0, 3, 8 } },
                    { 10, { -2, 2, 5, 10 } }
                };

            case DooWop50s: // I - vi - IV - V
                return {
                    { 0, { 0, 4, 7, 12 } },
                    { 9, { -3, 0, 4, 9 } },
                    { 5, { -7, -3, 0, 5 } },
                    { 7, { -5, -1, 2, 7 } }
                };

            case RoyalRoad: // IV - V - iii - vi (F - G - Em - Am)
                return {
                    { 5, { 0, 4, 7, 12 } },
                    { 7, { 2, 7, 11, 14 } },
                    { 4, { -1, 4, 7, 11 } },
                    { 9, { 0, 4, 9, 12 } }
                };

            case KeshaDance: // I - bVII - IV - I (C - Bb - F - C)
                return {
                    { 0, { 0, 4, 7, 12 } },
                    { 10, { -2, 2, 5, 10 } },
                    { 5, { 0, 5, 9, 12 } },
                    { 0, { 0, 4, 7, 12 } }
                };

            default:
                return {};
        }
    }
};
