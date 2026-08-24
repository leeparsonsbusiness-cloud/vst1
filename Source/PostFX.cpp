#include "PostFX.h"

PostFX::PostFX()
{
}

void PostFX::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    
    // Prepare Chorus
    chorus.prepare(spec);

    // Prepare Reverb
    reverb.prepare(spec);

    // 2 seconds max delay buffer at current sample rate
    delayBufferLength = static_cast<int>(sampleRate * 2.0);
    delayBufferL.setSize(1, delayBufferLength);
    delayBufferR.setSize(1, delayBufferLength);
    delayBufferL.clear();
    delayBufferR.clear();

    delayWritePosL = 0;
    delayWritePosR = 0;

    reset();
}

void PostFX::reset()
{
    chorus.reset();
    reverb.reset();
    
    delayBufferL.clear();
    delayBufferR.clear();
    delayWritePosL = 0;
    delayWritePosR = 0;

    trashEQ_L.reset();
    trashEQ_R.reset();
    glossEQ_L.reset();
    glossEQ_R.reset();
    
    crossoverLPF_L.reset();
    crossoverLPF_R.reset();
    envLowL = 0.0f; envLowR = 0.0f;
    envHighL = 0.0f; envHighR = 0.0f;

    sideHPF.reset();
    delayColorFilterL.reset();
    delayColorFilterR.reset();
    dropHPF_L.reset();
    dropHPF_R.reset();

    internalTime = 0.0;
}

void PostFX::process(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& apvts, 
                     double playHeadTimeSeconds, double bpm, double ppqPosition, bool isHostPlaying)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    if (numSamples == 0 || numChannels == 0)
        return;

    juce::ignoreUnused(playHeadTimeSeconds);

    // Load parameters
    float drive = apvts.getRawParameterValue("fx_drive")->load();
    float chorusRate = apvts.getRawParameterValue("fx_chorus_rate")->load();
    float chorusDepth = apvts.getRawParameterValue("fx_chorus_depth")->load();
    float chorusMix = apvts.getRawParameterValue("fx_chorus_mix")->load();

    float delayTimeMs = apvts.getRawParameterValue("fx_delay_time")->load();
    float delayFeedback = apvts.getRawParameterValue("fx_delay_feedback")->load();
    float delayMix = apvts.getRawParameterValue("fx_delay_mix")->load();
    float delayColor = apvts.getRawParameterValue("fx_delay_color")->load();
    
    float reverbDecay = apvts.getRawParameterValue("fx_reverb_decay")->load();
    float reverbDamping = apvts.getRawParameterValue("fx_reverb_damping")->load();
    float reverbWidth = apvts.getRawParameterValue("fx_reverb_width")->load();
    float reverbMix = apvts.getRawParameterValue("fx_reverb_mix")->load();

    float trashGlossX = apvts.getRawParameterValue("trash_gloss_x")->load();
    float trashGlossY = apvts.getRawParameterValue("trash_gloss_y")->load();
    bool monoMakerActive = apvts.getRawParameterValue("mono_maker_active")->load() > 0.5f;
    float monoMakerFreq = apvts.getRawParameterValue("mono_maker_frequency")->load();

    bool pumpActive = apvts.getRawParameterValue("pump_active")->load() > 0.5f;
    int pumpDivision = static_cast<int>(apvts.getRawParameterValue("pump_division")->load());
    float pumpDepth = apvts.getRawParameterValue("pump_depth")->load();
    float pumpCurve = apvts.getRawParameterValue("pump_curve")->load();

    float macroDrop = apvts.getRawParameterValue("macro_drop")->load();
    float macroGrit = apvts.getRawParameterValue("macro_grit")->load();
    float macroSpace = apvts.getRawParameterValue("macro_space")->load();
    float macroWidth = apvts.getRawParameterValue("macro_width")->load();

    // ----------------------------------------------------
    // MACRO TIE-INS
    // ----------------------------------------------------
    float D = macroDrop;
    float G = macroGrit;
    float S = macroSpace;
    float W = macroWidth;

    // Link Drop HPF sweep
    float dropCutoff = 20.0f + 580.0f * D;
    dropHPF_L.setHPF(static_cast<float>(sampleRate), dropCutoff, 0.707f);
    dropHPF_R.setHPF(static_cast<float>(sampleRate), dropCutoff, 0.707f);

    // Link delay feedback & wet levels
    float effectiveDelayMix = delayMix + (0.7f - delayMix) * D;
    float effectiveDelayFeedback = delayFeedback + (0.7f - delayFeedback) * D;
    effectiveDelayMix = effectiveDelayMix + S * (1.0f - effectiveDelayMix);

    // Link reverb wet levels
    float effectiveReverbMix = reverbMix + (0.7f - reverbMix) * D;
    effectiveReverbMix = effectiveReverbMix + S * (1.0f - effectiveReverbMix);

    // Link grit overdrive and trash X-axis
    float effectiveDrive = drive + G * 5.0f;
    float effectiveTrashX = trashGlossX + G * (1.0f - trashGlossX);

    // Link chorus width and depth
    float effectiveChorusDepth = chorusDepth + (0.3f * W) + (0.3f * D);
    float effectiveChorusMix = chorusMix + (0.3f * W) + (0.3f * D);
    effectiveChorusDepth = juce::jlimit(0.0f, 1.0f, effectiveChorusDepth);
    effectiveChorusMix = juce::jlimit(0.0f, 1.0f, effectiveChorusMix);

    float effectiveGlossY = trashGlossY;

    // ----------------------------------------------------
    // Update filters
    // ----------------------------------------------------
    // Trash EQ Peak filter at 2.5 kHz
    float trashPeakBoost = effectiveTrashX * 9.0f; // Boost up to +9dB
    trashEQ_L.setPeakEQ(static_cast<float>(sampleRate), 2500.0f, 1.0f, trashPeakBoost);
    trashEQ_R.setPeakEQ(static_cast<float>(sampleRate), 2500.0f, 1.0f, trashPeakBoost);

    // Gloss EQ High shelf filter at 14 kHz
    float glossShelfBoost = effectiveGlossY * 4.0f; // Boost up to +4dB
    glossEQ_L.setHighShelf(static_cast<float>(sampleRate), 14000.0f, glossShelfBoost);
    glossEQ_R.setHighShelf(static_cast<float>(sampleRate), 14000.0f, glossShelfBoost);

    // Side HPF for Mono Maker using variable monoMakerFreq
    sideHPF.setHPF(static_cast<float>(sampleRate), monoMakerFreq, 0.707f);

    float alphaCrossover = std::exp(-2.0f * juce::MathConstants<float>::pi * 500.0f / static_cast<float>(sampleRate));
    float decayCoeff = std::exp(-1.0f / (0.035f * static_cast<float>(sampleRate))); // 35ms RMS tracker window

    // Reverb configuration using expanded parameters and macro modifiers
    juce::dsp::Reverb::Parameters reverbParams;
    reverbParams.wetLevel = effectiveReverbMix;
    reverbParams.dryLevel = 1.0f - effectiveReverbMix * 0.5f;
    reverbParams.roomSize = reverbDecay + (0.95f - reverbDecay) * D; // morphs during drop builds
    reverbParams.damping = reverbDamping;
    reverbParams.width = reverbWidth;
    reverb.setParameters(reverbParams);

    // Sidechain pumping beat tracker
    double ppqRate = bpm / 60.0;
    double divBeats = (pumpDivision == 0) ? 1.0 : ((pumpDivision == 1) ? 0.5 : 0.25);
    
    // Fallback sync tracking
    if (!isHostPlaying)
    {
        internalTime += static_cast<double>(numSamples) / sampleRate;
        ppqPosition = internalTime * ppqRate;
    }

    // ----------------------------------------------------
    // Sample Processing Loop
    // ----------------------------------------------------
    float* leftData = buffer.getWritePointer(0);
    float* rightData = (numChannels > 1) ? buffer.getWritePointer(1) : nullptr;

    for (int s = 0; s < numSamples; ++s)
    {
        float inL = leftData[s];
        float inR = rightData ? rightData[s] : inL;

        // 1. Soft-Clipper Saturation & Asymmetric Overdrive (Drive + Trash X-axis)
        if (effectiveDrive > 1.01f || effectiveTrashX > 0.001f)
        {
            // Trash X overdrive: apply soft-clip + asymmetric drive
            float normScale = 1.0f / std::tanh(effectiveDrive);
            
            // Channel L
            float offsetL = 0.15f * effectiveTrashX;
            float driveL = (inL + offsetL) * effectiveDrive;
            float clipL = std::tanh(driveL) * normScale - std::tanh(offsetL) * normScale;
            
            // Channel R
            float offsetR = 0.15f * effectiveTrashX;
            float driveR = (inR + offsetR) * effectiveDrive;
            float clipR = std::tanh(driveR) * normScale - std::tanh(offsetR) * normScale;

            inL = juce::jmap(effectiveTrashX * 0.5f, inL, clipL);
            inR = juce::jmap(effectiveTrashX * 0.5f, inR, clipR);
        }

        // 2. Trash 2.5 kHz EQ Peak Boost
        if (effectiveTrashX > 0.001f)
        {
            inL = trashEQ_L.process(inL);
            inR = trashEQ_R.process(inR);
        }

        // 3. Trash Bitcrusher (Light, on X-axis)
        if (effectiveTrashX > 0.01f)
        {
            float bits = 16.0f - effectiveTrashX * 9.0f; // crush down to 7 bits
            float levels = std::pow(2.0f, bits);
            float crushedL = std::round(inL * levels) / levels;
            float crushedR = std::round(inR * levels) / levels;
            
            inL = inL * (1.0f - effectiveTrashX * 0.25f) + crushedL * (effectiveTrashX * 0.25f);
            inR = inR * (1.0f - effectiveTrashX * 0.25f) + crushedR * (effectiveTrashX * 0.25f);
        }

        // 4. Gloss OTT multi-band compression & 14 kHz shelf (Y-axis)
        if (effectiveGlossY > 0.001f)
        {
            // Split bands L
            float lowL = crossoverLPF_L.processLP(inL, alphaCrossover);
            float highL = inL - lowL;
            
            // Split bands R
            float lowR = crossoverLPF_R.processLP(inR, alphaCrossover);
            float highR = inR - lowR;

            // Track RMS envelope
            envLowL = envLowL * decayCoeff + std::abs(lowL) * (1.0f - decayCoeff);
            envLowR = envLowR * decayCoeff + std::abs(lowR) * (1.0f - decayCoeff);
            envHighL = envHighL * decayCoeff + std::abs(highL) * (1.0f - decayCoeff);
            envHighR = envHighR * decayCoeff + std::abs(highR) * (1.0f - decayCoeff);

            // OTT Upward/Downward compression thresholds & gains
            // Downward: Threshold = -18dB (0.125f), Ratio = 4:1 (scale = 0.4f)
            // Upward: Threshold = -38dB (0.0125f), Ratio = 1:2 (scale = 2.0f)
            auto calcGain = [](float env) {
                float g = 1.0f;
                if (env > 0.125f)
                    g = 1.0f - (env - 0.125f) * 0.45f; // compress down
                else if (env < 0.0125f && env > 0.0001f)
                    g = 1.0f + (0.0125f - env) * 2.2f; // boost quiet mids/lows
                return juce::jlimit(0.3f, 2.5f, g);
            };

            float gLowL = calcGain(envLowL);
            float gLowR = calcGain(envLowR);
            float gHighL = calcGain(envHighL);
            float gHighR = calcGain(envHighR);

            // Reconstruct OTT band sum
            float ottL = lowL * gLowL + highL * gHighL;
            float ottR = lowR * gLowR + highR * gHighR;

            // Interpolate OTT Gloss
            inL = inL * (1.0f - effectiveGlossY) + ottL * effectiveGlossY;
            inR = inR * (1.0f - effectiveGlossY) + ottR * effectiveGlossY;

            // Gloss EQ High shelf boost at 14 kHz
            inL = glossEQ_L.process(inL);
            inR = glossEQ_R.process(inR);
        }

        // 5. HPF Sweep for "The Drop / Build" Macro
        if (D > 0.001f)
        {
            inL = dropHPF_L.process(inL);
            inR = dropHPF_R.process(inR);
        }

        // 6. Mono-Maker: collapse side details under 120 Hz to pure mono
        if (monoMakerActive && numChannels >= 2)
        {
            float mid = (inL + inR) * 0.5f;
            float side = (inL - inR) * 0.5f;
            
            // High-pass filter the side information
            float sideFiltered = sideHPF.process(side);
            
            inL = mid + sideFiltered;
            inR = mid - sideFiltered;
        }

        // 7. Sync Sidechain Pumper volume ducking
        if (pumpActive)
        {
            double samplePPQ = ppqPosition + s * (ppqRate / sampleRate);
            double phase = std::fmod(samplePPQ, divBeats) / divBeats;
            float pumpEnv = std::pow(static_cast<float>(phase), pumpCurve);
            float pumpGain = (1.0f - pumpDepth) + pumpDepth * pumpEnv;
            
            inL *= pumpGain;
            inR *= pumpGain;
        }

        leftData[s] = inL;
        if (rightData)
            rightData[s] = inR;
    }

    // 8. Stereo Chorus
    if (effectiveChorusMix > 0.001f)
    {
        chorus.setRate(chorusRate);
        chorus.setDepth(effectiveChorusDepth);
        chorus.setMix(effectiveChorusMix);

        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        chorus.process(context);
    }

    // 9. Ping-Pong Delay
    if (effectiveDelayMix > 0.001f && numChannels >= 2)
    {
        int delaySampleOffset = juce::jlimit(1, delayBufferLength - 1, static_cast<int>((delayTimeMs / 1000.0f) * sampleRate));

        leftData = buffer.getWritePointer(0);
        rightData = buffer.getWritePointer(1);
        float* dBufL = delayBufferL.getWritePointer(0);
        float* dBufR = delayBufferR.getWritePointer(0);

        float fc = 400.0f + delayColor * 15000.0f; // 400Hz to 15.4kHz
        float dt = 1.0f / static_cast<float>(sampleRate);
        float tau = 1.0f / (2.0f * juce::MathConstants<float>::pi * fc);
        float alphaLP = tau / (tau + dt);

        for (int s = 0; s < numSamples; ++s)
        {
            int readPosL = (delayWritePosL - delaySampleOffset + delayBufferLength) % delayBufferLength;
            int readPosR = (delayWritePosR - delaySampleOffset + delayBufferLength) % delayBufferLength;

            float delayedL = dBufL[readPosL];
            float delayedR = dBufR[readPosR];

            // Apply color low-pass filters to delayed signals
            float delayedLP_L = delayColorFilterL.processLP(delayedL, alphaLP);
            float delayedLP_R = delayColorFilterR.processLP(delayedR, alphaLP);

            float inputL = leftData[s];
            float inputR = rightData[s];

            // Cross feedback delay (with color LPF applied to feedback path)
            dBufL[delayWritePosL] = inputL + (delayedLP_R * effectiveDelayFeedback);
            dBufR[delayWritePosR] = inputR + (delayedLP_L * effectiveDelayFeedback);

            leftData[s] = inputL * (1.0f - effectiveDelayMix) + delayedLP_L * effectiveDelayMix;
            rightData[s] = inputR * (1.0f - effectiveDelayMix) + delayedLP_R * effectiveDelayMix;

            delayWritePosL = (delayWritePosL + 1) % delayBufferLength;
            delayWritePosR = (delayWritePosR + 1) % delayBufferLength;
        }
    }

    // 10. Reverb
    if (effectiveReverbMix > 0.001f)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        reverb.process(context);
    }
}
