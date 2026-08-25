#include "SynthVoice.h"

SynthVoice::SynthVoice()
{
    for (int i = 0; i < 7; ++i)
        osc1Phases[i] = random.nextFloat() * juce::MathConstants<float>::twoPi;
    osc2Phase = random.nextFloat() * juce::MathConstants<float>::twoPi;
    driftSpeed = 0.2f + random.nextFloat() * 0.3f;
    driftPhase = random.nextFloat() * juce::MathConstants<float>::twoPi;
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/)
{
    midiNote = midiNoteNumber;
    float targetFreq = static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
    
    // Check if we should glide from previous frequency
    bool shouldGlide = (glideTimeMs > 0.5f) && (glideMode != 2); // 2: Off

    if (shouldGlide && lastPlayedFrequency > 20.0f)
    {
        currentFrequency = lastPlayedFrequency;
    }
    else
    {
        currentFrequency = targetFreq;
    }
    
    targetFrequency = targetFreq;
    lastPlayedFrequency = targetFreq;
    noteVelocity = velocity;

    // Layer B reset
    layerBTime = 0.0f;
    layerBPhase = 0.0f;

    // Only randomize oscillator phases on fresh non-gliding triggers
    if (!shouldGlide || std::abs(currentFrequency - targetFrequency) < 0.1f)
    {
        for (int i = 0; i < 7; ++i)
            osc1Phases[i] = random.nextFloat() * juce::MathConstants<float>::twoPi;
        osc2Phase = random.nextFloat() * juce::MathConstants<float>::twoPi;
    }

    // Trigger transient shaper only on fresh non-gliding attacks
    if (!shouldGlide || std::abs(currentFrequency - targetFrequency) < 0.1f)
    {
        transientTime = 0.0f;
        transientActive = true;
        transientPhase = 0.0f;
        lastTransientSample = 0.0f;
    }
    else
    {
        transientActive = false;
    }

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

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
    float normalized = (static_cast<float>(newPitchWheelValue) - 8192.0f) / 8192.0f;
    pitchWheelSemitones = normalized * pitchBendRange;
}

void SynthVoice::setPitchBend(float semitones)
{
    pitchWheelSemitones = semitones;
}

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
    lastPlayedFrequency = targetFreq;
    if (glideTimeMsVal >= 0.0f)
        glideTimeMs = glideTimeMsVal;
        
    if (glideTimeMs <= 0.5f)
    {
        currentFrequency = targetFrequency;
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

    // Analog Drift & Sound Stacker
    if (apvts.getRawParameterValue("analog_drift") != nullptr)
        analogDriftAmt = apvts.getRawParameterValue("analog_drift")->load();
    if (apvts.getRawParameterValue("layer_b_type") != nullptr)
        layerBType = static_cast<int>(apvts.getRawParameterValue("layer_b_type")->load());
    if (apvts.getRawParameterValue("layer_b_mix") != nullptr)
        layerBMix = apvts.getRawParameterValue("layer_b_mix")->load();

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

    float envDelay = (apvts.getRawParameterValue("env_delay") != nullptr) ? apvts.getRawParameterValue("env_delay")->load() : 0.0f;
    float envHold = (apvts.getRawParameterValue("env_hold") != nullptr) ? apvts.getRawParameterValue("env_hold")->load() : 0.0f;
    float decTension = (apvts.getRawParameterValue("env_dec_tension") != nullptr) ? apvts.getRawParameterValue("env_dec_tension")->load() : 1.0f;
    float relTension = (apvts.getRawParameterValue("env_rel_tension") != nullptr) ? apvts.getRawParameterValue("env_rel_tension")->load() : 1.0f;

    // Scale envelopes with macro punch
    float punch = apvts.getRawParameterValue("macro_punch")->load();
    aAttack = aAttack * (1.0f - punch * 0.85f);
    transientLevel = juce::jlimit(0.0f, 1.0f, transientLevel + punch * 0.5f);

    ampEnv.setParameters(aAttack, aDecay, aSustain, aRelease, decTension, envDelay, envHold, relTension);

    float fAttack = apvts.getRawParameterValue("filter_attack")->load();
    float fDecay = apvts.getRawParameterValue("filter_decay")->load();
    float fSustain = apvts.getRawParameterValue("filter_sustain")->load();
    float fRelease = apvts.getRawParameterValue("filter_release")->load();
    fAttack = fAttack * (1.0f - punch * 0.85f);

    filterEnv.setParameters(fAttack, fDecay, fSustain, fRelease, decTension, envDelay, envHold, relTension);

    // Pitch Drop parameters
    pitchDropActive = apvts.getRawParameterValue("pitch_drop_active")->load() > 0.5f;
    pitchDropOctaves = apvts.getRawParameterValue("pitch_drop_octaves")->load();
    pitchDropDuration = apvts.getRawParameterValue("pitch_drop_time")->load() / 1000.0f;

    // Formant filter morph
    formantMorph = apvts.getRawParameterValue("formant_morph")->load();

    // Portamento & Glide parameters
    glideTimeMs = apvts.getRawParameterValue("glide_time")->load();
    if (apvts.getRawParameterValue("glide_mode") != nullptr)
        glideMode = static_cast<int>(apvts.getRawParameterValue("glide_mode")->load());
    if (apvts.getRawParameterValue("pitch_bend_range") != nullptr)
        pitchBendRange = apvts.getRawParameterValue("pitch_bend_range")->load();

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

        // Process Smooth Portamento & Legato Pitch Slide in Logarithmic Musical Cents
        if (glideTimeMs > 0.5f && currentFrequency > 10.0f && targetFrequency > 10.0f)
        {
            if (std::abs(currentFrequency - targetFrequency) > 0.01f)
            {
                float currentCents = 1200.0f * std::log2(currentFrequency / 440.0f);
                float targetCents = 1200.0f * std::log2(targetFrequency / 440.0f);
                float alpha = 1.0f - std::exp(-1.0f / (std::max(0.001f, glideTimeMs * 0.001f) * static_cast<float>(currentSampleRate) * 0.22f));
                currentCents += (targetCents - currentCents) * alpha;
                currentFrequency = 440.0f * std::pow(2.0f, currentCents / 1200.0f);
            }
            else
            {
                currentFrequency = targetFrequency;
            }
        }
        else
        {
            currentFrequency = targetFrequency;
        }

        // Apply Analog VCO Drift
        driftPhase += (juce::MathConstants<float>::twoPi * driftSpeed) / static_cast<float>(currentSampleRate);
        if (driftPhase >= juce::MathConstants<float>::twoPi) driftPhase -= juce::MathConstants<float>::twoPi;
        float driftSemitones = std::sin(driftPhase) * (analogDriftAmt * 0.12f);

        // Apply Pitch Bend Wheel + LFO 2 Vibrato + Drift pitch modulation
        float vibratoMod = lfo2Val * lfo2ToPitch * 2.0f;
        float totalPitchOffset = pitchWheelSemitones + vibratoMod + driftSemitones;
        float modulatedFreq = currentFrequency * std::pow(2.0f, totalPitchOffset / 12.0f);

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

        // Generate Layer B Hybrid Sample
        float layerBSample = 0.0f;
        if (layerBType > 0 && layerBMix > 0.001f)
        {
            if (layerBType == 1) // Grand Piano Hammer Strike
            {
                float pEnv = std::exp(-layerBTime / 0.8f);
                layerBPhase += (juce::MathConstants<float>::twoPi * pitchFreq) / static_cast<float>(currentSampleRate);
                layerBSample = (std::sin(layerBPhase) * 0.7f + std::sin(layerBPhase * 2.0f) * 0.3f + std::sin(layerBPhase * 3.0f) * 0.15f) * pEnv;
            }
            else if (layerBType == 2) // Glass FM Bell
            {
                float bEnv = std::exp(-layerBTime / 1.5f);
                layerBPhase += (juce::MathConstants<float>::twoPi * pitchFreq) / static_cast<float>(currentSampleRate);
                float mod = std::sin(layerBPhase * 3.5f) * 2.5f * bEnv;
                layerBSample = std::sin(layerBPhase + mod) * bEnv;
            }
            else if (layerBType == 3) // Vocal Formant Chop
            {
                float vEnv = std::exp(-layerBTime / 0.45f);
                layerBPhase += (juce::MathConstants<float>::twoPi * pitchFreq) / static_cast<float>(currentSampleRate);
                layerBSample = (std::sin(layerBPhase * 2.0f) + std::sin(layerBPhase * 4.0f) * 0.5f) * vEnv;
            }
            else if (layerBType == 4) // White Noise
            {
                layerBSample = (random.nextFloat() * 2.0f - 1.0f) * 0.6f;
            }
            else if (layerBType == 5) // Vinyl Dust
            {
                float crackle = (random.nextFloat() > 0.985f) ? (random.nextFloat() * 2.0f - 1.0f) : 0.0f;
                layerBSample = crackle * 0.8f;
            }
            layerBTime += 1.0f / static_cast<float>(currentSampleRate);
        }

        // Generate Transient Click Layer
        float clickSample = 0.0f;
        if (transientActive && transientLevel > 0.001f)
        {
            float tDecaySecs = transientDecay / 1000.0f;
            float tEnv = std::exp(-transientTime / tDecaySecs);
            
            if (transientType == 0) // Punch Click
            {
                float noise = random.nextFloat() * 2.0f - 1.0f;
                float hpVal = noise - lastTransientSample;
                lastTransientSample = noise;
                clickSample = hpVal * tEnv * transientLevel;
            }
            else if (transientType == 1) // Piano Hammer
            {
                float pitchEnv = std::exp(-transientTime / 0.005f);
                float freq = 4000.0f * pitchEnv + 300.0f;
                transientPhase += (juce::MathConstants<float>::twoPi * freq) / static_cast<float>(currentSampleRate);
                clickSample = std::sin(transientPhase) * tEnv * transientLevel;
            }
            else if (transientType == 2) // Vocal Snap
            {
                transientPhase += (juce::MathConstants<float>::twoPi * 1500.0f) / static_cast<float>(currentSampleRate);
                float raw = std::sin(transientPhase) * (1.0f + 0.3f * (random.nextFloat() - 0.5f));
                clickSample = raw * tEnv * transientLevel;
            }
            else if (transientType == 3) // 808 Tock
            {
                float pitchEnv = std::exp(-transientTime / 0.015f);
                float freq = 500.0f * pitchEnv + 50.0f;
                transientPhase += (juce::MathConstants<float>::twoPi * freq) / static_cast<float>(currentSampleRate);
                clickSample = std::sin(transientPhase) * tEnv * transientLevel;
            }
            else if (transientType == 4) // Analog Pop
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
            
            float rawSub = (subWave == 0) ? std::sin(subPhase) : ((subPhase < juce::MathConstants<float>::pi) ? 1.0f : -1.0f);
            rawSub = rawSub * (1.0f + subDrive * 3.0f);
            subSample = std::tanh(rawSub) * subLevel;
            
            subPhase += subInc;
            if (subPhase >= juce::MathConstants<float>::twoPi)
                subPhase -= juce::MathConstants<float>::twoPi;
        }

        // 1. Calculate Oscillator 2
        float osc2BaseFreq = pitchFreq * std::pow(2.0f, static_cast<float>(osc2Octave) + (osc2DetuneCents / 1200.0f));
        float osc2Inc = (juce::MathConstants<float>::twoPi * osc2BaseFreq) / static_cast<float>(currentSampleRate);
        float osc2Sample = generateMorphedSample(osc2Shape, osc2Phase) * osc2Level;

        osc2Phase += osc2Inc;
        if (osc2Phase >= juce::MathConstants<float>::twoPi)
            osc2Phase -= juce::MathConstants<float>::twoPi;

        // 2. Calculate Oscillator 1 with 7-Voice Stereo Unison, FM, and Hard Sync
        float osc1BaseFreq = pitchFreq * std::pow(2.0f, static_cast<float>(osc1Octave) + (osc1DetuneCents / 1200.0f));
        float modulatedOsc1Shape = juce::jlimit(0.0f, 3.0f, osc1Shape + lfo1Val * lfo1ToShape * 3.0f);

        float osc1Left = 0.0f;
        float osc1Right = 0.0f;

        for (int u = 0; u < numUnisonVoices; ++u)
        {
            float detuneOffset = unisonOffsets[u] * (unisonDetuneCents / 1200.0f);
            float uFreq = osc1BaseFreq * std::pow(2.0f, detuneOffset);

            float fmPhaseMod = osc2Sample * fmDepth * juce::MathConstants<float>::twoPi;
            float totalPhase = osc1Phases[u] + fmPhaseMod;
            while (totalPhase >= juce::MathConstants<float>::twoPi) totalPhase -= juce::MathConstants<float>::twoPi;
            while (totalPhase < 0.0f) totalPhase += juce::MathConstants<float>::twoPi;

            float uSample = generateMorphedSample(modulatedOsc1Shape, totalPhase);

            float panL = (numUnisonVoices > 1) ? unisonPansL[u] : 0.5f;
            float panR = (numUnisonVoices > 1) ? unisonPansR[u] : 0.5f;

            osc1Left  += uSample * panL;
            osc1Right += uSample * panR;

            float uInc = (juce::MathConstants<float>::twoPi * uFreq) / static_cast<float>(currentSampleRate);
            osc1Phases[u] += uInc;

            if (oscSyncActive && osc2Phase < osc2Inc)
                osc1Phases[u] = 0.0f;
            else if (osc1Phases[u] >= juce::MathConstants<float>::twoPi)
                osc1Phases[u] -= juce::MathConstants<float>::twoPi;
        }

        float unisonNorm = 1.0f / std::sqrt(static_cast<float>(numUnisonVoices));
        osc1Left  *= (unisonNorm * osc1Level);
        osc1Right *= (unisonNorm * osc1Level);

        // Blend Layer A (Synth) + Layer B (Acoustic / Sound Stacker) + Click Transient
        float bGain = layerBSample * layerBMix;
        float aScale = 1.0f - (layerBMix * 0.4f);
        float mixL = (osc1Left * aScale) + bGain + (osc2Sample * 0.5f) + clickSample;
        float mixR = (osc1Right * aScale) + bGain + (osc2Sample * 0.5f) + clickSample;

        // 3. Dynamic Filter Processing
        float keyTrackOffset = (static_cast<float>(midiNote) - 60.0f) * (filterKeyTrack * 100.0f);
        float lfoCutoffMod = lfo1Val * lfo1ToCutoff * 4000.0f;
        float envCutoffMod = filterVal * filterEnvAmount * 8000.0f;
        float finalCutoff = baseCutoffHz + keyTrackOffset + lfoCutoffMod + envCutoffMod;
        finalCutoff = juce::jlimit(20.0f, 20000.0f, finalCutoff);

        if (filterDrive > 1.01f)
        {
            mixL = std::tanh(mixL * filterDrive);
            mixR = std::tanh(mixR * filterDrive);
        }

        float outL = mixL;
        float outR = mixR;

        if (filterMode < 4) // Ladder Filter
        {
            filter.setCutoffFrequencyHz(finalCutoff);

            if (numBufferChannels == 1)
            {
                float* channelData[] = { &outL };
                juce::dsp::AudioBlock<float> block(channelData, 1, 1);
                juce::dsp::ProcessContextReplacing<float> context(block);
                filter.process(context);
            }
            else if (numBufferChannels >= 2)
            {
                float* channelData[] = { &outL, &outR };
                juce::dsp::AudioBlock<float> block(channelData, 2, 1);
                juce::dsp::ProcessContextReplacing<float> context(block);
                filter.process(context);
            }
        }
        else if (filterMode == 4) // Notch
        {
            float Q = 0.5f + filterResonance * 9.5f;
            notchFilterL.setNotch(static_cast<float>(currentSampleRate), finalCutoff, Q);
            notchFilterR.setNotch(static_cast<float>(currentSampleRate), finalCutoff, Q);
            outL = notchFilterL.process(mixL);
            outR = notchFilterR.process(mixR);
        }
        else if (filterMode == 5) // Formant
        {
            static const float f1Table[5] = { 800.0f, 400.0f, 250.0f, 450.0f, 325.0f };
            static const float f2Table[5] = { 1200.0f, 2000.0f, 2100.0f, 800.0f, 700.0f };
            static const float f3Table[5] = { 2500.0f, 2800.0f, 3000.0f, 2500.0f, 2550.0f };

            float m = juce::jlimit(0.0f, 4.0f, formantMorph + filterVal * filterEnvAmount * 2.0f);
            int idx = static_cast<int>(m);
            float frac = m - static_cast<float>(idx);
            int nextIdx = std::min(4, idx + 1);

            float f1 = f1Table[idx] * (1.0f - frac) + f1Table[nextIdx] * frac;
            float f2 = f2Table[idx] * (1.0f - frac) + f2Table[nextIdx] * frac;
            float f3 = f3Table[idx] * (1.0f - frac) + f3Table[nextIdx] * frac;

            formantF1L.setCutoffFrequency(f1); formantF1R.setCutoffFrequency(f1);
            formantF2L.setCutoffFrequency(f2); formantF2R.setCutoffFrequency(f2);
            formantF3L.setCutoffFrequency(f3); formantF3R.setCutoffFrequency(f3);

            formantF1L.setResonance(4.0f + filterResonance * 6.0f);
            formantF1R.setResonance(4.0f + filterResonance * 6.0f);
            formantF2L.setResonance(4.0f + filterResonance * 6.0f);
            formantF2R.setResonance(4.0f + filterResonance * 6.0f);
            formantF3L.setResonance(4.0f + filterResonance * 6.0f);
            formantF3R.setResonance(4.0f + filterResonance * 6.0f);

            float vL = formantF1L.processSample(0, mixL) * 1.2f 
                     + formantF2L.processSample(0, mixL) * 0.8f 
                     + formantF3L.processSample(0, mixL) * 0.5f;

            float vR = formantF1R.processSample(0, mixR) * 1.2f 
                     + formantF2R.processSample(0, mixR) * 0.8f 
                     + formantF3R.processSample(0, mixR) * 0.5f;

            outL = vL;
            outR = vR;
        }

        // 4. Amp & Velocity
        float gain = ampVal * noteVelocity;
        outL *= gain;
        outR *= gain;

        // Auto-Pan
        float panMod = lfo2Val * lfo2ToPan * 0.5f;
        float finalPanL = juce::jlimit(0.0f, 1.0f, 0.5f - panMod);
        float finalPanR = juce::jlimit(0.0f, 1.0f, 0.5f + panMod);

        outL *= (finalPanL * 2.0f);
        outR *= (finalPanR * 2.0f);

        // Mix output into synth channels (0 & 1)
        outputBuffer.addSample(0, startSample + sample, outL);
        if (numBufferChannels > 1)
            outputBuffer.addSample(1, startSample + sample, outR);

        // Sub mono anchor into channel 2
        if (numBufferChannels > 2 && subLevel > 0.001f)
            outputBuffer.addSample(2, startSample + sample, subSample * gain);
    }
}
