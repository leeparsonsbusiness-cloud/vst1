#include "AutoMaster.h"

AutoMaster::AutoMaster()
    : oversampling(2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR)
{
}

void AutoMaster::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    oversampling.initProcessing(spec.maximumBlockSize);
    subLockHPF.setHPF(static_cast<float>(sampleRate), 120.0f);
    reset();
}

void AutoMaster::reset()
{
    oversampling.reset();
    smoothedGain = 1.0f;

    lowLP_L.reset(); lowLP_R.reset();
    highLP_L.reset(); highLP_R.reset();

    compLowL = BandCompressor{ -16.0f, 2.5f, 0.0f };
    compLowR = BandCompressor{ -16.0f, 2.5f, 0.0f };
    compMidL = BandCompressor{ -18.0f, 3.0f, 0.0f };
    compMidR = BandCompressor{ -18.0f, 3.0f, 0.0f };
    compHighL = BandCompressor{ -20.0f, 3.5f, 0.0f };
    compHighR = BandCompressor{ -20.0f, 3.5f, 0.0f };

    subLockHPF.reset();
}

void AutoMaster::process(juce::AudioBuffer<float>& buffer, bool active, float intensity)
{
    if (!active || intensity <= 0.001f)
        return;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    if (numSamples == 0 || numChannels == 0)
        return;

    float* LData = buffer.getWritePointer(0);
    float* RData = (numChannels > 1) ? buffer.getWritePointer(1) : nullptr;

    // ----------------------------------------------------
    // 1. Smart Auto-Gain (Real-time RMS Leveler targeting -12 dBFS)
    // ----------------------------------------------------
    float sumSquare = 0.0f;
    for (int i = 0; i < numSamples; ++i)
    {
        float sampleL = LData[i];
        float sampleR = RData ? RData[i] : sampleL;
        sumSquare += sampleL * sampleL + sampleR * sampleR;
    }

    float rms = std::sqrt(sumSquare / static_cast<float>(numSamples * numChannels));
    float targetLinear = 0.251f; // -12 dBFS
    float desiredGain = targetLinear / (rms + 1e-4f);
    desiredGain = juce::jlimit(0.5f, 2.5f, desiredGain);
    
    // Morph auto-gain intensity
    float effectiveGainTarget = 1.0f + (desiredGain - 1.0f) * intensity;
    smoothedGain = smoothedGain * 0.95f + effectiveGainTarget * 0.05f;

    for (int i = 0; i < numSamples; ++i)
    {
        LData[i] *= smoothedGain;
        if (RData)
            RData[i] *= smoothedGain;
    }

    // ----------------------------------------------------
    // 2. 3-Band OTT Compressor & Sub-Lock
    // ----------------------------------------------------
    float alpha150 = std::exp(-2.0f * juce::MathConstants<float>::pi * 150.0f / static_cast<float>(sampleRate));
    float alpha2500 = std::exp(-2.0f * juce::MathConstants<float>::pi * 2500.0f / static_cast<float>(sampleRate));

    for (int i = 0; i < numSamples; ++i)
    {
        float inL = LData[i];
        float inR = RData ? RData[i] : inL;

        // Split bands
        float lowL = lowLP_L.processLP(inL, alpha150);
        float lowR = lowLP_R.processLP(inR, alpha150);

        float midL = highLP_L.processLP(inL - lowL, alpha2500);
        float midR = highLP_R.processLP(inR - lowR, alpha2500);

        float highL = inL - lowL - midL;
        float highR = inR - lowR - midR;

        // Compress bands
        float cLowL = compLowL.processSample(lowL, static_cast<float>(sampleRate));
        float cLowR = compLowR.processSample(lowR, static_cast<float>(sampleRate));

        float cMidL = compMidL.processSample(midL, static_cast<float>(sampleRate));
        float cMidR = compMidR.processSample(midR, static_cast<float>(sampleRate));

        float cHighL = compHighL.processSample(highL, static_cast<float>(sampleRate));
        float cHighR = compHighR.processSample(highR, static_cast<float>(sampleRate));

        // Recombine bands according to intensity
        float processedL = inL * (1.0f - intensity) + (cLowL + cMidL + cHighL) * intensity;
        float processedR = inR * (1.0f - intensity) + (cLowR + cMidR + cHighR) * intensity;

        // Sub-Lock (Hard mono collapse < 120 Hz)
        if (numChannels >= 2)
        {
            float midSig = (processedL + processedR) * 0.5f;
            float sideSig = (processedL - processedR) * 0.5f;
            float sideFiltered = subLockHPF.process(sideSig);
            processedL = midSig + sideFiltered;
            processedR = midSig - sideFiltered;
        }

        LData[i] = processedL;
        if (RData)
            RData[i] = processedR;
    }

    // ----------------------------------------------------
    // 3. 4x Oversampled Soft-Clipper (Asymmetric tanh saturation)
    // ----------------------------------------------------
    if (numChannels >= 2)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::AudioBlock<float> oversampledBlock = oversampling.processSamplesUp(block);

        float* osL = oversampledBlock.getChannelPointer(0);
        float* osR = oversampledBlock.getChannelPointer(1);
        size_t osNumSamples = oversampledBlock.getNumSamples();

        for (size_t i = 0; i < osNumSamples; ++i)
        {
            float xL = osL[i] * (1.0f + 0.5f * intensity);
            float xR = osR[i] * (1.0f + 0.5f * intensity);

            // Asymmetric soft clip transfer function: tanh(x + 0.1*x^2)
            osL[i] = std::tanh(xL + 0.1f * xL * xL);
            osR[i] = std::tanh(xR + 0.1f * xR * xR);
        }

        oversampling.processSamplesDown(block);
    }
}
