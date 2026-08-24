#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <algorithm>
#include <random>

class Arpeggiator
{
public:
    enum Mode { Off = 0, Up, Down, UpDown, RandomMode, Sequence };

    struct Step
    {
        bool active = true;
        int pitchOffset = 0; // -24 to +24 semitones
        float velocity = 0.85f;
        float gate = 0.8f;   // 0.1 to 1.0
    };

    Arpeggiator()
    {
        steps.resize(16);
        for (int i = 0; i < 16; ++i)
        {
            steps[static_cast<size_t>(i)].active = (i % 2 == 0);
            steps[static_cast<size_t>(i)].pitchOffset = (i % 4 == 0) ? 0 : ((i % 4 == 2) ? 12 : 7);
            steps[static_cast<size_t>(i)].velocity = (i % 4 == 0) ? 0.95f : 0.80f;
            steps[static_cast<size_t>(i)].gate = 0.85f;
        }
    }

    ~Arpeggiator() = default;

    void setMode(int m) { mode = static_cast<Mode>(juce::jlimit(0, 5, m)); }
    void setRate(int r) { rateIndex = juce::jlimit(0, 5, r); }
    void setOctaves(int oct) { octaves = juce::jlimit(1, 4, oct); }
    void setSwing(float s) { swing = juce::jlimit(0.0f, 0.75f, s); }
    void setGate(float g) { masterGate = juce::jlimit(0.1f, 1.0f, g); }

    Step& getStep(int index) { return steps[static_cast<size_t>(juce::jlimit(0, 15, index))]; }
    const Step& getStep(int index) const { return steps[static_cast<size_t>(juce::jlimit(0, 15, index))]; }

    void processMidiBlock(juce::MidiBuffer& midiMessages, double sampleRate, double bpm, int numSamples)
    {
        if (mode == Off)
        {
            notesHeld.clear();
            currentStep = 0;
            sampleCounter = 0;
            return;
        }

        // Intercept held MIDI notes
        for (const auto metadata : midiMessages)
        {
            auto msg = metadata.getMessage();
            if (msg.isNoteOn())
            {
                int note = msg.getNoteNumber();
                if (std::find(notesHeld.begin(), notesHeld.end(), note) == notesHeld.end())
                    notesHeld.push_back(note);
            }
            else if (msg.isNoteOff())
            {
                int note = msg.getNoteNumber();
                notesHeld.erase(std::remove(notesHeld.begin(), notesHeld.end(), note), notesHeld.end());
            }
        }

        if (notesHeld.empty())
        {
            if (lastTriggeredNote != -1)
            {
                midiMessages.addEvent(juce::MidiMessage::noteOff(1, lastTriggeredNote, 0.0f), 0);
                lastTriggeredNote = -1;
            }
            sampleCounter = 0;
            currentStep = 0;
            return;
        }

        juce::MidiBuffer arpeggiatedBuffer;
        
        // Calculate step duration in samples based on rate division
        double divisionFactor = 0.25; // default 1/16
        switch (rateIndex)
        {
            case 0: divisionFactor = 1.0; break;   // 1/4
            case 1: divisionFactor = 0.5; break;   // 1/8
            case 2: divisionFactor = 0.25; break;  // 1/16
            case 3: divisionFactor = 0.125; break; // 1/32
            case 4: divisionFactor = 0.5 / 1.5; break; // 1/8T
            case 5: divisionFactor = 0.25 / 1.5; break; // 1/16T
        }

        double secondsPerBeat = 60.0 / std::max(20.0, bpm);
        double stepDurationSeconds = secondsPerBeat * divisionFactor;
        int stepSamples = static_cast<int>(stepDurationSeconds * sampleRate);
        if (stepSamples < 10) stepSamples = 1000;

        for (int s = 0; s < numSamples; ++s)
        {
            if (sampleCounter == 0)
            {
                if (lastTriggeredNote != -1)
                {
                    arpeggiatedBuffer.addEvent(juce::MidiMessage::noteOff(1, lastTriggeredNote, 0.0f), s);
                    lastTriggeredNote = -1;
                }

                const auto& step = steps[static_cast<size_t>(currentStep % 16)];
                if (step.active && !notesHeld.empty())
                {
                    int noteIndex = 0;
                    size_t numNotes = notesHeld.size();

                    if (mode == Up)
                        noteIndex = static_cast<int>((currentStep / 16) % static_cast<int>(numNotes));
                    else if (mode == Down)
                        noteIndex = (static_cast<int>(numNotes) - 1) - ((currentStep / 16) % static_cast<int>(numNotes));
                    else if (mode == UpDown)
                    {
                        int cycle = static_cast<int>((currentStep / 16) % static_cast<int>(numNotes * 2));
                        noteIndex = (cycle < static_cast<int>(numNotes)) ? cycle : (static_cast<int>(numNotes * 2 - 1) - cycle);
                    }
                    else if (mode == RandomMode)
                        noteIndex = rand() % static_cast<int>(numNotes);
                    else
                        noteIndex = currentStep % static_cast<int>(numNotes);

                    int octShift = (currentStep / static_cast<int>(numNotes)) % octaves;
                    int baseNote = notesHeld[static_cast<size_t>(noteIndex)] + octShift * 12;
                    int finalNote = juce::jlimit(0, 127, baseNote + step.pitchOffset);

                    float finalVel = juce::jlimit(0.05f, 1.0f, step.velocity);
                    arpeggiatedBuffer.addEvent(juce::MidiMessage::noteOn(1, finalNote, finalVel), s);
                    lastTriggeredNote = finalNote;
                }
            }

            sampleCounter++;
            if (sampleCounter >= stepSamples)
            {
                sampleCounter = 0;
                currentStep = (currentStep + 1) % 16;
            }
        }

        midiMessages.swapWith(arpeggiatedBuffer);
    }

private:
    Mode mode = Off;
    int rateIndex = 2; // 1/16
    int octaves = 1;
    float swing = 0.0f;
    float masterGate = 0.8f;

    std::vector<Step> steps;
    std::vector<int> notesHeld;

    int currentStep = 0;
    int sampleCounter = 0;
    int lastTriggeredNote = -1;
};
