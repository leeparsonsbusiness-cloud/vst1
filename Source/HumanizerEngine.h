#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <random>
#include <algorithm>

class HumanizerEngine
{
public:
    HumanizerEngine() = default;
    ~HumanizerEngine() = default;

    void processMidi(juce::MidiBuffer& midiMessages, float humanizeAmt, double sampleRate, int numSamples)
    {
        if (humanizeAmt < 0.001f)
            return;

        juce::MidiBuffer outBuffer;
        juce::Random rand;

        int maxJitterSamples = static_cast<int>((humanizeAmt * 0.012f) * sampleRate); // Up to 12ms jitter

        for (const auto metadata : midiMessages)
        {
            auto msg = metadata.getMessage();
            int samplePos = metadata.samplePosition;

            if (msg.isNoteOn())
            {
                // Add micro-timing delay
                int jitter = (maxJitterSamples > 0) ? rand.nextInt(maxJitterSamples) : 0;
                int newPos = std::min(numSamples - 1, samplePos + jitter);

                // Modulate velocity naturally
                float velOffset = (rand.nextFloat() * 0.20f - 0.10f) * humanizeAmt;
                float newVel = juce::jlimit(0.2f, 1.0f, msg.getFloatVelocity() + velOffset);

                outBuffer.addEvent(juce::MidiMessage::noteOn(msg.getChannel(), msg.getNoteNumber(), newVel), newPos);
            }
            else
            {
                outBuffer.addEvent(msg, samplePos);
            }
        }

        midiMessages.swapWith(outBuffer);
    }
};
