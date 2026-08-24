#include "SynthVoice.h"

SynthVoice::SynthVoice()
{
    for (int i = 0; i < 7; ++i)
        osc1Phases[i] = random.nextFloat() * juce::MathConstants<float>::twoPi;
    osc2Phase = random.nextFloat() * juce::MathConstants<float>::twoPi;
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/)
{
    float targetFreq = static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
    
    // Legato glide initialization
    if (glideTimeMs > 0.05f && targetFrequency > 0.0f)
    {
        // Glide starts from the last active voice frequency instead of jumping instantly
    }
    else
    {
        currentFrequency = targetFreq;
    }
    
    targetFrequency = targetFreq;
    noteVelocity = velocity;

    for (int i = 0; i < 7; ++i)
        osc1Phases[i] = random.nextFloat() * juce::MathConstants<float>::twoPi;
    osc2Phase = random.nextFloat() * juce::MathConstants<float>::twoPi;

    // Reset Click transient envelope
    clickTime = 0.0f;
    clickActive = true;

    // Reset Sub-oscillator phase
    subPhase = 0.0f;

    // Reset Pitch Drop sweep
    pitchDropTime = 0.0f;

    ampEnv.noteOn();
    filterEnv.noteOn();
}

void SynthVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        ampEnv.noteOff();
        filterEnv.noteOff();
    }
    else
    {
        clearCurrentNote();
        ampEnv.reset();
        filterEnv.reset();
    }
}

void SynthVoice::pitchWheelMoved(int /*newPitchWheelValue*/) {}
void SynthVoice::controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/) {}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    currentSampleRate = sampleRate;
    ampEnv.setSampleRate(sampleRate);
    filterEnv.setSampleRate(sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 1; // Format filters are processed mono sample-by-sample

    formantF1L.prepare(spec); formantF1R.prepare(spec);
    formantF2L.prepare(spec); formantF2R.prepare(spec);
    formantF3L.prepare(spec); formantF3R.prepare(spec);

    formantF1L.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    formantF1R.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    formantF2L.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    formantF2R.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    formantF3L.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    formantF3R.setType(juce::dsp::StateVariableTPTFilterType::bandpass);

    // Prepare main Ladder Filter
    juce::dsp::ProcessSpec specLadder;
    specLadder.sampleRate = sampleRate;
    specLadder.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    specLadder.numChannels = static_cast<juce::uint32>(outputChannels);
    filter.prepare(specLadder);
    filter.setEnabled(true);
    
    isPrepared = true;
}

void SynthVoice::glideTo(float targetFreq, float glideTimeMsVal)
{
    targetFrequency = targetFreq;
    glideTimeMs = glideTimeMsVal;
    
    if (glideTimeMs <= 0.05f)
    {
        currentFrequency = targetFrequency;
        glideFactor = 0.0f;
    }
    else
    {
        // Exponential glide decay coefficient (reaches 99% in glideTimeMs)
        glideFactor = std::pow(0.01f, 1.0f / (glideTimeMs * 0.001f * static_cast<float>(currentSampleRate)));
    }
}

void SynthVoice::updateParameters(juce::AudioProcessorValueTreeState& apvts)
{
    osc1Wave = static_cast<int>(apvts.getRawParameterValue("osc1_wave")->load());
    osc1Octave = static_cast<int>(apvts.getRawParameterValue("osc1_octave")->load());
    osc1DetuneCents = apvts.getRawParameterValue("osc1_detune")->load();
    unisonCount = static_cast<int>(apvts.getRawParameterValue("unison_count")->load());
    unisonDetuneCents = apvts.getRawParameterValue("unison_detune")->load();
    osc1Level = apvts.getRawParameterValue("osc1_level")->load();

    osc2Wave = static_cast<int>(apvts.getRawParameterValue("osc2_wave")->load());
    osc2Octave = static_cast<int>(apvts.getRawParameterValue("osc2_octave")->load());
    osc2DetuneCents = apvts.getRawParameterValue("osc2_detune")->load();
    osc2Level = apvts.getRawParameterValue("osc2_level")->load();

    juce::ADSR::Parameters ampParams;
    ampParams.attack  = apvts.getRawParameterValue("amp_attack")->load();
    ampParams.decay   = apvts.getRawParameterValue("amp_decay")->load();
    ampParams.sustain = apvts.getRawParameterValue("amp_sustain")->load();
    ampParams.release = apvts.getRawParameterValue("amp_release")->load();
    
    // Scale amp envelope attack speed with macro punch (punch makes it snappier)
    float punch = apvts.getRawParameterValue("macro_punch")->load();
    ampParams.attack = ampParams.attack * (1.0f - punch * 0.85f);
    ampEnv.setParameters(ampParams);

    juce::ADSR::Parameters filterParams;
    filterParams.attack  = apvts.getRawParameterValue("filter_attack")->load();
    filterParams.decay   = apvts.getRawParameterValue("filter_decay")->load();
    filterParams.sustain = apvts.getRawParameterValue("filter_sustain")->load();
    filterParams.release = apvts.getRawParameterValue("filter_release")->load();
    
    // Scale filter envelope attack speed with macro punch
    filterParams.attack = filterParams.attack * (1.0f - punch * 0.85f);
    filterEnv.setParameters(filterParams);

    baseCutoffHz = apvts.getRawParameterValue("filter_cutoff")->load();
    filterResonance = apvts.getRawParameterValue("filter_res")->load();
    filterEnvAmount = apvts.getRawParameterValue("filter_env_amt")->load();
    filterMode = static_cast<int>(apvts.getRawParameterValue("filter_mode")->load());

    // Transient click layer parameters
    clickDecay = apvts.getRawParameterValue("click_decay")->load();
    clickLevel = apvts.getRawParameterValue("click_level")->load() + punch * 0.8f; // Macro punch boosts click level
    clickLevel = juce::jlimit(0.0f, 1.0f, clickLevel);

    // Mono sub anchor parameters
    subLevel = apvts.getRawParameterValue("sub_level")->load();
    subOctave = static_cast<int>(apvts.getRawParameterValue("sub_octave")->load());
    subWave = static_cast<int>(apvts.getRawParameterValue("sub_wave")->load());

    // Pitch Drop parameters
    pitchDropActive = apvts.getRawParameterValue("pitch_drop_active")->load() > 0.5f;
    pitchDropOctaves = apvts.getRawParameterValue("pitch_drop_octaves")->load();
    pitchDropDuration = apvts.getRawParameterValue("pitch_drop_time")->load() / 1000.0f; // ms to seconds

    // Formant filter morph
    formantMorph = apvts.getRawParameterValue("formant_morph")->load();

    // Legato glide time
    glideTimeMs = apvts.getRawParameterValue("glide_time")->load();

    // Map Ladder Filter mode
    if (filterMode < 6)
    {
        juce::dsp::LadderFilterMode mode;
        switch (filterMode)
        {
            case 0: mode = juce::dsp::LadderFilterMode::LPF12; break;
            case 1: mode = juce::dsp::LadderFilterMode::LPF24; break;
            case 2: mode = juce::dsp::LadderFilterMode::HPF12; break;
            case 3: mode = juce::dsp::LadderFilterMode::HPF24; break;
            case 4: mode = juce::dsp::LadderFilterMode::BPF12; break;
            case 5: mode = juce::dsp::LadderFilterMode::BPF24; break;
            default: mode = juce::dsp::LadderFilterMode::LPF12; break;
        }
        filter.setMode(mode);
        filter.setResonance(filterResonance);
    }
}

float SynthVoice::generateSample(int waveType, float phase)
{
    switch (waveType)
    {
        case 0: // Sine
            return std::sin(phase);
        case 1: // Saw
            return 1.0f - (2.0f * phase / juce::MathConstants<float>::twoPi);
        case 2: // Square
            return (phase < juce::MathConstants<float>::pi) ? 1.0f : -1.0f;
        case 3: // Triangle
            {
                float norm = phase / juce::MathConstants<float>::twoPi;
                return 2.0f * std::abs(2.0f * norm - 1.0f) - 1.0f;
            }
        case 4: // Noise
            return random.nextFloat() * 2.0f - 1.0f;
        default:
            return 0.0f;
    }
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (!isVoiceActive())
        return;

    static const float unisonOffsets[7] = { -1.0f, -0.666f, -0.333f, 0.0f, 0.333f, 0.666f, 1.0f };
    static const float unisonPansL[7]   = {  1.0f,   0.85f,   0.70f, 0.50f,  0.30f,  0.15f, 0.0f };
    static const float unisonPansR[7]   = {  0.0f,   0.15f,   0.30f, 0.50f,  0.70f,  0.85f, 1.0f };

    int numUnisonVoices = std::max(1, std::min(7, unisonCount));
    int numBufferChannels = outputBuffer.getNumChannels();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float ampVal = ampEnv.getNextSample();
        float filterVal = filterEnv.getNextSample();

        if (ampVal <= 0.00001f && !ampEnv.isActive())
        {
            clearCurrentNote();
            break;
        }

        // 1. Process Legato Glide Frequency
        if (glideTimeMs > 0.05f)
        {
            currentFrequency = currentFrequency * glideFactor + targetFrequency * (1.0f - glideFactor);
        }
        else
        {
            currentFrequency = targetFrequency;
        }

        // 2. Process Pitch Drop Sweep
        float pitchFreq = currentFrequency;
        if (pitchDropActive && pitchDropTime < pitchDropDuration)
        {
            float pitchDropProgress = pitchDropTime / pitchDropDuration;
            float pitchDropAmt = 1.0f - pitchDropProgress;
            float semitones = pitchDropAmt * pitchDropOctaves * 12.0f;
            pitchFreq *= std::pow(2.0f, semitones / 12.0f);
            pitchDropTime += 1.0f / static_cast<float>(currentSampleRate);
        }

        // 3. Generate Transient Click Layer sample
        float clickSample = 0.0f;
        if (clickActive)
        {
            float clickEnv = std::exp(-clickTime / clickDecay);
            clickSample = (random.nextFloat() * 2.0f - 1.0f) * clickEnv * clickLevel;
            clickTime += 1.0f / static_cast<float>(currentSampleRate);
            if (clickEnv < 0.0001f)
                clickActive = false;
        }

        // 4. Generate Sub-Oscillator (Strictly Mono Anchor)
        float subSample = 0.0f;
        if (subLevel > 0.001f)
        {
            float subFreq = pitchFreq * std::pow(2.0f, static_cast<float>(subOctave));
            float subInc = (juce::MathConstants<float>::twoPi * subFreq) / static_cast<float>(currentSampleRate);
            subSample = generateSample(subWave, subPhase) * subLevel;
            
            subPhase += subInc;
            if (subPhase >= juce::MathConstants<float>::twoPi)
                subPhase -= juce::MathConstants<float>::twoPi;
        }

        // 5. Generate Osc 1 (Unison Stereo Sum)
        float osc1SampleL = 0.0f;
        float osc1SampleR = 0.0f;

        float osc1BaseFreq = pitchFreq * std::pow(2.0f, osc1Octave + (osc1DetuneCents / 1200.0f));

        for (int u = 0; u < numUnisonVoices; ++u)
        {
            float detuneOffset = (numUnisonVoices > 1) ? unisonOffsets[u] * (unisonDetuneCents / 1200.0f) : 0.0f;
            float subFreq = osc1BaseFreq * std::pow(2.0f, detuneOffset);
            float phaseInc = (juce::MathConstants<float>::twoPi * subFreq) / static_cast<float>(currentSampleRate);

            float sampleVal = generateSample(osc1Wave, osc1Phases[u]);

            osc1Phases[u] += phaseInc;
            if (osc1Phases[u] >= juce::MathConstants<float>::twoPi)
                osc1Phases[u] -= juce::MathConstants<float>::twoPi;

            if (numUnisonVoices == 1)
            {
                osc1SampleL += sampleVal;
                osc1SampleR += sampleVal;
            }
            else
            {
                osc1SampleL += sampleVal * unisonPansL[u];
                osc1SampleR += sampleVal * unisonPansR[u];
            }
        }

        float unisonNorm = 1.0f / std::sqrt(static_cast<float>(numUnisonVoices));
        osc1SampleL *= osc1Level * unisonNorm;
        osc1SampleR *= osc1Level * unisonNorm;

        // 6. Generate Osc 2
        float osc2Freq = pitchFreq * std::pow(2.0f, osc2Octave + (osc2DetuneCents / 1200.0f));
        float osc2PhaseInc = (juce::MathConstants<float>::twoPi * osc2Freq) / static_cast<float>(currentSampleRate);
        float osc2Val = generateSample(osc2Wave, osc2Phase) * osc2Level;

        osc2Phase += osc2PhaseInc;
        if (osc2Phase >= juce::MathConstants<float>::twoPi)
            osc2Phase -= juce::MathConstants<float>::twoPi;

        // Mix oscillators + Click layer
        float voiceSampleL = (osc1SampleL + osc2Val) * ampVal * noteVelocity + clickSample;
        float voiceSampleR = (osc1SampleR + osc2Val) * ampVal * noteVelocity + clickSample;

        // 7. Filter Section
        float filteredL = voiceSampleL;
        float filteredR = voiceSampleR;

        if (filterMode == 6) // Vowel Formant Filter (Morphable A-E-I-O-U)
        {
            float modulatedMorph = formantMorph + (filterVal * filterEnvAmount * 4.0f);
            modulatedMorph = juce::jlimit(0.0f, 4.0f, modulatedMorph);

            // Morph vowel frequencies
            static const float f1s[5] = { 700.0f, 500.0f, 300.0f, 570.0f, 300.0f };
            static const float f2s[5] = { 1220.0f, 1700.0f, 2200.0f, 840.0f, 870.0f };
            static const float f3s[5] = { 2600.0f, 2440.0f, 3000.0f, 2410.0f, 2240.0f };

            int idx = static_cast<int>(modulatedMorph);
            float frac = modulatedMorph - static_cast<float>(idx);

            float f1 = (idx < 4) ? (f1s[idx] * (1.0f - frac) + f1s[idx + 1] * frac) : f1s[4];
            float f2 = (idx < 4) ? (f2s[idx] * (1.0f - frac) + f2s[idx + 1] * frac) : f2s[4];
            float f3 = (idx < 4) ? (f3s[idx] * (1.0f - frac) + f3s[idx + 1] * frac) : f3s[4];

            float Q = 3.0f + filterResonance * 12.0f;
            float damping = 1.0f / Q;

            formantF1L.setCutoffFrequency(f1);
            formantF1R.setCutoffFrequency(f1);
            formantF2L.setCutoffFrequency(f2);
            formantF2R.setCutoffFrequency(f2);
            formantF3L.setCutoffFrequency(f3);
            formantF3R.setCutoffFrequency(f3);

            formantF1L.setResonance(damping);
            formantF1R.setResonance(damping);
            formantF2L.setResonance(damping);
            formantF2R.setResonance(damping);
            formantF3L.setResonance(damping);
            formantF3R.setResonance(damping);

            // Process parallel filters directly sample-by-sample
            filteredL = formantF1L.processSample(0, voiceSampleL) +
                        formantF2L.processSample(0, voiceSampleL) * 0.5f +
                        formantF3L.processSample(0, voiceSampleL) * 0.25f;

            filteredR = formantF1R.processSample(0, voiceSampleR) +
                        formantF2R.processSample(0, voiceSampleR) * 0.5f +
                        formantF3R.processSample(0, voiceSampleR) * 0.25f;
        }
        else // Standard Ladder Filter
        {
            float envModSemitones = filterVal * filterEnvAmount * 48.0f; // +-4 octaves
            float modCutoff = baseCutoffHz * std::pow(2.0f, envModSemitones / 12.0f);
            modCutoff = juce::jlimit(20.0f, 20000.0f, modCutoff);
            filter.setCutoffFrequencyHz(modCutoff);

            if (numBufferChannels == 1)
            {
                float* channelData[] = { &filteredL };
                juce::dsp::AudioBlock<float> block(channelData, 1, 1);
                juce::dsp::ProcessContextReplacing<float> context(block);
                filter.process(context);
            }
            else if (numBufferChannels >= 2)
            {
                float* channelData[] = { &filteredL, &filteredR };
                juce::dsp::AudioBlock<float> block(channelData, 2, 1);
                juce::dsp::ProcessContextReplacing<float> context(block);
                filter.process(context);
            }
        }

        // Add filtered voice samples to output channels 0 & 1
        int sampleIdx = startSample + sample;
        outputBuffer.addSample(0, sampleIdx, filteredL);
        if (numBufferChannels > 1)
            outputBuffer.addSample(1, sampleIdx, filteredR);

        // Add sub anchor to channel 2 (for PostFX mono sub anchor routing)
        if (numBufferChannels > 2)
        {
            outputBuffer.addSample(2, sampleIdx, subSample * ampVal * noteVelocity);
        }
    }
}
