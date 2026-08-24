#include "PluginProcessor.h"
#include "PluginEditor.h"

// Modern Dark-Mode Custom LookAndFeel Implementation
ModernSynthLookAndFeel::ModernSynthLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff12131a));
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff1e212d));
    setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff00f2fe).withAlpha(0.5f));
    setColour(juce::ComboBox::focusedOutlineColourId, juce::Colour(0xff00f2fe));
    setColour(juce::ComboBox::textColourId, juce::Colours::white);
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff181a24));
    setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xff00f2fe).withAlpha(0.3f));
    setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    setColour(juce::PopupMenu::textColourId, juce::Colour(0xffc5cad6));
}

void ModernSynthLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                              juce::Slider& slider)
{
    auto radius = (float) juce::jmin(width / 2, height / 2) - 6.0f;
    auto centreX = (float) x + (float) width * 0.5f;
    auto centreY = (float) y + (float) height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Background track arc
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colour(0xff252936));
    g.strokePath(backgroundArc, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Active cyan/magenta gradient track arc
    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
        juce::ColourGradient grad(juce::Colour(0xff00f2fe), centreX - radius, centreY,
                                  juce::Colour(0xffff2a85), centreX + radius, centreY, false);
        g.setGradientFill(grad);
        g.strokePath(valueArc, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Inner dial metallic body
    g.setColour(juce::Colour(0xff1a1c26));
    g.fillEllipse(rx + 4.0f, ry + 4.0f, rw - 8.0f, rw - 8.0f);
    g.setColour(juce::Colour(0xff2a2e3d));
    g.drawEllipse(rx + 4.0f, ry + 4.0f, rw - 8.0f, rw - 8.0f, 1.5f);

    // Dial indicator line
    juce::Path p;
    auto pointerLength = radius * 0.55f;
    auto pointerThickness = 3.0f;
    p.addRectangle(-pointerThickness * 0.5f, -radius + 6.0f, pointerThickness, pointerLength);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    g.setColour(juce::Colour(0xff00f2fe));
    g.fillPath(p);
}

void ModernSynthLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPos, float minSliderPos, float maxSliderPos,
                                              const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    if (style == juce::Slider::LinearVertical)
    {
        auto trackWidth = 4.0f;
        auto trackX = (float) x + ((float) width - trackWidth) * 0.5f;

        // Background track
        g.setColour(juce::Colour(0xff252936));
        g.fillRoundedRectangle(trackX, (float) y, trackWidth, (float) height, 2.0f);

        // Filled track
        auto fillHeight = maxSliderPos - sliderPos;
        g.setColour(juce::Colour(0xff00f2fe));
        g.fillRoundedRectangle(trackX, sliderPos, trackWidth, fillHeight, 2.0f);

        // Glowing thumb cap
        auto thumbWidth = 14.0f;
        auto thumbHeight = 10.0f;
        auto thumbX = (float) x + ((float) width - thumbWidth) * 0.5f;
        g.setColour(juce::Colour(0xffff2a85));
        g.fillRoundedRectangle(thumbX, sliderPos - thumbHeight * 0.5f, thumbWidth, thumbHeight, 3.0f);
    }
    else
    {
        juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    }
}

void ModernSynthLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool /*isButtonDown*/,
                                          int buttonX, int buttonY, int buttonW, int buttonH,
                                          juce::ComboBox& box)
{
    g.setColour(juce::Colour(0xff1e212d));
    g.fillRoundedRectangle(0.0f, 0.0f, (float) width, (float) height, 6.0f);

    g.setColour(juce::Colour(0xff00f2fe).withAlpha(box.hasKeyboardFocus(true) ? 0.8f : 0.4f));
    g.drawRoundedRectangle(0.0f, 0.0f, (float) width, (float) height, 6.0f, 1.2f);

    // Arrow indicator
    juce::Path path;
    path.addTriangle((float) buttonX + (float) buttonW * 0.3f, (float) buttonY + (float) buttonH * 0.4f,
                     (float) buttonX + (float) buttonW * 0.7f, (float) buttonY + (float) buttonH * 0.4f,
                     (float) buttonX + (float) buttonW * 0.5f, (float) buttonY + (float) buttonH * 0.7f);
    g.setColour(juce::Colour(0xff00f2fe));
    g.fillPath(path);
}

// Plugin Editor Constructor
KeshaZeddSynthAudioProcessorEditor::KeshaZeddSynthAudioProcessorEditor(KeshaZeddSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), dragMidiButton(p), vuMeter(p)
{
    setLookAndFeel(&lookAndFeel);
    setSize(960, 860);

    // Header Controls
    setupComboBox(presetBox, presetLabel, "PRESET");
    auto& presetNames = audioProcessor.getPresetManager().getPresetNames();
    for (size_t i = 0; i < presetNames.size(); ++i)
        presetBox.addItem(juce::String(i + 1) + ". " + presetNames[i], static_cast<int>(i + 1));
        
    presetBox.onPresetSelected = [this](int idx) {
        audioProcessor.loadPreset(idx);
        presetBox.setSelectedItemIndex(idx, juce::dontSendNotification);
    };
    presetBox.onUserPresetSelected = [this](const juce::File& file) {
        audioProcessor.getPresetManager().loadUserPreset(file);
        presetBox.setText(file.getFileNameWithoutExtension(), juce::dontSendNotification);
    };
    presetAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "preset", presetBox);

    prevPresetButton.setButtonText("<");
    prevPresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff181a24));
    addAndMakeVisible(prevPresetButton);
    prevPresetButton.onClick = [this]() {
        int currentIdx = presetBox.getSelectedItemIndex();
        if (currentIdx == -1) currentIdx = 0;
        int numPresets = audioProcessor.getPresetManager().getNumPresets();
        int prevIdx = (currentIdx - 1 + numPresets) % numPresets;
        presetBox.onPresetSelected(prevIdx);
    };

    nextPresetButton.setButtonText(">");
    nextPresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff181a24));
    addAndMakeVisible(nextPresetButton);
    nextPresetButton.onClick = [this]() {
        int currentIdx = presetBox.getSelectedItemIndex();
        if (currentIdx == -1) currentIdx = 0;
        int numPresets = audioProcessor.getPresetManager().getNumPresets();
        int nextIdx = (currentIdx + 1) % numPresets;
        presetBox.onPresetSelected(nextIdx);
    };

    // Zeddify & Auto-Master Controls
    zeddifyButton.setButtonText("ZEDDIFY");
    zeddifyButton.setColour(juce::ToggleButton::textColourId, juce::Colour(0xff00f2fe));
    zeddifyButton.setColour(juce::ToggleButton::tickColourId, juce::Colour(0xff00f2fe));
    addAndMakeVisible(zeddifyButton);
    zeddifyAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "zeddify_active", zeddifyButton);

    addAndMakeVisible(dragMidiButton);

    autoMasterButton.setButtonText("AUTO-MASTER");
    autoMasterButton.setColour(juce::ToggleButton::textColourId, juce::Colour(0xffff2a85));
    autoMasterButton.setColour(juce::ToggleButton::tickColourId, juce::Colour(0xffff2a85));
    addAndMakeVisible(autoMasterButton);
    autoMasterAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "auto_master_active", autoMasterButton);

    setupSlider(autoMasterIntensitySlider, autoMasterIntensityLabel, "PWR");
    autoMasterIntensityAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "auto_master_intensity", autoMasterIntensitySlider);

    addAndMakeVisible(vuMeter);
    addAndMakeVisible(audioProcessor.getVisualizer());

    setupSlider(masterVolSlider, masterVolLabel, "MASTER");
    masterVolAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "master_vol", masterVolSlider);

    // Save, Load & Dice Buttons
    savePresetButton.setButtonText("SAVE");
    savePresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff181a24));
    savePresetButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    addAndMakeVisible(savePresetButton);
    savePresetButton.onClick = [this]() { showSavePresetDialog(); };

    loadPresetButton.setButtonText("LOAD");
    loadPresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff181a24));
    loadPresetButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    addAndMakeVisible(loadPresetButton);
    loadPresetButton.onClick = [this]() { presetBox.showPopup(); };

    diceButton.setButtonText("🎲 DICE");
    diceButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff181a24));
    diceButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff00f2fe));
    addAndMakeVisible(diceButton);
    diceButton.onClick = [this]() { audioProcessor.randomizeParameters(); };

    voiceCountLabel.setText("VOICES: 0 / 8", juce::dontSendNotification);
    voiceCountLabel.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    voiceCountLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00f2fe));
    voiceCountLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(voiceCountLabel);

    // ----------------------------------------------------
    // Section 1: Oscillators & Unison Controls
    // ----------------------------------------------------
    setupSlider(osc1ShapeSlider, osc1ShapeLabel, "OSC 1 SHAPE");
    osc1ShapeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "osc1_shape", osc1ShapeSlider);

    setupComboBox(osc1OctaveBox, osc1OctaveLabel, "OCTAVE");
    osc1OctaveBox.addItemList({"-2", "-1", "0", "+1", "+2"}, 1);
    osc1OctaveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "osc1_octave", osc1OctaveBox);

    setupSlider(osc1DetuneSlider, osc1DetuneLabel, "DETUNE");
    osc1DetuneAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "osc1_detune", osc1DetuneSlider);

    setupSlider(osc1LevelSlider, osc1LevelLabel, "LEVEL");
    osc1LevelAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "osc1_level", osc1LevelSlider);

    setupComboBox(unisonCountBox, unisonCountLabel, "UNISON");
    unisonCountBox.addItemList({"1 Voice", "3 Voices", "5 Voices", "7 Voices"}, 1);
    unisonCountAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "unison_count", unisonCountBox);

    setupSlider(unisonDetuneSlider, unisonDetuneLabel, "SPREAD");
    unisonDetuneAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "unison_detune", unisonDetuneSlider);

    setupSlider(osc2ShapeSlider, osc2ShapeLabel, "OSC 2 SHAPE");
    osc2ShapeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "osc2_shape", osc2ShapeSlider);

    setupComboBox(osc2OctaveBox, osc2OctaveLabel, "OCTAVE");
    osc2OctaveBox.addItemList({"-2", "-1", "0", "+1", "+2"}, 1);
    osc2OctaveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "osc2_octave", osc2OctaveBox);

    setupSlider(osc2DetuneSlider, osc2DetuneLabel, "DETUNE");
    osc2DetuneAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "osc2_detune", osc2DetuneSlider);

    setupSlider(osc2LevelSlider, osc2LevelLabel, "LEVEL");
    osc2LevelAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "osc2_level", osc2LevelSlider);

    setupSlider(oscFmSlider, oscFmLabel, "FM DEPTH");
    oscFmAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "osc_fm_depth", oscFmSlider);

    setupComboBox(oscSyncBox, oscSyncLabel, "HARD SYNC");
    oscSyncBox.addItemList({"Sync Off", "Sync On"}, 1);
    oscSyncAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "osc_sync", oscSyncBox);

    // ----------------------------------------------------
    // Section 2: Filter Controls
    // ----------------------------------------------------
    setupComboBox(filterModeBox, filterModeLabel, "FILTER MODE");
    filterModeBox.addItemList({"LPF 12", "LPF 24", "BPF 12", "HPF 12", "Notch", "Vowel Formant"}, 1);
    filterModeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "filter_mode", filterModeBox);

    setupSlider(filterCutoffSlider, filterCutoffLabel, "CUTOFF");
    filterCutoffAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_cutoff", filterCutoffSlider);

    setupSlider(filterResSlider, filterResLabel, "RESONANCE");
    filterResAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_res", filterResSlider);

    setupSlider(filterDriveSlider, filterDriveLabel, "DRIVE");
    filterDriveAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_drive", filterDriveSlider);

    setupSlider(filterEnvAmtSlider, filterEnvAmtLabel, "ENV AMT");
    filterEnvAmtAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_env_amt", filterEnvAmtSlider);

    setupSlider(filterKeyTrackSlider, filterKeyTrackLabel, "KEY TRACK");
    filterKeyTrackAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_key_track", filterKeyTrackSlider);

    setupSlider(filterLfoModSlider, filterLfoModLabel, "LFO MOD");
    filterLfoModAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_lfo_mod", filterLfoModSlider);

    setupSlider(formantMorphSlider, formantMorphLabel, "VOWEL");
    formantMorphAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "formant_morph", formantMorphSlider);

    // Filter ADSR & Curve
    setupSlider(filterAttackSlider, filterAttackLabel, "F-ATT", juce::Slider::LinearVertical);
    filterAttackAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_attack", filterAttackSlider);

    setupSlider(filterDecaySlider, filterDecayLabel, "F-DEC", juce::Slider::LinearVertical);
    filterDecayAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_decay", filterDecaySlider);

    setupSlider(filterSustainSlider, filterSustainLabel, "F-SUS", juce::Slider::LinearVertical);
    filterSustainAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_sustain", filterSustainSlider);

    setupSlider(filterReleaseSlider, filterReleaseLabel, "F-REL", juce::Slider::LinearVertical);
    filterReleaseAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_release", filterReleaseSlider);

    setupSlider(filterCurveSlider, filterCurveLabel, "F-CURVE");
    filterCurveAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_decay_curve", filterCurveSlider);

    // ----------------------------------------------------
    // Section 3: Amp ADSR & Curves
    // ----------------------------------------------------
    setupSlider(ampAttackSlider, ampAttackLabel, "ATTACK", juce::Slider::LinearVertical);
    ampAttackAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_attack", ampAttackSlider);

    setupSlider(ampDecaySlider, ampDecayLabel, "DECAY", juce::Slider::LinearVertical);
    ampDecayAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_decay", ampDecaySlider);

    setupSlider(ampSustainSlider, ampSustainLabel, "SUSTAIN", juce::Slider::LinearVertical);
    ampSustainAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_sustain", ampSustainSlider);

    setupSlider(ampReleaseSlider, ampReleaseLabel, "RELEASE", juce::Slider::LinearVertical);
    ampReleaseAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_release", ampReleaseSlider);

    setupSlider(ampCurveSlider, ampCurveLabel, "DECAY CURVE");
    ampCurveAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_decay_curve", ampCurveSlider);

    // Modulation Bay LFOs Setup
    setupComboBox(lfo1WaveBox, lfo1WaveLabel, "LFO 1 WAVE");
    lfo1WaveBox.addItemList({"Sine", "Triangle", "Saw", "Square", "Random"}, 1);
    lfo1WaveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "lfo1_wave", lfo1WaveBox);

    setupComboBox(lfo1RateBox, lfo1RateLabel, "LFO 1 RATE");
    lfo1RateBox.addItemList({"1/1", "1/2", "1/4", "1/8", "1/16", "1/32", "1/4T", "1/8T", "1/16T", "1/4D", "1/8D", "1/16D"}, 1);
    lfo1RateAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "lfo1_rate", lfo1RateBox);

    setupSlider(lfo1ToCutoffSlider, lfo1ToCutoffLabel, "LFO1->CUTOFF");
    lfo1ToCutoffAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "lfo1_to_cutoff", lfo1ToCutoffSlider);

    setupSlider(lfo1ToShapeSlider, lfo1ToShapeLabel, "LFO1->SHAPE");
    lfo1ToShapeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "lfo1_to_shape", lfo1ToShapeSlider);

    setupComboBox(lfo2WaveBox, lfo2WaveLabel, "LFO 2 WAVE");
    lfo2WaveBox.addItemList({"Sine", "Triangle", "Saw", "Square", "Random"}, 1);
    lfo2WaveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "lfo2_wave", lfo2WaveBox);

    setupComboBox(lfo2RateBox, lfo2RateLabel, "LFO 2 RATE");
    lfo2RateBox.addItemList({"1/1", "1/2", "1/4", "1/8", "1/16", "1/32", "1/4T", "1/8T", "1/16T", "1/4D", "1/8D", "1/16D"}, 1);
    lfo2RateAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "lfo2_rate", lfo2RateBox);

    setupSlider(lfo2ToPitchSlider, lfo2ToPitchLabel, "LFO2->PITCH");
    lfo2ToPitchAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "lfo2_to_pitch", lfo2ToPitchSlider);

    setupSlider(lfo2ToPanSlider, lfo2ToPanLabel, "LFO2->PAN");
    lfo2ToPanAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "lfo2_to_pan", lfo2ToPanSlider);

    // ----------------------------------------------------
    // Section 4: Post FX Controls
    // ----------------------------------------------------
    setupSlider(fxDriveSlider, fxDriveLabel, "DRIVE");
    fxDriveAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_drive", fxDriveSlider);

    setupSlider(fxChorusRateSlider, fxChorusRateLabel, "C-RATE");
    fxChorusRateAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_chorus_rate", fxChorusRateSlider);

    setupSlider(fxChorusDepthSlider, fxChorusDepthLabel, "C-DEPTH");
    fxChorusDepthAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_chorus_depth", fxChorusDepthSlider);

    setupSlider(fxChorusMixSlider, fxChorusMixLabel, "C-MIX");
    fxChorusMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_chorus_mix", fxChorusMixSlider);

    setupSlider(fxDelayTimeSlider, fxDelayTimeLabel, "D-TIME");
    fxDelayTimeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_delay_time", fxDelayTimeSlider);

    setupSlider(fxDelayFeedbackSlider, fxDelayFeedbackLabel, "D-FEEDBACK");
    fxDelayFeedbackAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_delay_feedback", fxDelayFeedbackSlider);

    setupSlider(fxDelayColorSlider, fxDelayColorLabel, "D-COLOR");
    fxDelayColorAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_delay_color", fxDelayColorSlider);

    setupSlider(fxDelayMixSlider, fxDelayMixLabel, "D-MIX");
    fxDelayMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_delay_mix", fxDelayMixSlider);

    setupSlider(fxReverbDecaySlider, fxReverbDecayLabel, "R-DECAY");
    fxReverbDecayAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_reverb_decay", fxReverbDecaySlider);

    setupSlider(fxReverbDampingSlider, fxReverbDampingLabel, "R-DAMP");
    fxReverbDampingAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_reverb_damping", fxReverbDampingSlider);

    setupSlider(fxReverbWidthSlider, fxReverbWidthLabel, "R-WIDTH");
    fxReverbWidthAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_reverb_width", fxReverbWidthSlider);

    setupSlider(fxReverbMixSlider, fxReverbMixLabel, "R-MIX");
    fxReverbMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_reverb_mix", fxReverbMixSlider);

    // ----------------------------------------------------
    // Section 5: XY Pad, Drop & Macros Setup
    // ----------------------------------------------------
    addAndMakeVisible(trashGlossPad);
    trashGlossLabel.setText("TRASH vs GLOSS", juce::dontSendNotification);
    trashGlossLabel.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    trashGlossLabel.setColour(juce::Label::textColourId, juce::Colour(0xffc5cad6));
    trashGlossLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(trashGlossLabel);
    
    trashGlossPad.onPositionChanged = [this](float x, float y) {
        if (auto* paramX = audioProcessor.getAPVTS().getParameter("trash_gloss_x"))
            paramX->setValueNotifyingHost(x);
        if (auto* paramY = audioProcessor.getAPVTS().getParameter("trash_gloss_y"))
            paramY->setValueNotifyingHost(y);
    };

    setupSlider(macroDropSlider, macroDropLabel, "THE DROP", juce::Slider::LinearVertical);
    macroDropAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_drop", macroDropSlider);

    setupSlider(punchSlider, punchLabel, "PUNCH");
    punchAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_punch", punchSlider);

    setupSlider(gritSlider, gritLabel, "GRIT");
    gritAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_grit", gritSlider);

    setupSlider(spaceSlider, spaceLabel, "SPACE");
    spaceAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_space", spaceSlider);

    setupSlider(widthSlider, widthLabel, "WIDTH");
    widthAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_width", widthSlider);

    // ----------------------------------------------------
    // Section 6: Sub Anchor, Click, Settings
    // ----------------------------------------------------
    setupComboBox(subWaveBox, subWaveLabel, "SUB WAVE");
    subWaveBox.addItemList({"Sine Sub", "Square Sub"}, 1);
    subWaveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "sub_wave", subWaveBox);

    setupComboBox(subOctaveBox, subOctaveLabel, "SUB OCT");
    subOctaveBox.addItemList({"-2 Octaves", "-1 Octave"}, 1);
    subOctaveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "sub_octave", subOctaveBox);

    setupSlider(subLevelSlider, subLevelLabel, "SUB LEVEL");
    subLevelAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "sub_level", subLevelSlider);

    setupSlider(subDriveSlider, subDriveLabel, "SUB DRIVE");
    subDriveAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "sub_drive", subDriveSlider);

    setupComboBox(playModeBox, playModeLabel, "VOICING");
    playModeBox.addItemList({"Poly (8 Voice)", "Mono Legato", "Mono Retrig"}, 1);
    playModeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "play_mode", playModeBox);

    setupComboBox(glideModeBox, glideModeLabel, "GLIDE MODE");
    glideModeBox.addItemList({"Auto Legato", "Always Porta", "Glide Off"}, 1);
    glideModeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "glide_mode", glideModeBox);

    setupComboBox(chordModeBox, chordModeLabel, "SMART CHORD");
    chordModeBox.addItemList({"Bypass Chord", "Chord Enabled"}, 1);
    chordModeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "chord_mode", chordModeBox);

    setupComboBox(chordTypeBox, chordTypeLabel, "CHORD TYPE");
    chordTypeBox.addItemList({"Major triad", "Minor triad"}, 1);
    chordTypeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "chord_type", chordTypeBox);

    setupComboBox(pitchDropActiveBox, pitchDropActiveLabel, "PITCH DROP");
    pitchDropActiveBox.addItemList({"Bypass Drop", "Enable Drop"}, 1);
    pitchDropActiveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "pitch_drop_active", pitchDropActiveBox);

    setupSlider(pitchDropOctavesSlider, pitchDropOctavesLabel, "DROP OCT");
    pitchDropOctavesAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "pitch_drop_octaves", pitchDropOctavesSlider);

    setupSlider(pitchDropTimeSlider, pitchDropTimeLabel, "DROP TIME");
    pitchDropTimeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "pitch_drop_time", pitchDropTimeSlider);

    setupSlider(glideTimeSlider, glideTimeLabel, "GLIDE TIME");
    glideTimeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "glide_time", glideTimeSlider);

    // Pumper
    setupComboBox(pumpActiveBox, pumpActiveLabel, "PUMPER");
    pumpActiveBox.addItemList({"Bypass Duck", "Ducking On"}, 1);
    pumpActiveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "pump_active", pumpActiveBox);

    setupComboBox(pumpDivisionBox, pumpDivisionLabel, "PUMP RATE");
    pumpDivisionBox.addItemList({"1/4 Pump", "1/8 Pump", "1/16 Pump"}, 1);
    pumpDivisionAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "pump_division", pumpDivisionBox);

    setupSlider(pumpDepthSlider, pumpDepthLabel, "PUMP DEPTH");
    pumpDepthAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "pump_depth", pumpDepthSlider);

    setupSlider(pumpCurveSlider, pumpCurveLabel, "PUMP SHAPE");
    pumpCurveAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "pump_curve", pumpCurveSlider);

    // Mono Maker
    setupComboBox(monoMakerActiveBox, monoMakerActiveLabel, "MONO MAKER");
    monoMakerActiveBox.addItemList({"Maker Off", "Maker Active"}, 1);
    monoMakerActiveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "mono_maker_active", monoMakerActiveBox);

    setupSlider(monoMakerFreqSlider, monoMakerFreqLabel, "MONO FREQ");
    monoMakerFreqAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "mono_maker_frequency", monoMakerFreqSlider);

    // Transient Shaper
    setupComboBox(transientTypeBox, transientTypeLabel, "ATTACK TYPE");
    transientTypeBox.addItemList({"Punch Click", "Piano Hammer", "Vocal Snap", "808 Tock", "Analog Pop"}, 1);
    transientTypeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "transient_type", transientTypeBox);

    setupSlider(clickLevelSlider, clickLevelLabel, "ATTACK LVL");
    clickLevelAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "transient_level", clickLevelSlider);

    setupSlider(transientDecaySlider, transientDecayLabel, "ATTACK DEC");
    transientDecayAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "transient_decay", transientDecaySlider);

    startTimerHz(30);
}

KeshaZeddSynthAudioProcessorEditor::~KeshaZeddSynthAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void KeshaZeddSynthAudioProcessorEditor::timerCallback()
{
    int activeCount = audioProcessor.getActiveVoiceCount();
    voiceCountLabel.setText("VOICES: " + juce::String(activeCount) + " / 8", juce::dontSendNotification);

    // Sync XY Pad position when not dragging
    float x = audioProcessor.getAPVTS().getRawParameterValue("trash_gloss_x")->load();
    float y = audioProcessor.getAPVTS().getRawParameterValue("trash_gloss_y")->load();
    if (!juce::ModifierKeys::getCurrentModifiers().isAnyMouseButtonDown())
    {
        trashGlossPad.setPosition(x, y);
    }

    // Sync active preset selection in combobox asynchronously
    int activePreset = static_cast<int>(audioProcessor.getAPVTS().getRawParameterValue("preset")->load());
    if (activePreset != presetBox.getSelectedItemIndex() && activePreset >= 0 && activePreset < 40)
    {
        presetBox.setSelectedItemIndex(activePreset, juce::dontSendNotification);
    }
}

void KeshaZeddSynthAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text, juce::Slider::SliderStyle style)
{
    slider.setSliderStyle(style);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    label.setColour(juce::Label::textColourId, juce::Colour(0xffc5cad6));
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}

void KeshaZeddSynthAudioProcessorEditor::setupComboBox(juce::ComboBox& box, juce::Label& label, const juce::String& text)
{
    addAndMakeVisible(box);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    label.setColour(juce::Label::textColourId, juce::Colour(0xffc5cad6));
    label.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(label);
}

void KeshaZeddSynthAudioProcessorEditor::showSavePresetDialog()
{
    auto* w = new juce::AlertWindow ("Save User Preset", "Select a category and enter a name for your custom preset:", juce::AlertWindow::QuestionIcon);
    w->addComboBox ("presetCategory", { "01_Basses", "02_Leads", "03_Plucks", "04_Keys & Chords", "05_Pads & Textures", "06_Transitions & FX" }, "Select Category");
    w->addTextEditor ("presetName", "My Preset", "");
    w->addButton ("Save", 1, juce::KeyPress (juce::KeyPress::returnKey, 0, 0));
    w->addButton ("Cancel", 0, juce::KeyPress (juce::KeyPress::escapeKey, 0, 0));
    w->setLookAndFeel(&getLookAndFeel());
    
    w->enterModalState (true, juce::ModalCallbackFunction::create([this, w](int result) {
        if (result == 1) // Save clicked
        {
            juce::String category = w->getComboBoxComponent ("presetCategory")->getText();
            juce::String name = w->getTextEditorContents ("presetName");
            if (category == "Select Category")
                category = "01_Basses";
            if (name.isNotEmpty())
            {
                audioProcessor.getPresetManager().saveUserPreset(category, name);
                presetBox.setText(category + " > " + name, juce::dontSendNotification);
            }
        }
        delete w;
    }));
}

void KeshaZeddSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff12131a));

    // Top Header Bar Background
    g.setColour(juce::Colour(0xff1c1e29));
    g.fillRect(0, 0, getWidth(), 55);
    g.setColour(juce::Colour(0xff232533));
    g.drawHorizontalLine(55, 0.0f, (float) getWidth());

    // Title text
    g.setFont(juce::FontOptions(22.0f, juce::Font::bold));
    juce::ColourGradient titleGrad(juce::Colour(0xff00f2fe), 20.0f, 25.0f,
                                   juce::Colour(0xffff2a85), 180.0f, 25.0f, false);
    g.setGradientFill(titleGrad);
    g.drawText("KZ-SYNTH 1", 20, 15, 160, 30, juce::Justification::centredLeft);

    // Section outline panels helper lambda
    auto drawSection = [&](int sx, int sy, int sw, int sh, const juce::String& title) {
        g.setColour(juce::Colour(0xff1c1e29));
        g.fillRoundedRectangle((float) sx, (float) sy, (float) sw, (float) sh, 8.0f);
        
        g.setColour(juce::Colour(0xff2a2e40));
        g.drawRoundedRectangle((float) sx, (float) sy, (float) sw, (float) sh, 8.0f, 1.5f);
        
        g.setFont(juce::FontOptions(11.0f, juce::Font::bold));
        g.setColour(juce::Colour(0xff00f2fe));
        g.drawText(title.toUpperCase(), sx + 15, sy + 10, sw - 30, 20, juce::Justification::centredLeft);
    };

    // Draw the 6 Main visual panels
    drawSection(15, 65, 450, 260, "Oscillators (Morph, FM & Unison detune)");
    drawSection(480, 65, 465, 260, "Dynamic Filter & Custom Envelope Shaper");
    drawSection(15, 340, 450, 260, "Amp ADSR & Dual Synced LFO Modulators");
    drawSection(480, 340, 465, 260, "Master Post FX Chain (Drive, Chorus, Delay & Reverb)");
    drawSection(15, 615, 450, 230, "XY Morph Pad & Drop Macro Section");
    drawSection(480, 615, 465, 230, "Sub Anchor, Click Transient & Smart Settings");
}

void KeshaZeddSynthAudioProcessorEditor::resized()
{
    // Header Bar Layout
    prevPresetButton.setBounds(180, 22, 22, 26);
    presetBox.setBounds(205, 22, 175, 26);
    nextPresetButton.setBounds(383, 22, 22, 26);

    savePresetButton.setBounds(410, 22, 45, 26);
    loadPresetButton.setBounds(458, 22, 45, 26);
    diceButton.setBounds(506, 22, 55, 26);

    zeddifyButton.setBounds(566, 22, 75, 26);
    dragMidiButton.setBounds(646, 22, 85, 26);

    autoMasterButton.setBounds(736, 22, 90, 26);

    autoMasterIntensityLabel.setBounds(830, 5, 35, 15);
    autoMasterIntensitySlider.setBounds(830, 18, 35, 34);

    masterVolLabel.setBounds(870, 5, 45, 15);
    masterVolSlider.setBounds(870, 18, 45, 34);

    vuMeter.setBounds(920, 18, 10, 32);

    voiceCountLabel.setBounds(932, 18, 20, 26);

    audioProcessor.getVisualizer().setBounds(15, 55, 930, 80);

    // Section 1: Oscillators & Unison (x: 15, y: 65, w: 450, h: 260)
    osc1ShapeLabel.setBounds(30, 95, 80, 15);
    osc1ShapeSlider.setBounds(30, 110, 80, 50);

    osc1OctaveLabel.setBounds(120, 95, 50, 15);
    osc1OctaveBox.setBounds(120, 112, 50, 24);

    osc1DetuneLabel.setBounds(180, 95, 55, 15);
    osc1DetuneSlider.setBounds(180, 110, 55, 50);

    osc1LevelLabel.setBounds(245, 95, 55, 15);
    osc1LevelSlider.setBounds(245, 110, 55, 50);

    unisonCountLabel.setBounds(310, 95, 60, 15);
    unisonCountBox.setBounds(310, 112, 60, 24);

    unisonDetuneLabel.setBounds(380, 95, 60, 15);
    unisonDetuneSlider.setBounds(380, 110, 60, 50);

    osc2ShapeLabel.setBounds(30, 175, 80, 15);
    osc2ShapeSlider.setBounds(30, 190, 80, 50);

    osc2OctaveLabel.setBounds(120, 175, 50, 15);
    osc2OctaveBox.setBounds(120, 192, 50, 24);

    osc2DetuneLabel.setBounds(180, 175, 55, 15);
    osc2DetuneSlider.setBounds(180, 190, 55, 50);

    osc2LevelLabel.setBounds(245, 175, 55, 15);
    osc2LevelSlider.setBounds(245, 190, 55, 50);

    oscFmLabel.setBounds(310, 175, 60, 15);
    oscFmSlider.setBounds(310, 190, 60, 50);

    oscSyncLabel.setBounds(380, 175, 60, 15);
    oscSyncBox.setBounds(380, 192, 60, 24);

    // Section 2: Filter & Filter Env (x: 480, y: 65, w: 465, h: 260)
    filterModeLabel.setBounds(495, 95, 90, 15);
    filterModeBox.setBounds(495, 112, 100, 24);

    filterCutoffLabel.setBounds(610, 95, 50, 15);
    filterCutoffSlider.setBounds(610, 110, 50, 50);

    filterResLabel.setBounds(670, 95, 50, 15);
    filterResSlider.setBounds(670, 110, 50, 50);

    filterDriveLabel.setBounds(730, 95, 50, 15);
    filterDriveSlider.setBounds(730, 110, 50, 50);

    filterEnvAmtLabel.setBounds(790, 95, 50, 15);
    filterEnvAmtSlider.setBounds(790, 110, 50, 50);

    filterKeyTrackLabel.setBounds(850, 95, 50, 15);
    filterKeyTrackSlider.setBounds(850, 110, 50, 50);

    filterLfoModLabel.setBounds(495, 145, 60, 15);
    filterLfoModSlider.setBounds(495, 160, 60, 45);

    formantMorphLabel.setBounds(565, 145, 60, 15);
    formantMorphSlider.setBounds(565, 160, 60, 45);

    filterCurveLabel.setBounds(635, 145, 60, 15);
    filterCurveSlider.setBounds(635, 160, 60, 45);

    int filterAdsrY = 210;
    int filterAdsrH = 100;
    filterAttackLabel.setBounds(710, filterAdsrY - 15, 50, 15);
    filterAttackSlider.setBounds(710, filterAdsrY, 50, filterAdsrH);

    filterDecayLabel.setBounds(765, filterAdsrY - 15, 50, 15);
    filterDecaySlider.setBounds(765, filterAdsrY, 50, filterAdsrH);

    filterSustainLabel.setBounds(820, filterAdsrY - 15, 50, 15);
    filterSustainSlider.setBounds(820, filterAdsrY, 50, filterAdsrH);

    filterReleaseLabel.setBounds(875, filterAdsrY - 15, 50, 15);
    filterReleaseSlider.setBounds(875, filterAdsrY, 50, filterAdsrH);

    // Section 3: Amp ADSR & Modulation (x: 15, y: 340, w: 450, h: 260)
    int ampAdsrY = 380;
    int ampAdsrHeight = 110;
    ampAttackLabel.setBounds(30, ampAdsrY - 15, 50, 15);
    ampAttackSlider.setBounds(30, ampAdsrY, 50, ampAdsrHeight);

    ampDecayLabel.setBounds(85, ampAdsrY - 15, 50, 15);
    ampDecaySlider.setBounds(85, ampAdsrY, 50, ampAdsrHeight);

    ampSustainLabel.setBounds(140, ampAdsrY - 15, 50, 15);
    ampSustainSlider.setBounds(140, ampAdsrY, 50, ampAdsrHeight);

    ampReleaseLabel.setBounds(195, ampAdsrY - 15, 50, 15);
    ampReleaseSlider.setBounds(195, ampAdsrY, 50, ampAdsrHeight);

    ampCurveLabel.setBounds(255, ampAdsrY - 15, 60, 15);
    ampCurveSlider.setBounds(255, ampAdsrY, 60, 50);

    // LFO controls in Section 3 bottom/right
    int lfoY = 510;
    lfo1WaveLabel.setBounds(30, lfoY - 15, 80, 15);
    lfo1WaveBox.setBounds(30, lfoY, 80, 24);

    lfo1RateLabel.setBounds(120, lfoY - 15, 60, 15);
    lfo1RateBox.setBounds(120, lfoY, 60, 24);

    lfo1ToCutoffLabel.setBounds(190, lfoY - 15, 75, 15);
    lfo1ToCutoffSlider.setBounds(190, lfoY, 75, 45);

    lfo1ToShapeLabel.setBounds(275, lfoY - 15, 70, 15);
    lfo1ToShapeSlider.setBounds(275, lfoY, 70, 45);

    int lfo2Y = 555;
    lfo2WaveLabel.setBounds(30, lfo2Y - 15, 80, 15);
    lfo2WaveBox.setBounds(30, lfo2Y, 80, 24);

    lfo2RateLabel.setBounds(120, lfo2Y - 15, 60, 15);
    lfo2RateBox.setBounds(120, lfo2Y, 60, 24);

    lfo2ToPitchLabel.setBounds(190, lfo2Y - 15, 75, 15);
    lfo2ToPitchSlider.setBounds(190, lfo2Y, 75, 45);

    lfo2ToPanLabel.setBounds(275, lfo2Y - 15, 70, 15);
    lfo2ToPanSlider.setBounds(275, lfo2Y, 70, 45);

    // Section 4: Post FX (x: 480, y: 340, w: 465, h: 260)
    fxDriveLabel.setBounds(495, 370, 60, 15);
    fxDriveSlider.setBounds(495, 385, 60, 50);

    fxChorusRateLabel.setBounds(565, 370, 60, 15);
    fxChorusRateSlider.setBounds(565, 385, 60, 50);

    fxChorusDepthLabel.setBounds(635, 370, 60, 15);
    fxChorusDepthSlider.setBounds(635, 385, 60, 50);

    fxChorusMixLabel.setBounds(705, 370, 60, 15);
    fxChorusMixSlider.setBounds(705, 385, 60, 50);

    // Delay
    fxDelayTimeLabel.setBounds(495, 450, 60, 15);
    fxDelayTimeSlider.setBounds(495, 465, 60, 50);

    fxDelayFeedbackLabel.setBounds(565, 450, 65, 15);
    fxDelayFeedbackSlider.setBounds(565, 465, 60, 50);

    fxDelayColorLabel.setBounds(635, 450, 60, 15);
    fxDelayColorSlider.setBounds(635, 465, 60, 50);

    fxDelayMixLabel.setBounds(705, 450, 60, 15);
    fxDelayMixSlider.setBounds(705, 465, 60, 50);

    // Reverb
    fxReverbDecayLabel.setBounds(775, 370, 60, 15);
    fxReverbDecaySlider.setBounds(775, 385, 60, 50);

    fxReverbDampingLabel.setBounds(845, 370, 60, 15);
    fxReverbDampingSlider.setBounds(845, 385, 60, 50);

    fxReverbWidthLabel.setBounds(775, 450, 60, 15);
    fxReverbWidthSlider.setBounds(775, 465, 60, 50);

    fxReverbMixLabel.setBounds(845, 450, 60, 15);
    fxReverbMixSlider.setBounds(845, 465, 60, 50);

    // Section 5: XY Pad, Drop & Macros (x: 15, y: 615, w: 450, h: 230)
    trashGlossLabel.setBounds(30, 645, 150, 15);
    trashGlossPad.setBounds(30, 662, 150, 150);

    macroDropLabel.setBounds(200, 645, 60, 15);
    macroDropSlider.setBounds(200, 662, 60, 150);

    // 2x2 Macro layout
    punchLabel.setBounds(280, 645, 60, 15);
    punchSlider.setBounds(280, 660, 60, 60);

    gritLabel.setBounds(360, 645, 60, 15);
    gritSlider.setBounds(360, 660, 60, 60);

    spaceLabel.setBounds(280, 730, 60, 15);
    spaceSlider.setBounds(280, 745, 60, 60);

    widthLabel.setBounds(360, 730, 60, 15);
    widthSlider.setBounds(360, 745, 60, 60);

    // Section 6: Sub Anchor, Formant & Settings (x: 480, y: 615, w: 465, h: 230)
    subWaveLabel.setBounds(495, 645, 65, 15);
    subWaveBox.setBounds(495, 660, 65, 24);

    subOctaveLabel.setBounds(565, 645, 55, 15);
    subOctaveBox.setBounds(565, 660, 55, 24);

    subLevelLabel.setBounds(630, 645, 60, 15);
    subLevelSlider.setBounds(630, 660, 60, 45);

    subDriveLabel.setBounds(695, 645, 60, 15);
    subDriveSlider.setBounds(695, 660, 60, 45);

    playModeLabel.setBounds(765, 645, 85, 15);
    playModeBox.setBounds(765, 660, 85, 24);

    glideModeLabel.setBounds(855, 645, 85, 15);
    glideModeBox.setBounds(855, 660, 85, 24);

    chordModeLabel.setBounds(765, 690, 85, 15);
    chordModeBox.setBounds(765, 705, 85, 24);

    chordTypeLabel.setBounds(855, 690, 85, 15);
    chordTypeBox.setBounds(855, 705, 85, 24);

    pitchDropOctavesLabel.setBounds(495, 715, 65, 15);
    pitchDropOctavesSlider.setBounds(495, 730, 65, 45);

    pitchDropTimeLabel.setBounds(565, 715, 60, 15);
    pitchDropTimeSlider.setBounds(565, 730, 60, 45);

    glideTimeLabel.setBounds(630, 715, 60, 15);
    glideTimeSlider.setBounds(630, 730, 60, 45);

    // Pumper & Mono Maker in Section 6 bottom
    pumpActiveLabel.setBounds(700, 765, 65, 15);
    pumpActiveBox.setBounds(700, 780, 65, 24);

    pumpDivisionLabel.setBounds(770, 765, 70, 15);
    pumpDivisionBox.setBounds(770, 780, 70, 24);

    pumpDepthLabel.setBounds(495, 780, 65, 15);
    pumpDepthSlider.setBounds(495, 795, 65, 45);

    pumpCurveLabel.setBounds(565, 780, 60, 15);
    pumpCurveSlider.setBounds(565, 795, 60, 45);

    monoMakerActiveLabel.setBounds(845, 765, 90, 15);
    monoMakerActiveBox.setBounds(845, 780, 90, 24);

    monoMakerFreqLabel.setBounds(630, 780, 60, 15);
    monoMakerFreqSlider.setBounds(630, 795, 60, 45);

    // Transient Shaper placement
    transientTypeLabel.setBounds(700, 715, 90, 15);
    transientTypeBox.setBounds(700, 730, 90, 24);

    clickLevelLabel.setBounds(800, 715, 70, 15);
    clickLevelSlider.setBounds(800, 730, 65, 45);

    transientDecayLabel.setBounds(875, 715, 70, 15);
    transientDecaySlider.setBounds(875, 730, 65, 45);
}
