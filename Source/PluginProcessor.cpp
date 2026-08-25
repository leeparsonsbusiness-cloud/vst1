#include "PluginProcessor.h"
#include "PluginEditor.h"

KeshaZeddSynthAudioProcessor::KeshaZeddSynthAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    presetManager.setAPVTS(&apvts);

    // Create 8 Polyphonic Synth Voices with 7-Voice Stereo Unison each
    for (int i = 0; i < 8; ++i)
        synth.addVoice(new SynthVoice());

    synth.addSound(new SynthSound());
}

KeshaZeddSynthAudioProcessor::~KeshaZeddSynthAudioProcessor() = default;

void KeshaZeddSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
    }

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    postFX.prepare(spec);
    autoMaster.prepare(spec);
    sampleLayer.setSampleRate(sampleRate);
    expandedFX.prepare(spec);
    producerFlavor.prepare(spec);
    riserEngine.prepare(sampleRate);
    glitchEngine.prepare(sampleRate);
    glitterReverb.prepare(sampleRate);
    flEchoDelay.prepare(sampleRate);

    zeddifyEngine.setSampleRate(sampleRate);
    autoBassEngine.setSampleRate(sampleRate);
    hookEngine.setSampleRate(sampleRate);
    counterMelodyEngine.setSampleRate(sampleRate);

    heldNotes.clear();
    activeLegatoNote = -1;
    lastMidiNoteNumber = -1;
}

void KeshaZeddSynthAudioProcessor::releaseResources()
{
    postFX.reset();
    autoMaster.reset();
}

bool KeshaZeddSynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void KeshaZeddSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Count currently active voices for UI readout
    int currentActiveVoices = 0;
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            if (voice->isVoiceActive())
                currentActiveVoices++;
        }
    }
    activeVoiceCount.store(currentActiveVoices);

    // Query Host Playhead info (BPM, transport position)
    double bpm = 120.0;
    double timeInSeconds = 0.0;
    double ppqPosition = 0.0;
    bool isPlaying = false;

    if (auto* playHead = getPlayHead())
    {
        if (auto position = playHead->getPosition())
        {
            if (position->getBpm().hasValue())
                bpm = *position->getBpm();
            if (position->getTimeInSeconds().hasValue())
                timeInSeconds = *position->getTimeInSeconds();
            if (position->getPpqPosition().hasValue())
                ppqPosition = *position->getPpqPosition();
            isPlaying = position->getIsPlaying();
        }
    }

    // Pass Host BPM to Engines
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->updateParameters(apvts);
            voice->setHostInfo(bpm);
        }
    }
    zeddifyEngine.setBpm(bpm);
    autoBassEngine.setBpm(bpm);
    glitchEngine.setBpm(bpm);
    hookEngine.setBpm(bpm);
    counterMelodyEngine.setBpm(bpm);

    auto getVal = [&](const char* id, float defaultVal) -> float {
        if (auto* p = apvts.getRawParameterValue(id))
            return p->load();
        return defaultVal;
    };

    // Update Zeddify Pattern Style
    int zeddStyle = static_cast<int>(getVal("zeddify_style", 0.0f));
    zeddifyEngine.setPatternStyle(zeddStyle);

    // 1. FL Studio Time Shift / Gate
    float timeShiftMs = getVal("fl_time_shift", 0.0f);
    int shiftSamples = static_cast<int>((timeShiftMs * 0.001f) * getSampleRate());
    if (shiftSamples > 0)
    {
        juce::MidiBuffer shifted;
        for (const auto metadata : midiMessages)
        {
            int pos = std::min(buffer.getNumSamples() - 1, metadata.samplePosition + shiftSamples);
            shifted.addEvent(metadata.getMessage(), pos);
        }
        midiMessages.swapWith(shifted);
    }

    // 2. Smart Key & Scale Lock + Easy Key Remapper + Chord Strummer
    int scaleRoot = static_cast<int>(getVal("scale_root", 0.0f));
    int scaleType = static_cast<int>(getVal("scale_type", 0.0f));
    float strumMs = getVal("chord_strum_ms", 0.0f);
    bool easyKeyActive = getVal("easy_key_active", 0.0f) > 0.5f;
    scaleManager.processMidi(midiMessages, scaleRoot, scaleType, strumMs, easyKeyActive, getSampleRate(), buffer.getNumSamples());

    // 3. One-Finger Chord Progression Engine
    int chordProg = static_cast<int>(getVal("chord_prog_preset", 0.0f));
    chordProgEngine.processMidi(midiMessages, chordProg, scaleRoot, scaleType, buffer.getNumSamples());

    // 4. Topline Hook Generator Engine
    bool hookActive = getVal("hook_generator_active", 0.0f) > 0.5f;
    hookEngine.processMidi(midiMessages, hookActive, scaleRoot, scaleType, buffer.getNumSamples());

    // 5. Counter-Melody / Answer Fill Engine
    bool counterActive = getVal("counter_melody_active", 0.0f) > 0.5f;
    counterMelodyEngine.processMidi(midiMessages, counterActive, scaleRoot, scaleType, buffer.getNumSamples());

    // 6. Real-Time Auto-Harmonizer
    int harmMode = static_cast<int>(getVal("harmonizer_mode", 0.0f));
    harmonizerEngine.processMidi(midiMessages, harmMode, scaleRoot, scaleType);

    // 7. Auto-Bassline Follower Engine
    int autoBassMode = static_cast<int>(getVal("auto_bass_mode", 0.0f));
    autoBassEngine.processMidi(midiMessages, autoBassMode, buffer.getNumSamples());

    // 8. Process Zeddify Algorithmic Melody Engine on incoming MIDI
    bool zeddifyActive = getVal("zeddify_active", 0.0f) > 0.5f;
    zeddifyEngine.processMidiBlock(midiMessages, zeddifyActive, buffer.getNumSamples());

    // 9. Process Arpeggiator on MIDI
    int arpMode = static_cast<int>(getVal("arp_mode", 0.0f));
    arpeggiator.setMode(arpMode);
    arpeggiator.processMidiBlock(midiMessages, getSampleRate(), bpm, buffer.getNumSamples());

    // 10. Groove Feel Humanizer
    float humanizeAmt = getVal("humanize_amount", 0.0f);
    humanizerEngine.processMidi(midiMessages, humanizeAmt, getSampleRate(), buffer.getNumSamples());

    int playMode = static_cast<int>(getVal("play_mode", 0.0f));
    float glideTime = getVal("glide_time", 80.0f);
    bool chordMode = getVal("chord_mode", 0.0f) > 0.5f;
    int chordType = static_cast<int>(getVal("chord_type", 0.0f));
    bool cutSelf = getVal("fl_cut_self", 0.0f) > 0.5f;

    // 11. Smart Chord Stacking
    juce::MidiBuffer processedMidi;
    if (chordMode && chordProg == 0)
    {
        for (const auto metadata : midiMessages)
        {
            auto message = metadata.getMessage();
            auto samplePos = metadata.samplePosition;

            if (message.isNoteOn() || message.isNoteOff())
            {
                int root = message.getNoteNumber();
                float vel = message.getFloatVelocity();
                
                std::vector<int> offsets;
                if (chordType == 0)
                    offsets = { 0, 4, 7 };
                else
                    offsets = { 0, 3, 7 };
                
                for (int offset : offsets)
                {
                    int chordNote = root + offset;
                    if (chordNote >= 0 && chordNote <= 127)
                    {
                        if (message.isNoteOn())
                            processedMidi.addEvent(juce::MidiMessage::noteOn(message.getChannel(), chordNote, vel), samplePos);
                        else
                            processedMidi.addEvent(juce::MidiMessage::noteOff(message.getChannel(), chordNote), samplePos);
                    }
                }
            }
            else
            {
                processedMidi.addEvent(message, samplePos);
            }
        }
    }
    else
    {
        processedMidi = midiMessages;
    }

    // 12. Play Mode Routing (Poly, Mono Legato, Mono Retrig) & Voice Cut-Self
    juce::MidiBuffer finalMidi;
    if (playMode == 1) // Mono Legato
    {
        for (const auto metadata : processedMidi)
        {
            auto message = metadata.getMessage();
            auto samplePos = metadata.samplePosition;
            
            if (message.isNoteOn())
            {
                int note = message.getNoteNumber();
                
                heldNotes.erase(std::remove(heldNotes.begin(), heldNotes.end(), note), heldNotes.end());
                heldNotes.push_back(note);
                
                if (activeLegatoNote != -1)
                {
                    float targetFreq = static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(note));
                    for (int i = 0; i < synth.getNumVoices(); ++i)
                    {
                        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
                        {
                            if (voice->isVoiceActive())
                                voice->glideTo(targetFreq, glideTime);
                        }
                    }
                    lastMidiNoteNumber = note;
                }
                else
                {
                    activeLegatoNote = note;
                    lastMidiNoteNumber = note;
                    
                    for (int i = 0; i < synth.getNumVoices(); ++i)
                    {
                        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
                            voice->glideTo(static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(note)), 0.0f);
                    }
                    finalMidi.addEvent(message, samplePos);
                }
            }
            else if (message.isNoteOff())
            {
                int note = message.getNoteNumber();
                heldNotes.erase(std::remove(heldNotes.begin(), heldNotes.end(), note), heldNotes.end());
                
                if (note == activeLegatoNote || note == lastMidiNoteNumber)
                {
                    if (heldNotes.empty())
                    {
                        activeLegatoNote = -1;
                        lastMidiNoteNumber = -1;
                        finalMidi.addEvent(message, samplePos);
                    }
                    else
                    {
                        int prevNote = heldNotes.back();
                        float targetFreq = static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(prevNote));
                        for (int i = 0; i < synth.getNumVoices(); ++i)
                        {
                            if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
                            {
                                if (voice->isVoiceActive())
                                    voice->glideTo(targetFreq, glideTime);
                            }
                        }
                        lastMidiNoteNumber = prevNote;
                    }
                }
            }
            else
            {
                finalMidi.addEvent(message, samplePos);
            }
        }
    }
    else if (playMode == 2) // Mono Retrigger
    {
        for (const auto metadata : processedMidi)
        {
            auto message = metadata.getMessage();
            auto samplePos = metadata.samplePosition;
            
            if (message.isNoteOn())
            {
                int note = message.getNoteNumber();
                heldNotes.erase(std::remove(heldNotes.begin(), heldNotes.end(), note), heldNotes.end());
                heldNotes.push_back(note);
                
                if (lastMidiNoteNumber != -1)
                    finalMidi.addEvent(juce::MidiMessage::noteOff(message.getChannel(), lastMidiNoteNumber), samplePos);
                
                lastMidiNoteNumber = note;
                finalMidi.addEvent(message, samplePos);
            }
            else if (message.isNoteOff())
            {
                int note = message.getNoteNumber();
                heldNotes.erase(std::remove(heldNotes.begin(), heldNotes.end(), note), heldNotes.end());
                
                if (note == lastMidiNoteNumber)
                {
                    finalMidi.addEvent(message, samplePos);
                    if (!heldNotes.empty())
                    {
                        int prevNote = heldNotes.back();
                        lastMidiNoteNumber = prevNote;
                        finalMidi.addEvent(juce::MidiMessage::noteOn(message.getChannel(), prevNote, 0.85f), samplePos);
                    }
                    else
                    {
                        lastMidiNoteNumber = -1;
                    }
                }
            }
            else
            {
                finalMidi.addEvent(message, samplePos);
            }
        }
    }
    else // Polyphonic mode
    {
        if (cutSelf)
        {
            for (const auto metadata : processedMidi)
            {
                auto msg = metadata.getMessage();
                if (msg.isNoteOn())
                {
                    for (int i = 0; i < synth.getNumVoices(); ++i)
                    {
                        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
                            voice->stopNote(0.0f, false);
                    }
                }
            }
        }

        heldNotes.clear();
        activeLegatoNote = -1;
        lastMidiNoteNumber = -1;
        finalMidi = processedMidi;
    }

    // 13. Check Riser Pitch Bend Offset
    bool riserActive = getVal("riser_active", 0.0f) > 0.5f;
    float riserProgress = getVal("riser_progress", 0.0f);
    if (riserActive && riserProgress > 0.001f)
    {
        float riserSemitones = riserEngine.getPitchOffsetSemitones(riserProgress);
        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
                voice->setPitchBend(riserSemitones);
        }
    }

    // 14. Render Voices into 3-channel temporary buffer (Synth in 0/1, Mono Sub in 2)
    const int numSamples = buffer.getNumSamples();
    juce::AudioBuffer<float> tempBuffer(3, numSamples);
    tempBuffer.clear();

    synth.renderNextBlock(tempBuffer, finalMidi, 0, numSamples);

    // Copy temporary buffer channels 0 & 1 back to main buffer
    buffer.copyFrom(0, 0, tempBuffer, 0, 0, numSamples);
    if (totalNumOutputChannels > 1)
        buffer.copyFrom(1, 0, tempBuffer, 1, 0, numSamples);

    // 15. Process Post FX Chain (Chorus, Delay, Reverb, Trash/Gloss, Sidechain, Mono-Maker)
    postFX.process(buffer, apvts, timeInSeconds, bpm, ppqPosition, isPlaying);

    // 16. Mix Sub-Oscillator (Channel 2 of tempBuffer) back directly into output buffer
    float subLevel = getVal("sub_level", 0.0f);
    if (subLevel > 0.001f)
    {
        buffer.addFrom(0, 0, tempBuffer, 2, 0, numSamples);
        if (totalNumOutputChannels > 1)
            buffer.addFrom(1, 0, tempBuffer, 2, 0, numSamples);
    }

    // 17. Expanded FX Suite (Bitcrusher, Wavefolder, Phaser/Flanger)
    expandedFX.process(buffer, apvts);

    // 18. "Glitter Cloud" Granular Shimmer Reverb
    float glitterMix = getVal("glitter_mix", 0.0f);
    float grainSize = getVal("glitter_grain_size", 45.0f);
    glitterReverb.process(buffer, glitterMix, grainSize);

    // 19. FL Studio Echo Delay & Fat Mode Engine
    float echoFeed = getVal("fl_echo_feed", 0.0f);
    float echoTime = getVal("fl_echo_time", 250.0f);
    float echoPan = getVal("fl_echo_pan", 0.0f);
    float echoPitch = getVal("fl_echo_pitch", 0.0f);
    int echoCount = static_cast<int>(getVal("fl_echo_count", 0.0f));
    bool pingPong = getVal("fl_echo_pingpong", 0.0f) > 0.5f;
    bool fatMode = getVal("fl_echo_fat", 0.0f) > 0.5f;

    flEchoDelay.process(buffer, echoFeed, echoTime, echoPan, echoPitch, echoCount, pingPong, fatMode);

    // 20. Producer Flavor Processing
    int flavorIdx = static_cast<int>(getVal("producer_flavor", 0.0f));
    float flavorIntensity = getVal("producer_flavor_intensity", 0.0f);
    producerFlavor.process(buffer, flavorIdx, flavorIntensity);

    // 21. The Riser / Buildup Processing
    riserEngine.process(buffer, riserActive, riserProgress, bpm);

    // 22. Momentary Glitch & Tape-Stop Performance Ribbon
    int glitchMode = static_cast<int>(getVal("glitch_mode", 0.0f));
    glitchEngine.setMode(glitchMode);
    glitchEngine.process(buffer);

    // 23. Auto-Master One-Click Polish Stage
    bool autoMasterActive = apvts.getRawParameterValue("auto_master_active")->load() > 0.5f;
    float autoMasterIntensity = apvts.getRawParameterValue("auto_master_intensity")->load();
    autoMaster.process(buffer, autoMasterActive, autoMasterIntensity);

    // 24. Master Volume scaling
    float masterVol = apvts.getRawParameterValue("master_vol")->load();
    buffer.applyGain(masterVol);

    // 25. Stream to Visualizer FFT & Scope
    visualizer.pushSampleBlock(buffer.getReadPointer(0), numSamples);
    float currentCutoff = apvts.getRawParameterValue("filter_cutoff")->load();
    float currentRes = apvts.getRawParameterValue("filter_res")->load();
    visualizer.setFilterCutoffAndRes(currentCutoff, currentRes);

    // 26. Track Peak Levels for UI VU Meter
    float peakL = buffer.getMagnitude(0, 0, numSamples);
    float peakR = (totalNumOutputChannels > 1) ? buffer.getMagnitude(1, 0, numSamples) : peakL;
    outputLevelL.store(peakL);
    outputLevelR.store(peakR);
}

void KeshaZeddSynthAudioProcessor::randomizeParameters()
{
    juce::Random& rand = juce::Random::getSystemRandom();
    for (auto* param : getParameters())
    {
        if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
        {
            if (rangedParam->paramID == "preset" ||
                rangedParam->paramID == "master_vol" ||
                rangedParam->paramID == "play_mode" ||
                rangedParam->paramID == "chord_mode" ||
                rangedParam->paramID == "chord_type" ||
                rangedParam->paramID == "ui_theme" ||
                rangedParam->paramID == "chord_prog_preset" ||
                rangedParam->paramID == "glitch_mode" ||
                rangedParam->paramID == "easy_key_active" ||
                rangedParam->paramID == "hook_generator_active" ||
                rangedParam->paramID == "counter_melody_active")
            {
                continue;
            }

            float currentVal = rangedParam->getValue();
            float offset = rand.nextFloat() * 0.3f - 0.15f;
            float newVal = juce::jlimit(0.0f, 1.0f, currentVal + offset);
            rangedParam->setValueNotifyingHost(newVal);
        }
    }
}

void KeshaZeddSynthAudioProcessor::setCurrentProgram(int index)
{
    loadPreset(index);
}

void KeshaZeddSynthAudioProcessor::loadPreset(int index)
{
    if (index >= 0 && index < presetManager.getNumPresets())
    {
        currentPresetIndex = index;
        presetManager.applyPreset(index, apvts);
    }
}

juce::AudioProcessorEditor* KeshaZeddSynthAudioProcessor::createEditor()
{
    return new KeshaZeddSynthAudioProcessorEditor(*this);
}

void KeshaZeddSynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void KeshaZeddSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KeshaZeddSynthAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout KeshaZeddSynthAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Preset & Master Volume
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("preset", 1), "Preset", 0, 83, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("master_vol", 1), "Master Volume", 0.0f, 1.0f, 0.8f));

    // Oscillator 1 & Unison
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc1_shape", 1), "Osc 1 Shape Morph", 0.0f, 3.0f, 2.0f));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc1_octave", 1), "Osc 1 Octave", -2, 2, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc1_detune", 1), "Osc 1 Detune", -100.0f, 100.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc1_level", 1), "Osc 1 Level", 0.0f, 1.0f, 0.85f));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("unison_count", 1), "Unison Voices", 1, 7, 7));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("unison_detune", 1), "Unison Spread", 0.0f, 100.0f, 20.0f));

    // Oscillator 2 & Modulations
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc2_shape", 1), "Osc 2 Shape Morph", 0.0f, 3.0f, 2.0f));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc2_octave", 1), "Osc 2 Octave", -2, 2, -1));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc2_detune", 1), "Osc 2 Detune", -100.0f, 100.0f, 5.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc2_level", 1), "Osc 2 Level", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc_fm_depth", 1), "FM Cross Mod", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc_sync", 1), "Osc Hard Sync", 0, 1, 0));

    // Transient Shaper Attack Layer
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("transient_type", 1), "Transient Type", 0, 4, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("transient_level", 1), "Transient Level", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("transient_decay", 1), "Transient Decay", 5.0f, 100.0f, 15.0f));

    // Sub-Bass Mono Anchor
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("sub_wave", 1), "Sub Waveform", 0, 1, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("sub_octave", 1), "Sub Octave", -2, 0, -1));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("sub_level", 1), "Sub Level", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("sub_drive", 1), "Sub Drive", 0.0f, 1.0f, 0.0f));

    // Advanced Filter Section
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("filter_mode", 1), "Filter Mode", 0, 5, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filter_cutoff", 1), "Filter Cutoff", 
                                                                 juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 4500.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filter_res", 1), "Filter Resonance", 0.0f, 0.99f, 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filter_drive", 1), "Filter Drive", 1.0f, 10.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filter_env_amt", 1), "Filter Env Amount", -1.0f, 1.0f, 0.4f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filter_key_track", 1), "Filter Key Track", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filter_lfo_mod", 1), "Filter LFO Mod", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("formant_morph", 1), "Formant Morph", 0.0f, 4.0f, 0.0f));

    // Dual Synced LFOs
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("lfo1_rate", 1), "LFO 1 Rate", 0, 11, 2));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("lfo1_wave", 1), "LFO 1 Waveform", 0, 4, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("lfo1_to_cutoff", 1), "LFO 1 to Cutoff", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("lfo1_to_shape", 1), "LFO 1 to Shape", 0.0f, 1.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("lfo2_rate", 1), "LFO 2 Rate", 0, 11, 3));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("lfo2_wave", 1), "LFO 2 Waveform", 0, 4, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("lfo2_to_pitch", 1), "LFO 2 to Pitch", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("lfo2_to_pan", 1), "LFO 2 to Pan", 0.0f, 1.0f, 0.0f));

    // Envelopes & Curve Shapers (FL Studio Style AHDSR + Tension)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("amp_attack", 1), "Amp Attack", 
                                                                 juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("amp_decay", 1), "Amp Decay", 
                                                                 juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.4f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("amp_sustain", 1), "Amp Sustain", 0.0f, 1.0f, 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("amp_release", 1), "Amp Release", 
                                                                 juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("env_delay", 1), "Env Delay", 0.0f, 2.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("env_hold", 1), "Env Hold", 0.0f, 2.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("env_dec_tension", 1), "Decay Tension", 0.1f, 4.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("env_rel_tension", 1), "Release Tension", 0.1f, 4.0f, 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filter_attack", 1), "Filter Attack", 
                                                                 juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filter_decay", 1), "Filter Decay", 
                                                                 juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filter_sustain", 1), "Filter Sustain", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filter_release", 1), "Filter Release", 
                                                                 juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.4f));

    // Play Mode, Portamento & Pitch Bend Range
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("play_mode", 1), "Play Mode", 0, 2, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("glide_mode", 1), "Glide Mode", 0, 2, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("glide_time", 1), "Glide Time", 
                                                                 juce::NormalisableRange<float>(0.0f, 2000.0f, 0.5f, 0.4f), 80.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("pitch_bend_range", 1), "Pitch Bend Range", 1.0f, 24.0f, 2.0f));

    // Pitch Drop
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("pitch_drop_active", 1), "Pitch Drop Active", 0, 1, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("pitch_drop_octaves", 1), "Pitch Drop Octaves", 1.0f, 2.0f, 2.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("pitch_drop_time", 1), "Pitch Drop Time", 20.0f, 80.0f, 50.0f));

    // Smart Chord Engine
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("chord_mode", 1), "Chord Mode", 0, 1, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("chord_type", 1), "Chord Type", 0, 1, 0));

    // 2D Trash vs Gloss
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("trash_gloss_x", 1), "Trash (X)", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("trash_gloss_y", 1), "Gloss (Y)", 0.0f, 1.0f, 0.0f));

    // Smart Mono-Maker
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("mono_maker_active", 1), "Mono Maker Active", 0, 1, 1));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("mono_maker_frequency", 1), "Mono Maker Frequency", 80.0f, 250.0f, 120.0f));

    // Host Synced Ducking Pumper
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("pump_active", 1), "Pump Active", 0, 1, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("pump_division", 1), "Pump Division", 0, 2, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("pump_depth", 1), "Pump Depth", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("pump_curve", 1), "Pump Curve", 0.1f, 5.0f, 2.0f));

    // FX Chain (Chorus, Delay, Reverb)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_drive", 1), "FX Drive", 1.0f, 10.0f, 1.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_chorus_rate", 1), "Chorus Rate", 0.0f, 10.0f, 1.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_chorus_depth", 1), "Chorus Depth", 0.0f, 1.0f, 0.35f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_chorus_mix", 1), "Chorus Mix", 0.0f, 1.0f, 0.4f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_delay_time", 1), "Delay Time", 10.0f, 1000.0f, 350.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_delay_feedback", 1), "Delay Feedback", 0.0f, 0.99f, 0.4f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_delay_mix", 1), "Delay Mix", 0.0f, 1.0f, 0.35f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_delay_color", 1), "Delay Color", 0.0f, 1.0f, 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_reverb_decay", 1), "Reverb Decay", 0.1f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_reverb_damping", 1), "Reverb Damping", 0.0f, 1.0f, 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_reverb_width", 1), "Reverb Width", 0.0f, 1.0f, 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_reverb_mix", 1), "Reverb Mix", 0.0f, 1.0f, 0.15f));

    // Macros
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("macro_drop", 1), "The Drop", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("macro_punch", 1), "Macro Punch", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("macro_grit", 1), "Macro Grit", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("macro_space", 1), "Macro Space", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("macro_width", 1), "Macro Width", 0.0f, 1.0f, 0.0f));

    // Zeddify & AutoMaster Workflow Features
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("zeddify_active", 1), "Zeddify Active", 0, 1, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("auto_master_active", 1), "Auto Master Active", 0, 1, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("auto_master_intensity", 1), "Auto Master Intensity", 0.0f, 1.0f, 0.8f));

    // Arpeggiator Parameters
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("arp_mode", 1), "Arp Mode", 0, 5, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("arp_rate", 1), "Arp Rate", 0, 5, 2));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("arp_octaves", 1), "Arp Octaves", 1, 4, 1));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("arp_swing", 1), "Arp Swing", 0.0f, 0.75f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("arp_gate", 1), "Arp Gate", 0.1f, 1.0f, 0.8f));

    // Bitcrusher Parameters
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("bitcrush_active", 1), "Bitcrush Active", 0, 1, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("bitcrush_bits", 1), "Bit Depth", 1, 16, 8));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("bitcrush_downsample", 1), "Downsample", 1, 32, 1));

    // Wavefolder Parameters
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("wavefold_active", 1), "Wavefold Active", 0, 1, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("wavefold_drive", 1), "Wavefold Drive", 1.0f, 10.0f, 1.0f));

    // Phaser Parameters
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("phaser_active", 1), "Phaser Active", 0, 1, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("phaser_rate", 1), "Phaser Rate", 0.1f, 10.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("phaser_depth", 1), "Phaser Depth", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("phaser_feedback", 1), "Phaser Feedback", 0.0f, 0.95f, 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("phaser_mix", 1), "Phaser Mix", 0.0f, 1.0f, 0.0f));

    // ----------------------------------------------------
    // PRODUCER SUITE EXPANSION PARAMETERS
    // ----------------------------------------------------
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("producer_flavor", 1), "Producer Flavor", 0, 4, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("producer_flavor_intensity", 1), "Producer Flavor Intensity", 0.0f, 1.0f, 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("riser_active", 1), "The Riser Active", 0, 1, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("riser_progress", 1), "The Riser Progress", 0.0f, 1.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("scale_root", 1), "Scale Root Key", 0, 11, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("scale_type", 1), "Scale Type Lock", 0, 6, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("chord_strum_ms", 1), "Chord Strum Humanize", 0.0f, 100.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("zeddify_style", 1), "Zeddify Pattern Style", 0, 11, 0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("analog_drift", 1), "Analog VCO Drift", 0.0f, 1.0f, 0.25f));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("layer_b_type", 1), "Layer B Hybrid Type", 0, 5, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("layer_b_mix", 1), "Layer B Hybrid Mix", 0.0f, 1.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("ui_theme", 1), "UI Theme", 0, 3, 0));

    // Songwriting Suite Additions
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("chord_prog_preset", 1), "Chord Sets Progression", 0, 8, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("harmonizer_mode", 1), "Auto Harmonizer", 0, 4, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("auto_bass_mode", 1), "Auto Bassline Follower", 0, 4, 0));

    // Performance & Spatial Shimmer Additions
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("glitch_mode", 1), "Glitch Mode", 0, 4, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("glitter_mix", 1), "Glitter Shimmer Mix", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("glitter_grain_size", 1), "Glitter Grain Size", 20.0f, 80.0f, 40.0f));

    // Ultimate Melody Suite Additions
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("easy_key_active", 1), "Easy Key Active", 0, 1, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("hook_generator_active", 1), "Hook Generator Active", 0, 1, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("hook_mood", 1), "Hook Mood", 0, 4, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("counter_melody_active", 1), "Counter Melody Active", 0, 1, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("humanize_amount", 1), "Humanize Amount", 0.0f, 1.0f, 0.0f));

    // FL Studio Channel Settings Suite Additions
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fl_echo_feed", 1), "Echo Feedback", 0.0f, 0.95f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fl_echo_time", 1), "Echo Time", 10.0f, 1000.0f, 250.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fl_echo_pan", 1), "Echo Pan Spread", -1.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fl_echo_pitch", 1), "Echo Pitch Shift", -12.0f, 12.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("fl_echo_count", 1), "Echoes Count", 0, 10, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("fl_echo_pingpong", 1), "Echo Ping Pong", 0, 1, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("fl_echo_fat", 1), "Echo Fat Mode", 0, 1, 0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fl_time_shift", 1), "Time Shift", 0.0f, 100.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("fl_cut_self", 1), "Cut Self", 0, 1, 0));

    return { params.begin(), params.end() };
}
