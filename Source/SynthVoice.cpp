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
    midiNote = midiNoteNumber;
    float targetFreq = static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
    
    if (glideTimeMs > 0.05f && targetFrequency > 0.0f)
    {
        // Glide from last frequency
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

    // Trigger transient shaper
    transientTime = 0.0f;
    transientActive = true;
    transientPhase = 0.0f;
    lastTransientSample = 0.0f;

    // Reset sub phase
    subPhase = 0.0f;

    // Reset pitch drop
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
    spec.numChannels = 1;

    formantF1L.prepare(spec); formantF1R.prepare(spec);
    formantF2L.prepare(spec); formantF2R.prepare(spec);
    formantF3L.prepare(spec); formantF3R.prepare(spec);

    formantF1L.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    formantF1R.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    formantF2L.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    formantF2R.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    formantF3L.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    formantF3R.setType(juce::dsp::StateVariableTPTFilterType::bandpass);

    juce::dsp::ProcessSpec specLadder;
    specLadder.sampleRate = sampleRate;
    specLadder.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    specLadder.numChannels = static_cast<juce::uint32>(outputChannels);
    filter.prepare(specLadder);
    filter.setEnabled(true);
    
    notchFilterL.reset();
    notchFilterR.reset();

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
        glideFactor = std::pow(0.01f, 1.0f / (glideTimeMs * 0.001f * static_cast<float>(currentSampleRate)));
    }
}

void SynthVoice::setHostInfo(double bpm)
{
    hostBpm = bpm;
}

void SynthVoice::updateParameters(juce::AudioProcessorValueTreeState& apvts)
{
    osc1Shape = apvts.getRawParameterValue("osc1_shape")->load();
    osc1Octave = static_cast<int>(apvts.getRawParameterValue("osc1_octave")->load());
    osc1DetuneCents = apvts.getRawParameterValue("osc1_detune")->load();
    unisonCount = static_cast<int>(apvts.getRawParameterValue("unison_count")->load());
    unisonDetuneCents = apvts.getRawParameterValue("unison_detune")->load();
    osc1Level = apvts.getRawParameterValue("osc1_level")->load();

    osc2Shape = apvts.getRawParameterValue("osc2_shape")->load();
    osc2Octave = static_cast<int>(apvts.getRawParameterValue("osc2_octave")->load());
    osc2DetuneCents = apvts.getRawParameterValue("osc2_detune")->load();
    osc2Level = apvts.getRawParameterValue("osc2_level")->load();
    fmDepth = apvts.getRawParameterValue("osc_fm_depth")->load();
    oscSyncActive = apvts.getRawParameterValue("osc_sync")->load() > 0.5f;

    // Transient selector
    transientType = static_cast<int>(apvts.getRawParameterValue("transient_type")->load());
    transientLevel = apvts.getRawParameterValue("transient_level")->load();
    transientDecay = apvts.getRawParameterValue("transient_decay")->load();

    // Sub Anchor parameters
    subLevel = apvts.getRawParameterValue("sub_level")->load();
    subOctave = static_cast<int>(apvts.getRawParameterValue("sub_octave")->load());
    subWave = static_cast<int>(apvts.getRawParameterValue("sub_wave")->load());
    subDrive = apvts.getRawParameterValue("sub_drive")->load();

    baseCutoffHz = apvts.getRawParameterValue("filter_cutoff")->load();
    filterResonance = apvts.getRawParameterValue("filter_res")->load();
    filterDrive = apvts.getRawParameterValue("filter_drive")->load();
    filterEnvAmount = apvts.getRawParameterValue("filter_env_amt")->load();
    filterKeyTrack = apvts.getRawParameterValue("filter_key_track")->load();
    filterLfoModAmount = apvts.getRawParameterValue("filter_lfo_mod")->load();
    filterMode = static_cast<int>(apvts.getRawParameterValue("filter_mode")->load());

    // LFO parameters
    lfo1Rate = static_cast<int>(apvts.getRawParameterValue("lfo1_rate")->load());
    lfo1Wave = static_cast<int>(apvts.getRawParameterValue("lfo1_wave")->load());
    lfo1ToCutoff = apvts.getRawParameterValue("lfo1_to_cutoff")->load();
    lfo1ToShape = apvts.getRawParameterValue("lfo1_to_shape")->load();

    lfo2Rate = static_cast<int>(apvts.getRawParameterValue("lfo2_rate")->load());
    lfo2Wave = static_cast<int>(apvts.getRawParameterValue("lfo2_wave")->load());
    lfo2ToPitch = apvts.getRawParameterValue("lfo2_to_pitch")->load();
    lfo2ToPan = apvts.getRawParameterValue("lfo2_to_pan")->load();

    // Envelopes curve shapers
    ampDecayCurve = apvts.getRawParameterValue("amp_decay_curve")->load();
    filterDecayCurve = apvts.getRawParameterValue("filter_decay_curve")->load();

    // Setup envelopes
    float aAttack = apvts.getRawParameterValue("amp_attack")->load();
    float aDecay = apvts.getRawParameterValue("amp_decay")->load();
    float aSustain = apvts.getRawParameterValue("amp_sustain")->load();
    float aRelease = apvts.getRawParameterValue("amp_release")->load();

    // Scale envelopes with macro punch
    float punch = apvts.getRawParameterValue("macro_punch")->load();
    aAttack = aAttack * (1.0f - punch * 0.85f);
    transientLevel = juce::jlimit(0.0f, 1.0f, transientLevel + punch * 0.5f);

    ampEnv.setParameters(aAttack, aDecay, aSustain, aRelease, ampDecayCurve);

    float fAttack = apvts.getRawParameterValue("filter_attack")->load();
    float fDecay = apvts.getRawParameterValue("filter_decay")->load();
    float fSustain = apvts.getRawParameterValue("filter_sustain")->load();
    float fRelease = apvts.getRawParameterValue("filter_release")->load();
    fAttack = fAttack * (1.0f - punch * 0.85f);

    filterEnv.setParameters(fAttack, fDecay, fSustain, fRelease, filterDecayCurve);

    // Pitch Drop parameters
    pitchDropActive = apvts.getRawParameterValue("pitch_drop_active")->load() > 0.5f;
    pitchDropOctaves = apvts.getRawParameterValue("pitch_drop_octaves")->load();
    pitchDropDuration = apvts.getRawParameterValue("pitch_drop_time")->load() / 1000.0f;

    // Formant filter morph
    formantMorph = apvts.getRawParameterValue("formant_morph")->load();

    // Legato glide time
    glideTimeMs = apvts.getRawParameterValue("glide_time")->load();

    // Map Ladder Filter mode
    if (filterMode < 4)
    {
        juce::dsp::LadderFilterMode mode;
        switch (filterMode)
        {
            case 0: mode = juce::dsp::LadderFilterMode::LPF12; break;
            case 1: mode = juce::dsp::LadderFilterMode::LPF24; break;
            case 2: mode = juce::dsp::LadderFilterMode::BPF12; break;
            case 3: mode = juce::dsp::LadderFilterMode::HPF12; break;
            default: mode = juce::dsp::LadderFilterMode::LPF12; break;
        }
        filter.setMode(mode);
        filter.setResonance(filterResonance);
    }
}

float SynthVoice::generateMorphedSample(float shape, float phase)
{
    auto getSine = [](float p) { return std::sin(p); };
    auto getTri = [](float p) {
        float norm = p / juce::MathConstants<float>::twoPi;
        return 2.0f * std::abs(2.0f * norm - 1.0f) - 1.0f;
    };
    auto getSaw = [](float p) { return 1.0f - (2.0f * p / juce::MathConstants<float>::twoPi); };
    auto getSquare = [](float p) { return (p < juce::MathConstants<float>::pi) ? 1.0f : -1.0f; };

    if (shape <= 0.0f) return getSine(phase);
    if (shape >= 3.0f) return getSquare(phase);

    int idx = static_cast<int>(shape);
    float frac = shape - static_cast<float>(idx);

    if (idx == 0)      return getSine(phase) * (1.0f - frac) + getTri(phase) * frac;
    else if (idx == 1) return getTri(phase) * (1.0f - frac) + getSaw(phase) * frac;
    else if (idx == 2) return getSaw(phase) * (1.0f - frac) + getSquare(phase) * frac;

    return 0.0f;
}

float SynthVoice::generateLfoSample(int waveType, float phase, float& lastRandVal, bool phaseWrapped)
{
    switch (waveType)
    {
        case 0: // Sine
            return std::sin(phase);
        case 1: // Triangle
            {
                float norm = phase / juce::MathConstants<float>::twoPi;
                return 2.0f * std::abs(2.0f * norm - 1.0f) - 1.0f;
            }
        case 2: // Saw
            return 1.0f - (2.0f * phase / juce::MathConstants<float>::twoPi);
        case 3: // Square
            return (phase < juce::MathConstants<float>::pi) ? 1.0f : -1.0f;
        case 4: // Random S&H
            if (phaseWrapped)
                lastRandVal = random.nextFloat() * 2.0f - 1.0f;
            return lastRandVal;
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

    // LFO subdivisions
    static const float lfoDivisions[12] = { 
        4.0f, 2.0f, 1.0f, 0.5f, 0.25f, 0.125f, // 1/1, 1/2, 1/4, 1/8, 1/16, 1/32
        2.0f/3.0f, 1.0f/3.0f, 0.5f/3.0f,       // Triplet: 1/4T, 1/8T, 1/16T
        1.5f, 0.75f, 0.375f                     // Dotted: 1/4D, 1/8D, 1/16D
    };
    
    float div1 = lfoDivisions[juce::jlimit(0, 11, lfo1Rate)];
    float lfo1Freq = static_cast<float>(hostBpm) / (60.0f * div1);
    float lfo1Inc = (juce::MathConstants<float>::twoPi * lfo1Freq) / static_cast<float>(currentSampleRate);

    float div2 = lfoDivisions[juce::jlimit(0, 11, lfo2Rate)];
    float lfo2Freq = static_cast<float>(hostBpm) / (60.0f * div2);
    float lfo2Inc = (juce::MathConstants<float>::twoPi * lfo2Freq) / static_cast<float>(currentSampleRate);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float ampVal = ampEnv.getNextSample();
        float filterVal = filterEnv.getNextSample();

        if (ampVal <= 0.00001f && !ampEnv.isActive())
        {
            clearCurrentNote();
            break;
        }

        // Advance LFO 1
        lfo1Phase += lfo1Inc;
        bool lfo1Wrapped = false;
        if (lfo1Phase >= juce::MathConstants<float>::twoPi)
        {
            lfo1Phase -= juce::MathConstants<float>::twoPi;
            lfo1Wrapped = true;
        }
        float lfo1Val = generateLfoSample(lfo1Wave, lfo1Phase, lastRandomVal1, lfo1Wrapped);

        // Advance LFO 2
        lfo2Phase += lfo2Inc;
        bool lfo2Wrapped = false;
        if (lfo2Phase >= juce::MathConstants<float>::twoPi)
        {
            lfo2Phase -= juce::MathConstants<float>::twoPi;
            lfo2Wrapped = true;
        }
        float lfo2Val = generateLfoSample(lfo2Wave, lfo2Phase, lastRandomVal2, lfo2Wrapped);

        // Process Legato Glide Frequency
        if (glideTimeMs > 0.05f)
        {
            currentFrequency = currentFrequency * glideFactor + targetFrequency * (1.0f - glideFactor);
        }
        else
        {
            currentFrequency = targetFrequency;
        }

        // Apply LFO 2 Vibrato pitch modulation (+-2 semitones)
        float vibratoMod = lfo2Val * lfo2ToPitch * 2.0f;
        float modulatedFreq = currentFrequency * std::pow(2.0f, vibratoMod / 12.0f);

        // Process Pitch Drop Sweep
        float pitchFreq = modulatedFreq;
        if (pitchDropActive && pitchDropTime < pitchDropDuration)
        {
            float pitchDropProgress = pitchDropTime / pitchDropDuration;
            float pitchDropAmt = 1.0f - pitchDropProgress;
            float semitones = pitchDropAmt * pitchDropOctaves * 12.0f;
            pitchFreq *= std::pow(2.0f, semitones / 12.0f);
            pitchDropTime += 1.0f / static_cast<float>(currentSampleRate);
        }

        // Generate Transient Click Layer
        float clickSample = 0.0f;
        if (transientActive && transientLevel > 0.001f)
        {
            float tDecaySecs = transientDecay / 1000.0f;
            float tEnv = std::exp(-transientTime / tDecaySecs);
            
            if (transientType == 0) // Punch Click (Highpass White Noise)
            {
                float noise = random.nextFloat() * 2.0f - 1.0f;
                float hpVal = noise - lastTransientSample;
                lastTransientSample = noise;
                clickSample = hpVal * tEnv * transientLevel;
            }
            else if (transientType == 1) // Piano Hammer (Rapid pitch swept sine)
            {
                float pitchEnv = std::exp(-transientTime / 0.005f);
                float freq = 4000.0f * pitchEnv + 300.0f;
                transientPhase += (juce::MathConstants<float>::twoPi * freq) / static_cast<float>(currentSampleRate);
                clickSample = std::sin(transientPhase) * tEnv * transientLevel;
            }
            else if (transientType == 2) // Vocal Snap (Bandpassed voice resonant snap)
            {
                transientPhase += (juce::MathConstants<float>::twoPi * 1500.0f) / static_cast<float>(currentSampleRate);
                float raw = std::sin(transientPhase) * (1.0f + 0.3f * (random.nextFloat() - 0.5f));
                clickSample = raw * tEnv * transientLevel;
            }
            else if (transientType == 3) // 808 Tock (Thumpy pitch swept sub pop)
            {
                float pitchEnv = std::exp(-transientTime / 0.015f);
                float freq = 500.0f * pitchEnv + 50.0f;
                transientPhase += (juce::MathConstants<float>::twoPi * freq) / static_cast<float>(currentSampleRate);
                clickSample = std::sin(transientPhase) * tEnv * transientLevel;
            }
            else if (transientType == 4) // Analog Pop (Single cycle pulse)
            {
                float pop = (transientTime < 0.0015f) ? 1.0f : -1.0f;
                clickSample = pop * tEnv * transientLevel;
            }
            
            transientTime += 1.0f / static_cast<float>(currentSampleRate);
            if (tEnv < 0.0001f)
                transientActive = false;
        }

        // Generate Sub-Oscillator (Strictly Mono Anchor) with Drive
        float subSample = 0.0f;
        if (subLevel > 0.001f)
        {
            float subFreq = pitchFreq * std::pow(2.0f, static_cast<float>(subOctave));
            float subInc = (juce::MathConstants<float>::twoPi * subFreq) / static_cast<float>(currentSampleRate);
            
            // Sub wave: 0: Sine, 1: Square
            float rawSub = 0.0f;
            if (subWave == 0)
                rawSub = std::sin(subPhase);
            else
                rawSub = (subPhase < juce::MathConstants<float>::pi) ? 1.0f : -1.0f;
                
            // Sub Drive pre-saturation
            rawSub = rawSub * (1.0f + subDrive * 3.0f);
            subSample = std::tanh(rawSub) * subLevel;
            
            subPhase += subInc;
            if (subPhase >= juce::MathConstants<float>::twoPi)
                subPhase -= juce::MathConstants<float>::twoPi;
        }

        // Update Osc 2 phase (modulator)
        float osc2Freq = pitchFreq * std::pow(2.0f, osc2Octave + (osc2DetuneCents / 1200.0f));
        float osc2PhaseInc = (juce::MathConstants<float>::twoPi * osc2Freq) / static_cast<float>(currentSampleRate);
        
        bool osc2Wrapped = false;
        osc2Phase += osc2PhaseInc;
        if (osc2Phase >= juce::MathConstants<float>::twoPi)
        {
            osc2Phase -= juce::MathConstants<float>::twoPi;
            osc2Wrapped = true;
        }
        
        // Osc 2 morphing
        float osc2Val = generateMorphedSample(osc2Shape, osc2Phase);
        
        // FM Modulator: modulates Osc 1 phase
        float fmModulator = osc2Val * osc2Level * fmDepth * 5.0f;

        // Generate Osc 1 (Unison Stereo Sum) with shape morphing and sync
        float osc1SampleL = 0.0f;
        float osc1SampleR = 0.0f;
        float osc1BaseFreq = pitchFreq * std::pow(2.0f, osc1Octave + (osc1DetuneCents / 1200.0f));

        // Modulate Osc 1 shape with LFO 1
        float modOsc1Shape = juce::jlimit(0.0f, 3.0f, osc1Shape + lfo1Val * lfo1ToShape);

        for (int u = 0; u < numUnisonVoices; ++u)
        {
            float detuneOffset = (numUnisonVoices > 1) ? unisonOffsets[u] * (unisonDetuneCents / 1200.0f) : 0.0f;
            float voiceFreq = osc1BaseFreq * std::pow(2.0f, detuneOffset);
            float phaseInc = (juce::MathConstants<float>::twoPi * voiceFreq) / static_cast<float>(currentSampleRate);

            // Hard Sync
            if (oscSyncActive && osc2Wrapped)
            {
                osc1Phases[u] = 0.0f;
            }

            osc1Phases[u] += phaseInc;
            if (osc1Phases[u] >= juce::MathConstants<float>::twoPi)
                osc1Phases[u] -= juce::MathConstants<float>::twoPi;

            // Apply FM Phase Modulation
            float modulatedPhase = osc1Phases[u] + fmModulator;
            while (modulatedPhase >= juce::MathConstants<float>::twoPi) modulatedPhase -= juce::MathConstants<float>::twoPi;
            while (modulatedPhase < 0.0f) modulatedPhase += juce::MathConstants<float>::twoPi;

            float sampleVal = generateMorphedSample(modOsc1Shape, modulatedPhase);

            osc1SampleL += sampleVal * unisonPansL[u];
            osc1SampleR += sampleVal * unisonPansR[u];
        }

        float unisonNorm = 1.0f / std::sqrt(static_cast<float>(numUnisonVoices));
        osc1SampleL *= osc1Level * unisonNorm;
        osc1SampleR *= osc1Level * unisonNorm;

        // Stereo panning modulation via LFO 2
        float effectivePan = lfo2Val * lfo2ToPan;
        float panL = std::cos((effectivePan + 1.0f) * juce::MathConstants<float>::pi * 0.25f);
        float panR = std::sin((effectivePan + 1.0f) * juce::MathConstants<float>::pi * 0.25f);

        float mixedL = (osc1SampleL * panL) + (osc2Val * osc2Level * panL);
        float mixedR = (osc1SampleR * panR) + (osc2Val * osc2Level * panR);

        // Mix down with main amp envelope and note velocity
        float voiceSampleL = mixedL * ampVal * noteVelocity + clickSample;
        float voiceSampleR = mixedR * ampVal * noteVelocity + clickSample;

        // Apply pre-filter saturation (Drive)
        voiceSampleL = std::tanh(voiceSampleL * filterDrive);
        voiceSampleR = std::tanh(voiceSampleR * filterDrive);

        // 7. Filter Section
        float filteredL = voiceSampleL;
        float filteredR = voiceSampleR;

        // Calculate modulated filter cutoff (Envelope + Keytrack + LFO 1)
        float envModSemitones = filterVal * filterEnvAmount * 48.0f; // +-4 octaves
        float keyTrackSemitones = (static_cast<float>(midiNote) - 60.0f) * filterKeyTrack;
        float lfoCutoffMod = lfo1Val * filterLfoModAmount * 60.0f; // +-5 octaves

        float totalModSemitones = envModSemitones + keyTrackSemitones + lfoCutoffMod;
        float modCutoff = baseCutoffHz * std::pow(2.0f, totalModSemitones / 12.0f);
        modCutoff = juce::jlimit(20.0f, 20000.0f, modCutoff);

        if (filterMode == 5) // Vowel Formant Filter (Morphable A-E-I-O-U)
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

            filteredL = formantF1L.processSample(0, voiceSampleL) +
                        formantF2L.processSample(0, voiceSampleL) * 0.5f +
                        formantF3L.processSample(0, voiceSampleL) * 0.25f;

            filteredR = formantF1R.processSample(0, voiceSampleR) +
                        formantF2R.processSample(0, voiceSampleR) * 0.5f +
                        formantF3R.processSample(0, voiceSampleR) * 0.25f;
        }
        else if (filterMode == 4) // Notch Filter (Custom Biquad)
        {
            float Q = 0.5f + filterResonance * 4.0f;
            notchFilterL.setNotch(static_cast<float>(currentSampleRate), modCutoff, Q);
            notchFilterR.setNotch(static_cast<float>(currentSampleRate), modCutoff, Q);
            filteredL = notchFilterL.process(voiceSampleL);
            filteredR = notchFilterR.process(voiceSampleR);
        }
        else // LP12, LP24, BP, HP
        {
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

        // Output to channels 0 & 1 (FX path)
        int sampleIdx = startSample + sample;
        outputBuffer.addSample(0, sampleIdx, filteredL);
        if (numBufferChannels > 1)
            outputBuffer.addSample(1, sampleIdx, filteredR);

        // Render mono sub anchor to channel 2 (Bypasses post FX)
        if (numBufferChannels > 2)
        {
            outputBuffer.addSample(2, sampleIdx, subSample);
        }
    }
}
