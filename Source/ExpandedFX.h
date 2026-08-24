#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>

class ExpandedFX
{
public:
    ExpandedFX();
    ~ExpandedFX() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    void process(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& apvts);

private:
    struct AllPassFilter
    {
        float a1 = 0.0f;
        float zm1 = 0.0f;

        void setDelay(float delay)
        {
            a1 = (1.0f - delay) / (1.0f + delay);
        }

        float process(float in)
        {
            float out = -a1 * in + zm1;
            zm1 = in + a1 * out;
            return out;
        }

        void reset() { zm1 = 0.0f; }
    };

    double sampleRate = 44100.0;
    float lfoPhase = 0.0f;

    // Phaser All-Pass filters (4 stages for L & R)
    AllPassFilter phaserL[4];
    AllPassFilter phaserR[4];
    float phaserFeedbackL = 0.0f;
    float phaserFeedbackR = 0.0f;

    // Bitcrusher downsample hold states
    int sampleHoldCounterL = 0, sampleHoldCounterR = 0;
    float heldSampleL = 0.0f, heldSampleR = 0.0f;
};
