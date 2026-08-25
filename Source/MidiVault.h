#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "ScaleManager.h"
#include <vector>
#include <string>

class MidiVault
{
public:
    struct VaultItem
    {
        juce::String category;
        juce::String name;
        std::vector<int> pitchOffsets; // 16 or 32 steps
        std::vector<float> velocities;
    };

    MidiVault()
    {
        initVault();
    }

    ~MidiVault() = default;

    const std::vector<VaultItem>& getItems() const { return items; }

    bool exportItemToMidi(int itemIndex, int rootKey, int scaleType, const juce::File& destFile)
    {
        if (itemIndex < 0 || itemIndex >= static_cast<int>(items.size()))
            itemIndex = 0;

        const auto& item = items[static_cast<size_t>(itemIndex)];
        juce::MidiMessageSequence seq;
        double ticksPer16th = 960.0 / 4.0;

        for (size_t i = 0; i < item.pitchOffsets.size(); ++i)
        {
            int off = item.pitchOffsets[i];
            if (off >= -48)
            {
                int note = 60 + rootKey + off;
                note = ScaleManager::quantizeNote(note, rootKey, scaleType);
                note = juce::jlimit(0, 127, note);

                float vel = (i < item.velocities.size()) ? item.velocities[i] : 0.85f;
                double startTick = i * ticksPer16th;
                double endTick = startTick + ticksPer16th * 0.85;

                seq.addEvent(juce::MidiMessage::noteOn(1, note, vel), startTick);
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
    void initVault()
    {
        // 01. Billboard Pop Hooks
        items.push_back({ "01_Pop_Hooks", "Midnight Starlight", 
            { 0, -99, 12, 7, 5, -99, 7, 12, 10, -99, 7, 5, 7, 12, 15, 12 },
            { 0.95f, 0, 0.85f, 0.80f, 0.88f, 0, 0.80f, 0.90f, 0.92f, 0, 0.82f, 0.85f, 0.88f, 0.90f, 0.95f, 0.85f } });

        items.push_back({ "01_Pop_Hooks", "Radio Topline Anthem", 
            { 0, 0, 7, 7, 9, 9, 7, -99, 5, 5, 4, 4, 2, 2, 0, -99 },
            { 0.90f, 0.85f, 0.90f, 0.85f, 0.95f, 0.88f, 0.90f, 0, 0.88f, 0.82f, 0.85f, 0.80f, 0.82f, 0.78f, 0.88f, 0 } });

        // 02. Festival EDM Drops
        items.push_back({ "02_Festival_EDM", "Mainstage Stadium Riff", 
            { 0, 12, 0, 24, 7, -99, 12, 24, 0, 12, 7, 19, 12, -99, 24, 12 },
            { 0.98f, 0.80f, 0.88f, 0.95f, 0.85f, 0, 0.88f, 0.98f, 0.92f, 0.80f, 0.85f, 0.90f, 0.92f, 0, 0.98f, 0.85f } });

        items.push_back({ "02_Festival_EDM", "Future Rave Gallop", 
            { 0, 0, 12, 0, 0, 12, 0, 0, 12, 0, 12, 12, 0, 0, 12, 24 },
            { 0.95f, 0.80f, 0.90f, 0.85f, 0.80f, 0.90f, 0.85f, 0.80f, 0.90f, 0.85f, 0.92f, 0.90f, 0.88f, 0.82f, 0.92f, 0.98f } });

        // 03. Retro Synthwave Riffs
        items.push_back({ "03_Retro_Synthwave", "Neon Highway 80s", 
            { 0, 12, 0, 12, 7, 12, 7, 12, 5, 12, 5, 12, 3, 12, 2, 12 },
            { 0.92f, 0.78f, 0.88f, 0.78f, 0.90f, 0.78f, 0.88f, 0.78f, 0.90f, 0.78f, 0.88f, 0.78f, 0.90f, 0.78f, 0.88f, 0.78f } });

        // 04. Nu-Disco Funk Lines
        items.push_back({ "04_Nu_Disco", "Levitating Funk Stabs", 
            { 0, -99, 12, 0, -99, 12, -99, 0, -99, 12, 12, -99, 0, 12, -99, 0 },
            { 0.95f, 0, 0.85f, 0.90f, 0, 0.85f, 0, 0.90f, 0, 0.88f, 0.88f, 0, 0.92f, 0.85f, 0, 0.95f } });

        // 05. Future Bass Melodies
        items.push_back({ "05_Future_Bass", "Kawaii Shimmer Drop", 
            { 12, 15, 19, 24, -99, 12, 15, 19, 24, -99, 19, 24, 27, 24, 19, 15 },
            { 0.90f, 0.85f, 0.88f, 0.95f, 0, 0.85f, 0.82f, 0.88f, 0.95f, 0, 0.90f, 0.95f, 0.98f, 0.92f, 0.88f, 0.85f } });

        // 06. Emotional Ballads
        items.push_back({ "06_Emotional_Ballads", "Tears In The Club", 
            { 0, -99, -99, -99, 7, -99, -99, -99, 9, -99, -99, -99, 5, -99, 4, 2 },
            { 0.92f, 0, 0, 0, 0.85f, 0, 0, 0, 0.88f, 0, 0, 0, 0.82f, 0, 0.80f, 0.75f } });

        // 07. Club Basslines
        items.push_back({ "07_Club_Basslines", "Slap House Rolling Bass", 
            { 0, -99, 0, -99, 12, -99, 0, 12, -99, 0, -99, 12, 0, -99, 12, -99 },
            { 0.98f, 0, 0.88f, 0, 0.92f, 0, 0.88f, 0.92f, 0, 0.90f, 0, 0.92f, 0.95f, 0, 0.90f, 0 } });
    }

    std::vector<VaultItem> items;
};
