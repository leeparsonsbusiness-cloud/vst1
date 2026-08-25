#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <algorithm>
#include <cmath>

class AutoBassEngine
{
public:
    enum BassPattern
    {
        Off = 0,
        Rolling16ths = 1,
        Offbeat8ths = 2,
        FourOnFloor = 3,
        NuDiscoSync = 4
    };

    AutoBassEngine() = default;
    ~AutoBassEngine() = default;

    void setSampleRate(double sRate) { sampleRate = sRate; }
    void setBpm(double bpm) { hostBpm = bpm; }

    void processMidi(juce::MidiBuffer& midiMessages, int patternStyle, int numSamples)
    {
        if (patternStyle == Off)
        {
            if (lastBassNote != -1)
            {
                midiMessages.addEvent(juce::MidiMessage::noteOff(1, lastBassNote, 0.0f), 0);
                lastBassNote = -1;
            }
            stepCounter = 0;
            sampleInStep = 0;
            return;
        }

        // Track root notes from incoming melody (upper notes)
        for (const auto metadata : midiMessages)
        {
            auto msg = metadata.getMessage();
            if (msg.isNoteOn() && msg.getNoteNumber() >= 36)
            {
                int noteInOctave = msg.getNoteNumber() % 12;
                currentRootNote = 24 + noteInOctave; // Octave 1 (C1 base: 24)
                melodyIsActive = true;
            }
            else if (msg.isNoteOff() && msg.getNoteNumber() % 12 == currentRootNote % 12)
            {
                // melody release handled naturally
            }
        }

        if (!melodyIsActive) return;

        double secondsPerBeat = 60.0 / std::max(20.0, hostBpm);
        double secondsPer16th = secondsPerBeat * 0.25;
        int samplesPer16th = static_cast<int>(secondsPer16th * sampleRate);
        if (samplesPer16th < 10) samplesPer16th = 1000;

        for (int s = 0; s < numSamples; ++s)
        {
            if (sampleInStep == 0)
            {
                if (lastBassNote != -1)
                {
                    midiMessages.addEvent(juce::MidiMessage::noteOff(1, lastBassNote, 0.0f), s);
                    lastBassNote = -1;
                }

                int sub16 = stepCounter % 16;
                bool shouldTrigger = false;
                int octaveOffset = 0;
                float vel = 0.90f;

                if (patternStyle == Rolling16ths)
                {
                    shouldTrigger = true;
                    octaveOffset = (sub16 % 4 == 0) ? 0 : 12;
                    vel = (sub16 % 4 == 0) ? 0.95f : 0.80f;
                }
                else if (patternStyle == Offbeat8ths)
                {
                    shouldTrigger = (sub16 % 4 == 2); // Offbeat 8th note
                    octaveOffset = 0;
                    vel = 0.95f;
                }
                else if (patternStyle == FourOnFloor)
                {
                    shouldTrigger = (sub16 % 4 == 0); // Quarter notes
                    octaveOffset = 0;
                    vel = 0.95f;
                }
                else if (patternStyle == NuDiscoSync)
                {
                    static const int discoPattern[16] = { 0, -1, 12, 0, -1, 12, 0, -1, 12, 12, 0, -1, 12, 0, 12, -1 };
                    int off = discoPattern[sub16];
                    if (off >= 0)
                    {
                        shouldTrigger = true;
                        octaveOffset = off;
                        vel = (off == 0) ? 0.95f : 0.82f;
                    }
                }

                if (shouldTrigger)
                {
                    int note = currentRootNote + octaveOffset;
                    note = juce::jlimit(12, 60, note);
                    midiMessages.addEvent(juce::MidiMessage::noteOn(1, note, vel), s);
                    lastBassNote = note;
                }
            }

            sampleInStep++;
            if (sampleInStep >= samplesPer16th)
            {
                sampleInStep = 0;
                stepCounter = (stepCounter + 1) % 16;
            }
        }
    }

private:
    double sampleRate = 44100.0;
    double hostBpm = 120.0;
    int currentRootNote = 24; // C1
    bool melodyIsActive = false;
    int stepCounter = 0;
    int sampleInStep = 0;
    int lastBassNote = -1;
};
