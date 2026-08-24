#include "ExpandedFX.h"

ExpandedFX::ExpandedFX()
{
}

void ExpandedFX::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    reset();
}

void ExpandedFX::reset()
{
    lfoPhase = 0.0f;
    phaserFeedbackL = phaserFeedbackR = 0.0f;
    sampleHoldCounterL = sampleHoldCounterR = 0;
    heldSampleL = heldSampleR = 0.0f;

    for (int i = 0; i < 4; ++i)
    {
        phaserL[i].reset();
        phaserR[i].reset();
    }
}

void ExpandedFX::process(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& apvts)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    if (numSamples == 0 || numChannels == 0)
        return;

    // Load parameter values safely
    bool bitcrushActive = apvts.getRawParameterValue("bitcrush_active")->load() > 0.5f;
    int bitDepth = static_cast<int>(apvts.getRawParameterValue("bitcrush_bits")->load());
    int downsample = static_cast<int>(apvts.getRawParameterValue("bitcrush_downsample")->load());

    bool wavefoldActive = apvts.getRawParameterValue("wavefold_active")->load() > 0.5f;
    float wavefoldDrive = apvts.getRawParameterValue("wavefold_drive")->load();

    bool phaserActive = apvts.getRawParameterValue("phaser_active")->load() > 0.5f;
    float phaserRate = apvts.getRawParameterValue("phaser_rate")->load();
    float phaserDepth = apvts.getRawParameterValue("phaser_depth")->load();
    float phaserFeedback = apvts.getRawParameterValue("phaser_feedback")->load();
    float phaserMix = apvts.getRawParameterValue("phaser_mix")->load();

    float* LData = buffer.getWritePointer(0);
    float* RData = (numChannels > 1) ? buffer.getWritePointer(1) : nullptr;

    for (int i = 0; i < numSamples; ++i)
    {
        float inL = LData[i];
        float inR = RData ? RData[i] : inL;

        // ----------------------------------------------------
        // 1. Bitcrusher (Downsampling & Bit-Quantization)
        // ----------------------------------------------------
        if (bitcrushActive)
        {
            if (sampleHoldCounterL == 0)
            {
                // Quantize bit depth
                float stepCount = std::pow(2.0f, static_cast<float>(juce::jlimit(1, 16, bitDepth)));
                heldSampleL = std::round(inL * stepCount) / stepCount;
            }
            inL = heldSampleL;
            sampleHoldCounterL = (sampleHoldCounterL + 1) % std::max(1, downsample);

            if (RData)
            {
                if (sampleHoldCounterR == 0)
                {
                    float stepCount = std::pow(2.0f, static_cast<float>(juce::jlimit(1, 16, bitDepth)));
                    heldSampleR = std::round(inR * stepCount) / stepCount;
                }
                inR = heldSampleR;
                sampleHoldCounterR = (sampleHoldCounterR + 1) % std::max(1, downsample);
            }
        }

        // ----------------------------------------------------
        // 2. Wavefolder Distortion
        // ----------------------------------------------------
        if (wavefoldActive && wavefoldDrive > 1.01f)
        {
            inL = std::sin(inL * wavefoldDrive);
            inR = std::sin(inR * wavefoldDrive);
        }

        // ----------------------------------------------------
        // 3. Phaser / Flanger Engine
        // ----------------------------------------------------
        if (phaserActive && phaserMix > 0.001f)
        {
            lfoPhase += (2.0f * juce::MathConstants<float>::pi * phaserRate / static_cast<float>(sampleRate));
            if (lfoPhase >= 2.0f * juce::MathConstants<float>::pi)
                lfoPhase -= 2.0f * juce::MathConstants<float>::pi;

            float lfoVal = (std::sin(lfoPhase) + 1.0f) * 0.5f; // 0 to 1
            float dL = 0.1f + lfoVal * 0.8f * phaserDepth;
            float dR = 0.1f + (1.0f - lfoVal) * 0.8f * phaserDepth;

            for (int k = 0; k < 4; ++k)
            {
                phaserL[k].setDelay(dL);
                phaserR[k].setDelay(dR);
            }

            float pInL = inL + phaserFeedbackL * phaserFeedback;
            float pInR = inR + phaserFeedbackR * phaserFeedback;

            float apL = pInL;
            float apR = pInR;

            for (int k = 0; k < 4; ++k)
            {
                apL = phaserL[k].process(apL);
                apR = phaserR[k].process(apR);
            }

            phaserFeedbackL = apL;
            phaserFeedbackR = apR;

            inL = inL * (1.0f - phaserMix) + apL * phaserMix;
            inR = inR * (1.0f - phaserMix) + apR * phaserMix;
        }

        LData[i] = inL;
        if (RData)
            RData[i] = inR;
    }
}
