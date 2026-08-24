#include "PluginProcessor.h"
#include "PluginEditor.h"

KeshaZeddSynthAudioProcessor::KeshaZeddSynthAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    presetManager.setAPVTS(&apvts);

    synth.clearVoices();
    for (int i = 0; i < 8; ++i)
        synth.addVoice(new SynthVoice());

    synth.clearSounds();
    synth.addSound(new SynthSound());
}

KeshaZeddSynthAudioProcessor::~KeshaZeddSynthAudioProcessor()
{
}

void KeshaZeddSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    // Prepare voices with 3 channels to accommodate the mono sub anchor in channel 2
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock, 3);
        }
    }

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    postFX.prepare(spec);
}

void KeshaZeddSynthAudioProcessor::releaseResources()
{
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
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = 0; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Update voice parameters from APVTS
    int activeCount = 0;
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->updateParameters(apvts);
            if (voice->isVoiceActive())
                activeCount++;
        }
    }
    activeVoiceCount.store(activeCount);

    // Read parameter values
    bool chordMode = apvts.getRawParameterValue("chord_mode")->load() > 0.5f;
    int chordType = apvts.getRawParameterValue("chord_type")->load();
    int playMode = apvts.getRawParameterValue("play_mode")->load();
    float glideTime = apvts.getRawParameterValue("glide_time")->load();

    if (chordMode)
        playMode = 0; // Force Poly mode if Chord Engine is active

    // 1. MIDI Chord Engine Transformation
    juce::MidiBuffer processedMidi;
    if (chordMode)
    {
        for (const auto metadata : midiMessages)
        {
            auto message = metadata.getMessage();
            auto samplePos = metadata.samplePosition;
            
            if (message.isNoteOn() || message.isNoteOff())
            {
                int root = message.getNoteNumber();
                float vel = message.isNoteOn() ? message.getFloatVelocity() : 0.0f;
                bool isMinor = (chordType == 1);
                
                int offsets[6] = {
                    -12,                        // Sub root
                    0,                          // Root
                    7,                          // 5th
                    12 + (isMinor ? 3 : 4),     // Mids (Minor or Major 3rd)
                    19,                         // Upper 5th
                    24 + (isMinor ? 10 : 11)    // Upper 7th
                };
                
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

    // 2. Legato Monophonic Glide Interception
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
                float vel = message.getFloatVelocity();
                
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
    else
    {
        heldNotes.clear();
        activeLegatoNote = -1;
        lastMidiNoteNumber = -1;
        
        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            {
                float noteFreq = static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(voice->getCurrentlyPlayingNote()));
                if (noteFreq > 0.0f)
                    voice->glideTo(noteFreq, 0.0f);
            }
        }
        finalMidi = processedMidi;
    }

    // 3. Render Voices into 3-channel temporary buffer (Synth in 0/1, Mono Sub in 2)
    const int numSamples = buffer.getNumSamples();
    juce::AudioBuffer<float> tempBuffer(3, numSamples);
    tempBuffer.clear();

    synth.renderNextBlock(tempBuffer, finalMidi, 0, numSamples);

    // Copy temporary buffer channels 0 & 1 back to main buffer
    buffer.copyFrom(0, 0, tempBuffer, 0, 0, numSamples);
    if (totalNumOutputChannels > 1)
        buffer.copyFrom(1, 0, tempBuffer, 1, 0, numSamples);

    // Get host playhead positioning
    double bpm = 120.0;
    double timeInSeconds = 0.0;
    double ppqPosition = 0.0;
    bool isPlaying = false;
    
    if (auto* playHead = getPlayHead())
    {
        if (auto positionInfo = playHead->getPosition())
        {
            bpm = positionInfo->getBpm().hasValue() ? *(positionInfo->getBpm()) : 120.0;
            timeInSeconds = positionInfo->getTimeInSeconds().hasValue() ? *(positionInfo->getTimeInSeconds()) : 0.0;
            ppqPosition = positionInfo->getPpqPosition().hasValue() ? *(positionInfo->getPpqPosition()) : 0.0;
            isPlaying = positionInfo->getIsPlaying();
        }
    }

    // 4. Process Post FX Chain (Chorus, Delay, Reverb, Trash/Gloss, Sidechain, Mono-Maker) on buffer
    postFX.process(buffer, apvts, timeInSeconds, bpm, ppqPosition, isPlaying);

    // 5. Mix Sub-Oscillator (Channel 2 of tempBuffer) back directly into output buffer (Stereo)
    float subLevel = apvts.getRawParameterValue("sub_level")->load();
    if (subLevel > 0.001f)
    {
        buffer.addFrom(0, 0, tempBuffer, 2, 0, numSamples);
        if (totalNumOutputChannels > 1)
            buffer.addFrom(1, 0, tempBuffer, 2, 0, numSamples);
    }

    // 6. Master Volume scaling
    float masterVol = apvts.getRawParameterValue("master_vol")->load();
    buffer.applyGain(masterVol);
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
                rangedParam->paramID == "chord_type")
            {
                continue;
            }

            float currentVal = rangedParam->getValue();
            float offset = rand.nextFloat() * 0.3f - 0.15f; // ±15% range
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
    if (xmlState != nullptr)
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
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("preset", 1), "Preset", 0, 19, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("master_vol", 1), "Master Volume", 0.0f, 1.0f, 0.8f));

    // Oscillator 1 & Unison
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc1_wave", 1), "Osc 1 Wave", 0, 4, 1));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc1_octave", 1), "Osc 1 Octave", -2, 2, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc1_detune", 1), "Osc 1 Detune", -100.0f, 100.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc1_level", 1), "Osc 1 Level", 0.0f, 1.0f, 0.85f));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("unison_count", 1), "Unison Voices", 1, 7, 7));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("unison_detune", 1), "Unison Spread", 0.0f, 100.0f, 20.0f));

    // Oscillator 2
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc2_wave", 1), "Osc 2 Wave", 0, 4, 1));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc2_octave", 1), "Osc 2 Octave", -2, 2, -1));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc2_detune", 1), "Osc 2 Detune", -100.0f, 100.0f, 5.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc2_level", 1), "Osc 2 Level", 0.0f, 1.0f, 0.5f));

    // Filter
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("filter_mode", 1), "Filter Mode", 0, 6, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filter_cutoff", 1), "Filter Cutoff", 
                                                                 juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 4500.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filter_res", 1), "Filter Resonance", 0.0f, 0.99f, 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filter_env_amt", 1), "Filter Env Amount", -1.0f, 1.0f, 0.4f));

    // Filter ADSR
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filter_attack", 1), "Filter Attack", 
                                                                 juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filter_decay", 1), "Filter Decay", 
                                                                 juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filter_sustain", 1), "Filter Sustain", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filter_release", 1), "Filter Release", 
                                                                 juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.4f));

    // Amp ADSR
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("amp_attack", 1), "Amp Attack", 
                                                                 juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("amp_decay", 1), "Amp Decay", 
                                                                 juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.4f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("amp_sustain", 1), "Amp Sustain", 0.0f, 1.0f, 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("amp_release", 1), "Amp Release", 
                                                                 juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.5f));

    // Post FX
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_drive", 1), "FX Drive", 1.0f, 10.0f, 1.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_chorus_rate", 1), "FX Chorus Rate", 0.0f, 10.0f, 1.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_chorus_depth", 1), "FX Chorus Depth", 0.0f, 1.0f, 0.35f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_chorus_mix", 1), "FX Chorus Mix", 0.0f, 1.0f, 0.4f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_delay_time", 1), "FX Delay Time", 10.0f, 1000.0f, 350.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_delay_feedback", 1), "FX Delay Feedback", 0.0f, 0.99f, 0.4f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_delay_mix", 1), "FX Delay Mix", 0.0f, 1.0f, 0.35f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_reverb_mix", 1), "FX Reverb Mix", 0.0f, 1.0f, 0.15f));

    // Transient Click Attack Layer
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("click_decay", 1), "Click Decay", 0.005f, 0.030f, 0.015f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("click_level", 1), "Click Level", 0.0f, 1.0f, 0.0f));

    // Phase-Locked Mono Sub Anchor
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("sub_level", 1), "Sub Level", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("sub_octave", 1), "Sub Octave", -2, -1, -1));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("sub_wave", 1), "Sub Wave", 0, 2, 0)); // 0: Sine, 1: Triangle, 2: Square

    // Pitch Drop & Legato Glide
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("pitch_drop_active", 1), "Pitch Drop Active", 0, 1, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("pitch_drop_octaves", 1), "Pitch Drop Octaves", 1.0f, 2.0f, 2.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("pitch_drop_time", 1), "Pitch Drop Time", 20.0f, 80.0f, 50.0f));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("play_mode", 1), "Play Mode", 0, 1, 0)); // 0: Poly, 1: Mono Legato
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("glide_time", 1), "Glide Time", 0.0f, 1000.0f, 100.0f));

    // Smart Voicing / Chord Engine
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("chord_mode", 1), "Chord Mode", 0, 1, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("chord_type", 1), "Chord Type", 0, 1, 0)); // 0: Major, 1: Minor

    // Vowel Formant Filter
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("formant_morph", 1), "Formant Morph", 0.0f, 4.0f, 0.0f));

    // 2D Trash vs Gloss X/Y Morphing
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("trash_gloss_x", 1), "Trash (X)", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("trash_gloss_y", 1), "Gloss (Y)", 0.0f, 1.0f, 0.0f));

    // Smart Mono-Maker (<120 Hz)
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("mono_maker_active", 1), "Mono Maker Active", 0, 1, 1));

    // Tempo-Synced Sidechain Pumper
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("pump_active", 1), "Pump Active", 0, 1, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("pump_division", 1), "Pump Division", 0, 2, 0)); // 0: 1/4, 1: 1/8, 2: 1/16
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("pump_depth", 1), "Pump Depth", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("pump_curve", 1), "Pump Curve", 0.1f, 5.0f, 2.0f));

    // Macro Controls
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("macro_drop", 1), "The Drop / Build", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("macro_punch", 1), "Macro Punch", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("macro_grit", 1), "Macro Grit", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("macro_space", 1), "Macro Space", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("macro_width", 1), "Macro Width", 0.0f, 1.0f, 0.0f));

    return { params.begin(), params.end() };
}
