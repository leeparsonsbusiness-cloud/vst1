#include "PresetManager.h"

PresetManager::PresetManager()
{
    presets = {
        // 1. Clarity Supersaw
        {
            "Clarity Supersaw",
            1, 0, 0.0f, 7, 20.0f, 0.85f,     // Osc1: Saw, Oct 0, Det 0, Unison 7 (20c), Lvl 0.85
            1, -1, 5.0f, 0.5f,              // Osc2: Saw, Oct -1, Det 5c, Lvl 0.5
            0.01f, 0.4f, 0.8f, 0.5f,        // Amp ADSR
            4500.0f, 0.2f, 0, 0.4f,         // Filter: 4500Hz, Res 0.2, LPF12, EnvAmt 0.4
            0.01f, 0.3f, 0.5f, 0.4f,        // Filter ADSR
            1.2f, 1.5f, 0.35f, 0.4f,        // FX: Drive 1.2, Chorus (Rate 1.5, Depth 0.35, Mix 0.4)
            350.0f, 0.4f, 0.35f             // Delay: 350ms, FB 0.4, Mix 0.35
        },
        // 2. Dirty Electro Saw Bass
        {
            "Dirty Electro Saw Bass",
            1, -1, 0.0f, 3, 12.0f, 0.9f,
            2, -2, 0.0f, 0.7f,
            0.005f, 0.2f, 0.9f, 0.2f,
            800.0f, 0.55f, 1, 0.65f,         // LPF24
            0.001f, 0.15f, 0.1f, 0.1f,
            3.5f, 0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f
        },
        // 3. Zedd Punch Pluck
        {
            "Zedd Punch Pluck",
            1, 0, 0.0f, 5, 15.0f, 0.85f,
            2, 0, 7.0f, 0.6f,
            0.001f, 0.25f, 0.0f, 0.15f,
            400.0f, 0.6f, 0, 0.85f,
            0.001f, 0.12f, 0.0f, 0.1f,
            1.8f, 2.0f, 0.25f, 0.2f,
            250.0f, 0.3f, 0.3f
        },
        // 4. Vocal Formant Lead
        {
            "Vocal Formant Lead",
            1, 0, 0.0f, 7, 18.0f, 0.8f,
            3, 1, 0.0f, 0.4f,
            0.05f, 0.3f, 0.85f, 0.4f,
            1200.0f, 0.8f, 4, 0.5f,          // BPF12
            0.02f, 0.4f, 0.3f, 0.3f,
            2.0f, 1.2f, 0.5f, 0.5f,
            300.0f, 0.35f, 0.25f
        },
        // 5. 8-Bit Glitch Lead
        {
            "8-Bit Glitch Lead",
            2, 1, 0.0f, 1, 0.0f, 0.9f,
            2, 0, 12.0f, 0.5f,
            0.001f, 0.1f, 0.6f, 0.05f,
            8000.0f, 0.1f, 0, 0.1f,
            0.001f, 0.1f, 0.5f, 0.05f,
            1.0f, 0.0f, 0.0f, 0.0f,
            150.0f, 0.2f, 0.2f
        },
        // 6. Tik Tok Party Bass
        {
            "Tik Tok Party Bass",
            2, -1, 0.0f, 5, 14.0f, 0.85f,
            1, -1, -5.0f, 0.6f,
            0.005f, 0.3f, 0.7f, 0.2f,
            1100.0f, 0.4f, 1, 0.5f,
            0.005f, 0.2f, 0.2f, 0.15f,
            4.0f, 1.0f, 0.3f, 0.3f,
            0.0f, 0.0f, 0.0f
        },
        // 7. Spectrum Anthem Lead
        {
            "Spectrum Anthem Lead",
            1, 1, 0.0f, 7, 25.0f, 0.9f,
            1, 0, 8.0f, 0.5f,
            0.02f, 0.5f, 0.9f, 0.6f,
            6000.0f, 0.3f, 0, 0.3f,
            0.01f, 0.4f, 0.6f, 0.4f,
            1.5f, 2.5f, 0.4f, 0.6f,
            375.0f, 0.45f, 0.4f
        },
        // 8. Stay The Night Pluck
        {
            "Stay The Night Pluck",
            3, 1, 0.0f, 3, 10.0f, 0.8f,
            0, 2, 5.0f, 0.4f,
            0.001f, 0.35f, 0.0f, 0.25f,
            2500.0f, 0.45f, 0, 0.6f,
            0.001f, 0.2f, 0.0f, 0.15f,
            1.1f, 1.8f, 0.3f, 0.3f,
            300.0f, 0.4f, 0.3f
        },
        // 9. Blow Electro Stab
        {
            "Blow Electro Stab",
            1, 0, 0.0f, 7, 22.0f, 0.85f,
            2, -1, 0.0f, 0.6f,
            0.001f, 0.2f, 0.1f, 0.1f,
            900.0f, 0.5f, 1, 0.8f,
            0.001f, 0.1f, 0.0f, 0.08f,
            5.0f, 1.0f, 0.2f, 0.2f,
            180.0f, 0.25f, 0.2f
        },
        // 10. Beautiful Now Chords
        {
            "Beautiful Now Chords",
            1, 0, 0.0f, 7, 16.0f, 0.8f,
            3, 0, 4.0f, 0.5f,
            0.2f, 0.8f, 0.8f, 1.2f,
            3200.0f, 0.15f, 0, 0.25f,
            0.3f, 0.6f, 0.7f, 1.0f,
            1.05f, 0.8f, 0.5f, 0.55f,
            450.0f, 0.35f, 0.3f
        },
        // 11. Die Young Synth Brass
        {
            "Die Young Synth Brass",
            1, 0, 0.0f, 5, 18.0f, 0.85f,
            1, -1, -8.0f, 0.6f,
            0.03f, 0.4f, 0.75f, 0.3f,
            1800.0f, 0.35f, 0, 0.55f,
            0.04f, 0.35f, 0.3f, 0.25f,
            2.2f, 1.5f, 0.25f, 0.25f,
            280.0f, 0.3f, 0.25f
        },
        // 12. Break Free Arp Synth
        {
            "Break Free Arp Synth",
            1, 1, 0.0f, 5, 12.0f, 0.8f,
            2, 1, 6.0f, 0.6f,
            0.002f, 0.18f, 0.2f, 0.1f,
            3500.0f, 0.5f, 0, 0.7f,
            0.001f, 0.15f, 0.1f, 0.1f,
            1.6f, 2.2f, 0.3f, 0.3f,
            200.0f, 0.35f, 0.3f
        },
        // 13. Starving Hyperpop Lead
        {
            "Starving Hyperpop Lead",
            2, 1, 5.0f, 7, 28.0f, 0.85f,
            3, 2, -5.0f, 0.5f,
            0.005f, 0.3f, 0.7f, 0.2f,
            7500.0f, 0.6f, 0, 0.4f,
            0.005f, 0.2f, 0.4f, 0.2f,
            3.2f, 3.0f, 0.4f, 0.5f,
            220.0f, 0.4f, 0.35f
        },
        // 14. I Want You To Know Bass
        {
            "I Want You To Know Bass",
            1, -1, 0.0f, 7, 15.0f, 0.9f,
            0, -2, 0.0f, 0.7f,
            0.01f, 0.35f, 0.8f, 0.3f,
            650.0f, 0.4f, 1, 0.6f,
            0.005f, 0.2f, 0.15f, 0.15f,
            2.8f, 0.5f, 0.1f, 0.1f,
            0.0f, 0.0f, 0.0f
        },
        // 15. Take It Off Sub Bass
        {
            "Take It Off Sub Bass",
            0, -1, 0.0f, 1, 0.0f, 1.0f,
            2, -2, 0.0f, 0.4f,
            0.01f, 0.4f, 0.9f, 0.4f,
            400.0f, 0.1f, 0, 0.2f,
            0.01f, 0.3f, 0.2f, 0.2f,
            1.3f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f
        },
        // 16. Clarity FX Riser
        {
            "Clarity FX Riser",
            4, 0, 0.0f, 1, 0.0f, 0.6f,        // Noise
            1, 1, 0.0f, 0.6f,
            0.5f, 1.0f, 1.0f, 1.5f,
            500.0f, 0.75f, 0, 0.9f,
            2.0f, 0.5f, 0.9f, 1.0f,
            2.0f, 1.0f, 0.5f, 0.4f,
            500.0f, 0.5f, 0.4f
        },
        // 17. Crazy Kids FM Metal
        {
            "Crazy Kids FM Metal",
            2, 0, 0.0f, 3, 22.0f, 0.85f,
            2, 1, 35.0f, 0.65f,
            0.005f, 0.25f, 0.6f, 0.2f,
            4200.0f, 0.7f, 0, 0.5f,
            0.005f, 0.2f, 0.2f, 0.15f,
            4.2f, 2.0f, 0.4f, 0.35f,
            240.0f, 0.3f, 0.25f
        },
        // 18. Adrenaline Rush Saw
        {
            "Adrenaline Rush Saw",
            1, 0, 0.0f, 7, 30.0f, 0.9f,
            1, 1, 10.0f, 0.5f,
            0.005f, 0.4f, 0.85f, 0.35f,
            5500.0f, 0.25f, 0, 0.45f,
            0.005f, 0.3f, 0.4f, 0.25f,
            1.9f, 2.8f, 0.45f, 0.45f,
            320.0f, 0.4f, 0.35f
        },
        // 19. Symphony Glass Keys
        {
            "Symphony Glass Keys",
            3, 0, 0.0f, 3, 8.0f, 0.8f,
            0, 1, 3.0f, 0.5f,
            0.002f, 0.6f, 0.1f, 0.5f,
            3800.0f, 0.2f, 0, 0.4f,
            0.001f, 0.3f, 0.0f, 0.3f,
            1.0f, 1.5f, 0.3f, 0.3f,
            380.0f, 0.45f, 0.35f
        },
        // 20. Cannibal Acid Bass
        {
            "Cannibal Acid Bass",
            1, -1, 0.0f, 1, 0.0f, 0.9f,
            2, -1, 5.0f, 0.5f,
            0.001f, 0.25f, 0.4f, 0.15f,
            600.0f, 0.9f, 0, 0.95f,
            0.001f, 0.15f, 0.0f, 0.1f,
            4.5f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f
        }
    };

    for (const auto& p : presets)
        presetNames.push_back(p.name);
}

int PresetManager::getNumPresets() const
{
    return static_cast<int>(presets.size());
}

juce::String PresetManager::getPresetName(int index) const
{
    if (index >= 0 && index < static_cast<int>(presetNames.size()))
        return presetNames[static_cast<size_t>(index)];
    return "Custom";
}

const std::vector<std::string>& PresetManager::getPresetNames() const
{
    return presetNames;
}

static void setParamValue(juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID, float plainValue)
{
    if (auto* param = apvts.getParameter(paramID))
    {
        float normalized = param->convertTo0to1(plainValue);
        param->setValueNotifyingHost(normalized);
    }
}

void PresetManager::applyPreset(int index, juce::AudioProcessorValueTreeState& apvts)
{
    if (index < 0 || index >= static_cast<int>(presets.size()))
        return;

    const auto& p = presets[static_cast<size_t>(index)];

    setParamValue(apvts, "preset", static_cast<float>(index));

    setParamValue(apvts, "osc1_wave", static_cast<float>(p.osc1Wave));
    setParamValue(apvts, "osc1_octave", static_cast<float>(p.osc1Octave));
    setParamValue(apvts, "osc1_detune", p.osc1Detune);
    setParamValue(apvts, "unison_count", static_cast<float>(p.unisonCount));
    setParamValue(apvts, "unison_detune", p.unisonDetune);
    setParamValue(apvts, "osc1_level", p.osc1Level);

    setParamValue(apvts, "osc2_wave", static_cast<float>(p.osc2Wave));
    setParamValue(apvts, "osc2_octave", static_cast<float>(p.osc2Octave));
    setParamValue(apvts, "osc2_detune", p.osc2Detune);
    setParamValue(apvts, "osc2_level", p.osc2Level);

    setParamValue(apvts, "amp_attack", p.ampAttack);
    setParamValue(apvts, "amp_decay", p.ampDecay);
    setParamValue(apvts, "amp_sustain", p.ampSustain);
    setParamValue(apvts, "amp_release", p.ampRelease);

    setParamValue(apvts, "filter_cutoff", p.filterCutoff);
    setParamValue(apvts, "filter_res", p.filterRes);
    setParamValue(apvts, "filter_mode", static_cast<float>(p.filterMode));
    setParamValue(apvts, "filter_env_amt", p.filterEnvAmt);
    setParamValue(apvts, "filter_attack", p.filterAttack);
    setParamValue(apvts, "filter_decay", p.filterDecay);
    setParamValue(apvts, "filter_sustain", p.filterSustain);
    setParamValue(apvts, "filter_release", p.filterRelease);

    setParamValue(apvts, "fx_drive", p.fxDrive);
    setParamValue(apvts, "fx_chorus_rate", p.fxChorusRate);
    setParamValue(apvts, "fx_chorus_depth", p.fxChorusDepth);
    setParamValue(apvts, "fx_chorus_mix", p.fxChorusMix);
    setParamValue(apvts, "fx_delay_time", p.fxDelayTime);
    setParamValue(apvts, "fx_delay_feedback", p.fxDelayFeedback);
    setParamValue(apvts, "fx_delay_mix", p.fxDelayMix);
}

void PresetManager::saveUserPreset(const juce::String& name)
{
    if (apvts == nullptr)
        return;

    juce::File appData = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                            .getChildFile("KeshaAndZeddSynth")
                            .getChildFile("UserPresets");
    if (!appData.exists())
        appData.createDirectory();

    juce::File presetFile = appData.getChildFile(name + ".json");
    
    juce::DynamicObject::Ptr jsonObj = new juce::DynamicObject();
    for (auto* param : apvts->processor.getParameters())
    {
        if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
        {
            jsonObj->setProperty(rangedParam->paramID, rangedParam->getValue());
        }
    }
    
    juce::FileOutputStream stream(presetFile);
    if (stream.openedOk())
    {
        stream.setPosition(0);
        stream.truncate();
        juce::var jsonVar(jsonObj.get());
        juce::JSON::writeToStream(stream, jsonVar);
    }
}

bool PresetManager::loadUserPreset(const juce::String& name)
{
    if (apvts == nullptr)
        return false;

    juce::File appData = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                            .getChildFile("KeshaAndZeddSynth")
                            .getChildFile("UserPresets");
    juce::File presetFile = appData.getChildFile(name + ".json");
    if (!presetFile.existsAsFile())
        return false;
        
    juce::var jsonVar = juce::JSON::parse(presetFile);
    if (auto* jsonObj = jsonVar.getDynamicObject())
    {
        for (auto* param : apvts->processor.getParameters())
        {
            if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
            {
                if (jsonObj->hasProperty(rangedParam->paramID))
                {
                    float val = static_cast<float>(jsonObj->getProperty(rangedParam->paramID));
                    rangedParam->setValueNotifyingHost(val);
                }
            }
        }
        return true;
    }
    return false;
}
