#include "PluginProcessor.h"
#include "PluginEditor.h"

// ==============================================================================
// Analog Lab Inspired LookAndFeel Implementation
// ==============================================================================
ModernSynthLookAndFeel::ModernSynthLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff121318));
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff1c1e26));
    setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff353948));
    setColour(juce::ComboBox::focusedOutlineColourId, juce::Colour(0xff00d4ff));
    setColour(juce::ComboBox::textColourId, juce::Colour(0xffe0e4f0));
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff171820));
    setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xff00d4ff).withAlpha(0.25f));
    setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    setColour(juce::PopupMenu::textColourId, juce::Colour(0xffc5cad8));
}

void ModernSynthLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                              juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f - 7.0f;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // 1. Soft Drop Shadow
    g.setColour(juce::Colour(0x66000000));
    g.fillEllipse(rx + 1.0f, ry + 3.0f, rw, rw);

    // 2. Outer Background Track Ring (Analog Lab Dark Inset)
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colour(0xff1f222c));
    g.strokePath(backgroundArc, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // 3. Illuminated LED Value Arc
    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
        
        // Choose glowing theme color (Cyan to Amber gradient)
        juce::ColourGradient grad(juce::Colour(0xff00d4ff), centreX - radius, centreY,
                                  juce::Colour(0xffffaa00), centreX + radius, centreY, false);
        g.setGradientFill(grad);
        g.strokePath(valueArc, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // 4. Outer Dial Body (Brushed Gunmetal Cap)
    auto dialRadius = radius - 5.0f;
    auto drx = centreX - dialRadius;
    auto dry = centreY - dialRadius;
    auto drw = dialRadius * 2.0f;

    // Bevel outer ring
    g.setColour(juce::Colour(0xff2d313f));
    g.fillEllipse(drx, dry, drw, drw);

    // Metallic Gradient Face
    juce::ColourGradient faceGrad(juce::Colour(0xff282b36), centreX - dialRadius, centreY - dialRadius,
                                  juce::Colour(0xff15161d), centreX + dialRadius, centreY + dialRadius, false);
    g.setGradientFill(faceGrad);
    g.fillEllipse(drx + 1.5f, dry + 1.5f, drw - 3.0f, drw - 3.0f);

    // Subtle inner bevel
    g.setColour(juce::Colour(0xff3f4455).withAlpha(0.6f));
    g.drawEllipse(drx + 1.5f, dry + 1.5f, drw - 3.0f, drw - 3.0f, 1.0f);

    // 5. Dial Pointer Line & Center Dot
    juce::Path p;
    auto pointerLength = dialRadius * 0.70f;
    auto pointerThickness = 2.5f;
    p.addRoundedRectangle(-pointerThickness * 0.5f, -dialRadius + 2.0f, pointerThickness, pointerLength, 1.0f);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    
    g.setColour(juce::Colour(0xffffffff));
    g.fillPath(p);

    // Center metallic cap dot
    g.setColour(juce::Colour(0xff121318));
    g.fillEllipse(centreX - 3.0f, centreY - 3.0f, 6.0f, 6.0f);
    g.setColour(juce::Colour(0xff00d4ff));
    g.fillEllipse(centreX - 1.5f, centreY - 1.5f, 3.0f, 3.0f);
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
        g.setColour(juce::Colour(0xff1e212b));
        g.fillRoundedRectangle(trackX, (float) y, trackWidth, (float) height, 2.0f);

        // Filled track
        auto fillHeight = maxSliderPos - sliderPos;
        g.setColour(juce::Colour(0xff00d4ff));
        g.fillRoundedRectangle(trackX, sliderPos, trackWidth, fillHeight, 2.0f);

        // Thumb cap
        auto thumbWidth = 16.0f;
        auto thumbHeight = 10.0f;
        auto thumbX = (float) x + ((float) width - thumbWidth) * 0.5f;
        
        g.setColour(juce::Colour(0xff2d3140));
        g.fillRoundedRectangle(thumbX, sliderPos - thumbHeight * 0.5f, thumbWidth, thumbHeight, 2.5f);
        g.setColour(juce::Colour(0xffffaa00));
        g.drawRoundedRectangle(thumbX, sliderPos - thumbHeight * 0.5f, thumbWidth, thumbHeight, 2.5f, 1.0f);
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
    auto bounds = juce::Rectangle<float>(0.0f, 0.0f, (float) width, (float) height);
    
    // Background pill
    g.setColour(juce::Colour(0xff1a1c24));
    g.fillRoundedRectangle(bounds, 4.0f);

    // Border
    g.setColour(box.hasKeyboardFocus(true) ? juce::Colour(0xff00d4ff) : juce::Colour(0xff2f3344));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    // Chevron Arrow indicator
    juce::Path path;
    float ax = (float) buttonX + (float) buttonW * 0.5f;
    float ay = (float) buttonY + (float) buttonH * 0.5f;
    path.startNewSubPath(ax - 3.5f, ay - 2.0f);
    path.lineTo(ax, ay + 2.5f);
    path.lineTo(ax + 3.5f, ay - 2.0f);

    g.setColour(juce::Colour(0xff8c92a6));
    g.strokePath(path, juce::PathStrokeType(1.5f));
}

void ModernSynthLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                              bool shouldDrawButtonAsHighlighted, bool /*shouldDrawButtonAsDown*/)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(2.0f);
    bool isOn = button.getToggleState();

    // Button body
    g.setColour(isOn ? juce::Colour(0xff222836) : (shouldDrawButtonAsHighlighted ? juce::Colour(0xff1e212b) : juce::Colour(0xff181a22)));
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(isOn ? juce::Colour(0xff00d4ff) : juce::Colour(0xff2d3142));
    g.drawRoundedRectangle(bounds, 4.0f, isOn ? 1.5f : 1.0f);

    // Glowing LED dot indicator
    float ledX = bounds.getX() + 10.0f;
    float ledY = bounds.getCentreY();
    
    if (isOn)
    {
        g.setColour(juce::Colour(0x5500d4ff));
        g.fillEllipse(ledX - 5.0f, ledY - 5.0f, 10.0f, 10.0f);
        g.setColour(juce::Colour(0xff00d4ff));
        g.fillEllipse(ledX - 3.0f, ledY - 3.0f, 6.0f, 6.0f);
    }
    else
    {
        g.setColour(juce::Colour(0xff33384a));
        g.fillEllipse(ledX - 3.0f, ledY - 3.0f, 6.0f, 6.0f);
    }

    // Button text
    g.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    g.setColour(isOn ? juce::Colour(0xffffffff) : juce::Colour(0xff8e94a8));
    g.drawText(button.getButtonText(), bounds.withTrimmedLeft(20.0f), juce::Justification::centredLeft, true);
}

void ModernSynthLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                                  const juce::Colour& backgroundColour,
                                                  bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
    
    juce::Colour base = backgroundColour;
    if (shouldDrawButtonAsDown)
        base = base.darker(0.2f);
    else if (shouldDrawButtonAsHighlighted)
        base = base.brighter(0.15f);

    g.setColour(base);
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(shouldDrawButtonAsHighlighted ? juce::Colour(0xff00d4ff) : juce::Colour(0xff323648));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
}

// ==============================================================================
// Plugin Editor Constructor & Setup
// ==============================================================================
KeshaZeddSynthAudioProcessorEditor::KeshaZeddSynthAudioProcessorEditor(KeshaZeddSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), dragMidiButton(p), vuMeter(p)
{
    setLookAndFeel(&lookAndFeel);
    setSize(1040, 920);

    // Header Preset Browser
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

    // Zeddify & Auto-Master Workflow Buttons
    zeddifyButton.setButtonText("ZEDDIFY");
    addAndMakeVisible(zeddifyButton);
    zeddifyAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "zeddify_active", zeddifyButton);

    addAndMakeVisible(dragMidiButton);

    autoMasterButton.setButtonText("AUTO-MASTER");
    addAndMakeVisible(autoMasterButton);
    autoMasterAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "auto_master_active", autoMasterButton);

    setupSlider(autoMasterIntensitySlider, autoMasterIntensityLabel, "INTENSITY");
    autoMasterIntensityAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "auto_master_intensity", autoMasterIntensitySlider);

    addAndMakeVisible(vuMeter);
    addAndMakeVisible(audioProcessor.getVisualizer());

    setupSlider(masterVolSlider, masterVolLabel, "VOLUME");
    masterVolAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "master_vol", masterVolSlider);

    // Save, Load, Dice
    savePresetButton.setButtonText("SAVE");
    savePresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1f222e));
    addAndMakeVisible(savePresetButton);
    savePresetButton.onClick = [this]() { showSavePresetDialog(); };

    loadPresetButton.setButtonText("LOAD");
    loadPresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1f222e));
    addAndMakeVisible(loadPresetButton);
    loadPresetButton.onClick = [this]() {
        auto chooser = std::make_shared<juce::FileChooser>("Load User Preset JSON...",
                                  juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("KeshaAndZeddSynth").getChildFile("UserPresets"),
                                  "*.json");
        chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this, chooser](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                {
                    audioProcessor.getPresetManager().loadUserPreset(file);
                    presetBox.setText(file.getFileNameWithoutExtension(), juce::dontSendNotification);
                }
            });
    };

    diceButton.setButtonText("RANDOM");
    diceButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2d2238));
    addAndMakeVisible(diceButton);
    diceButton.onClick = [this]() {
        audioProcessor.randomizeParameters();
        presetBox.setText("Custom Random", juce::dontSendNotification);
    };

    voiceCountLabel.setText("8 VOICES", juce::dontSendNotification);
    voiceCountLabel.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    voiceCountLabel.setColour(juce::Label::textColourId, juce::Colour(0xff757a8e));
    addAndMakeVisible(voiceCountLabel);

    // ----------------------------------------------------
    // Section 1: Oscillators & Unison
    // ----------------------------------------------------
    setupSlider(osc1ShapeSlider, osc1ShapeLabel, "OSC 1 SHAPE");
    osc1ShapeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "osc1_shape", osc1ShapeSlider);

    setupComboBox(osc1OctaveBox, osc1OctaveLabel, "OCT");
    osc1OctaveBox.addItemList({"-2", "-1", "0", "+1", "+2"}, 1);
    osc1OctaveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "osc1_octave", osc1OctaveBox);

    setupSlider(osc1DetuneSlider, osc1DetuneLabel, "DETUNE");
    osc1DetuneAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "osc1_detune", osc1DetuneSlider);

    setupSlider(osc1LevelSlider, osc1LevelLabel, "LEVEL");
    osc1LevelAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "osc1_level", osc1LevelSlider);

    setupComboBox(unisonCountBox, unisonCountLabel, "UNISON");
    unisonCountBox.addItemList({"1 Voice", "2 Voices", "3 Voices", "4 Voices", "5 Voices", "6 Voices", "7 Voices"}, 1);
    unisonCountAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "unison_count", unisonCountBox);

    setupSlider(unisonDetuneSlider, unisonDetuneLabel, "SPREAD");
    unisonDetuneAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "unison_detune", unisonDetuneSlider);

    // Section 2: Oscillator 2 & Mod
    setupSlider(osc2ShapeSlider, osc2ShapeLabel, "OSC 2 SHAPE");
    osc2ShapeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "osc2_shape", osc2ShapeSlider);

    setupComboBox(osc2OctaveBox, osc2OctaveLabel, "OCT");
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
    // Section 2: Dynamic Filter & Envelope
    // ----------------------------------------------------
    setupComboBox(filterModeBox, filterModeLabel, "FILTER TYPE");
    filterModeBox.addItemList({"LPF 12dB", "LPF 24dB", "BPF 12dB", "HPF 12dB", "Notch", "Vowel Formant"}, 1);
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

    // Filter Envelope ADSR
    setupSlider(filterAttackSlider, filterAttackLabel, "F-ATT");
    filterAttackAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_attack", filterAttackSlider);

    setupSlider(filterDecaySlider, filterDecayLabel, "F-DEC");
    filterDecayAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_decay", filterDecaySlider);

    setupSlider(filterSustainSlider, filterSustainLabel, "F-SUS");
    filterSustainAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_sustain", filterSustainSlider);

    setupSlider(filterReleaseSlider, filterReleaseLabel, "F-REL");
    filterReleaseAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_release", filterReleaseSlider);

    setupSlider(filterCurveSlider, filterCurveLabel, "F-CURVE");
    filterCurveAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_decay_curve", filterCurveSlider);

    // ----------------------------------------------------
    // Section 3: Amp Envelope & Dual Synced LFOs
    // ----------------------------------------------------
    setupSlider(ampAttackSlider, ampAttackLabel, "ATTACK");
    ampAttackAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_attack", ampAttackSlider);

    setupSlider(ampDecaySlider, ampDecayLabel, "DECAY");
    ampDecayAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_decay", ampDecaySlider);

    setupSlider(ampSustainSlider, ampSustainLabel, "SUSTAIN");
    ampSustainAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_sustain", ampSustainSlider);

    setupSlider(ampReleaseSlider, ampReleaseLabel, "RELEASE");
    ampReleaseAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_release", ampReleaseSlider);

    setupSlider(ampCurveSlider, ampCurveLabel, "A-CURVE");
    ampCurveAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_decay_curve", ampCurveSlider);

    // LFO 1
    setupComboBox(lfo1WaveBox, lfo1WaveLabel, "LFO 1 SHAPE");
    lfo1WaveBox.addItemList({"Sine", "Triangle", "Saw", "Square", "Random S&H"}, 1);
    lfo1WaveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "lfo1_wave", lfo1WaveBox);

    setupComboBox(lfo1RateBox, lfo1RateLabel, "LFO 1 RATE");
    lfo1RateBox.addItemList({"1/1", "1/2", "1/4", "1/8", "1/16", "1/32", "1/4T", "1/8T", "1/16T", "1/4D", "1/8D", "1/16D"}, 1);
    lfo1RateAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "lfo1_rate", lfo1RateBox);

    setupSlider(lfo1ToCutoffSlider, lfo1ToCutoffLabel, "LFO1 > CUT");
    lfo1ToCutoffAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "lfo1_to_cutoff", lfo1ToCutoffSlider);

    setupSlider(lfo1ToShapeSlider, lfo1ToShapeLabel, "LFO1 > SHP");
    lfo1ToShapeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "lfo1_to_shape", lfo1ToShapeSlider);

    // LFO 2
    setupComboBox(lfo2WaveBox, lfo2WaveLabel, "LFO 2 SHAPE");
    lfo2WaveBox.addItemList({"Sine", "Triangle", "Saw", "Square", "Random S&H"}, 1);
    lfo2WaveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "lfo2_wave", lfo2WaveBox);

    setupComboBox(lfo2RateBox, lfo2RateLabel, "LFO 2 RATE");
    lfo2RateBox.addItemList({"1/1", "1/2", "1/4", "1/8", "1/16", "1/32", "1/4T", "1/8T", "1/16T", "1/4D", "1/8D", "1/16D"}, 1);
    lfo2RateAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "lfo2_rate", lfo2RateBox);

    setupSlider(lfo2ToPitchSlider, lfo2ToPitchLabel, "LFO2 > PITCH");
    lfo2ToPitchAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "lfo2_to_pitch", lfo2ToPitchSlider);

    setupSlider(lfo2ToPanSlider, lfo2ToPanLabel, "LFO2 > PAN");
    lfo2ToPanAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "lfo2_to_pan", lfo2ToPanSlider);

    // ----------------------------------------------------
    // Section 4: Post FX Chain
    // ----------------------------------------------------
    setupSlider(fxDriveSlider, fxDriveLabel, "DRIVE");
    fxDriveAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_drive", fxDriveSlider);

    setupSlider(fxChorusRateSlider, fxChorusRateLabel, "CHORUS RATE");
    fxChorusRateAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_chorus_rate", fxChorusRateSlider);

    setupSlider(fxChorusDepthSlider, fxChorusDepthLabel, "CHORUS DEPTH");
    fxChorusDepthAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_chorus_depth", fxChorusDepthSlider);

    setupSlider(fxChorusMixSlider, fxChorusMixLabel, "CHORUS MIX");
    fxChorusMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_chorus_mix", fxChorusMixSlider);

    setupSlider(fxDelayTimeSlider, fxDelayTimeLabel, "DELAY TIME");
    fxDelayTimeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_delay_time", fxDelayTimeSlider);

    setupSlider(fxDelayFeedbackSlider, fxDelayFeedbackLabel, "DELAY FDBK");
    fxDelayFeedbackAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_delay_feedback", fxDelayFeedbackSlider);

    setupSlider(fxDelayColorSlider, fxDelayColorLabel, "DELAY COLOR");
    fxDelayColorAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_delay_color", fxDelayColorSlider);

    setupSlider(fxDelayMixSlider, fxDelayMixLabel, "DELAY MIX");
    fxDelayMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_delay_mix", fxDelayMixSlider);

    setupSlider(fxReverbDecaySlider, fxReverbDecayLabel, "REVERB SIZE");
    fxReverbDecayAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_reverb_decay", fxReverbDecaySlider);

    setupSlider(fxReverbDampingSlider, fxReverbDampingLabel, "DAMPING");
    fxReverbDampingAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_reverb_damping", fxReverbDampingSlider);

    setupSlider(fxReverbWidthSlider, fxReverbWidthLabel, "WIDTH");
    fxReverbWidthAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_reverb_width", fxReverbWidthSlider);

    setupSlider(fxReverbMixSlider, fxReverbMixLabel, "REVERB MIX");
    fxReverbMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_reverb_mix", fxReverbMixSlider);

    // ----------------------------------------------------
    // Section 5: Macro Controls & 2D Pad
    // ----------------------------------------------------
    addAndMakeVisible(trashGlossPad);
    trashGlossPad.onPositionChanged = [this](float x, float y) {
        if (auto* paramX = audioProcessor.getAPVTS().getParameter("trash_gloss_x"))
            paramX->setValueNotifyingHost(x);
        if (auto* paramY = audioProcessor.getAPVTS().getParameter("trash_gloss_y"))
            paramY->setValueNotifyingHost(y);
    };

    setupSlider(macroDropSlider, macroDropLabel, "THE DROP");
    macroDropAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_drop", macroDropSlider);

    setupSlider(punchSlider, punchLabel, "PUNCH");
    punchAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_punch", punchSlider);

    setupSlider(gritSlider, gritLabel, "GRIT");
    gritAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_grit", gritSlider);

    setupSlider(spaceSlider, spaceLabel, "SPACE");
    spaceAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_space", spaceSlider);

    setupSlider(widthSlider, widthLabel, "STEREO");
    widthAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_width", widthSlider);

    // ----------------------------------------------------
    // Section 6: Sub Anchor, Voicing, Portamento & Settings
    // ----------------------------------------------------
    setupComboBox(subWaveBox, subWaveLabel, "SUB WAVE");
    subWaveBox.addItemList({"Sine Sub", "Square Sub"}, 1);
    subWaveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "sub_wave", subWaveBox);

    setupComboBox(subOctaveBox, subOctaveLabel, "SUB OCT");
    subOctaveBox.addItemList({"-2 Oct", "-1 Oct"}, 1);
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
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 55, 14);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffc5cad8));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::FontOptions(9.5f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colour(0xff8c92a6));
    addAndMakeVisible(label);
}

void KeshaZeddSynthAudioProcessorEditor::setupComboBox(juce::ComboBox& box, juce::Label& label, const juce::String& text)
{
    addAndMakeVisible(box);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::FontOptions(9.5f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centredLeft);
    label.setColour(juce::Label::textColourId, juce::Colour(0xff8c92a6));
    addAndMakeVisible(label);
}

void KeshaZeddSynthAudioProcessorEditor::showSavePresetDialog()
{
    auto* dialog = new juce::AlertWindow("Save User Preset", "Enter category and preset name:", juce::AlertWindow::QuestionIcon);
    dialog->addTextEditor("category", "01_Custom", "Category:");
    dialog->addTextEditor("name", "My Electro Sound", "Preset Name:");
    dialog->addButton("Save", 1, juce::KeyPress(juce::KeyPress::returnKey));
    dialog->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

    dialog->enterModalState(true, juce::ModalCallbackFunction::create([this, dialog](int result)
    {
        if (result == 1)
        {
            juce::String category = dialog->getTextEditorContents("category").trim();
            juce::String name = dialog->getTextEditorContents("name").trim();
            if (name.isNotEmpty())
            {
                if (category.isEmpty()) category = "01_Custom";
                audioProcessor.getPresetManager().saveUserPreset(category, name);
                presetBox.setText(name, juce::dontSendNotification);
            }
        }
        delete dialog;
    }));
}

void KeshaZeddSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    int w = getWidth();
    int h = getHeight();

    // 1. Chassis Background (Analog Lab Graphite Matte)
    g.fillAll(juce::Colour(0xff121318));

    // 2. Vintage Wooden Side Cheeks (Left & Right)
    auto drawWoodPanel = [&](float x, float width) {
        juce::ColourGradient woodGrad(juce::Colour(0xff452314), x, 0.0f,
                                      juce::Colour(0xff221008), x + width, 0.0f, false);
        g.setGradientFill(woodGrad);
        g.fillRect(x, 0.0f, width, (float) h);

        // Woodgrain hairline streaks
        g.setColour(juce::Colour(0xff5c301c).withAlpha(0.4f));
        for (float ly = 10.0f; ly < h; ly += 24.0f)
            g.drawHorizontalLine(static_cast<int>(ly), x + 2.0f, x + width - 2.0f);

        // Subtle bevel shadow
        g.setColour(juce::Colour(0x55000000));
        g.drawVerticalLine(static_cast<int>(x + width - 1.0f), 0.0f, (float) h);
    };

    drawWoodPanel(0.0f, 14.0f);
    drawWoodPanel((float) (w - 14), 14.0f);

    // 3. Top Header Bar (Brushed Gunmetal)
    juce::ColourGradient headerGrad(juce::Colour(0xff20232d), 0.0f, 0.0f,
                                    juce::Colour(0xff14161d), 0.0f, 54.0f, false);
    g.setGradientFill(headerGrad);
    g.fillRect(14, 0, w - 28, 54);

    g.setColour(juce::Colour(0xff2d3140));
    g.drawHorizontalLine(54, 14.0f, (float) (w - 14));

    // Title Logo in Analog Lab Style
    g.setFont(juce::FontOptions(19.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffffffff));
    g.drawText("KZ-SYNTH", 28, 10, 110, 22, juce::Justification::centredLeft);

    g.setFont(juce::FontOptions(8.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xffffaa00));
    g.drawText("ANALOG LAB EDITION", 28, 30, 120, 14, juce::Justification::centredLeft);

    // 4. Central OLED Screen Glass Frame
    g.setColour(juce::Colour(0xff0d0e12));
    g.fillRoundedRectangle(24.0f, 62.0f, (float) (w - 48), 76.0f, 6.0f);

    g.setColour(juce::Colour(0xff252936));
    g.drawRoundedRectangle(24.0f, 62.0f, (float) (w - 48), 76.0f, 6.0f, 1.2f);

    // 5. Section Outline Cards Lambda (Analog Lab Modular Synth Bay Style)
    auto drawSection = [&](int sx, int sy, int sw, int sh, const juce::String& title, const juce::Colour& accent) {
        g.setColour(juce::Colour(0xff181a22));
        g.fillRoundedRectangle((float) sx, (float) sy, (float) sw, (float) sh, 6.0f);
        
        g.setColour(juce::Colour(0xff272a38));
        g.drawRoundedRectangle((float) sx, (float) sy, (float) sw, (float) sh, 6.0f, 1.0f);
        
        // Header title badge with accent LED pill
        g.setColour(accent);
        g.fillRoundedRectangle((float) (sx + 14), (float) (sy + 12), 4.0f, 12.0f, 2.0f);

        g.setFont(juce::FontOptions(10.5f, juce::Font::bold));
        g.setColour(juce::Colour(0xffd5d9e8));
        g.drawText(title.toUpperCase(), sx + 24, sy + 8, sw - 40, 20, juce::Justification::centredLeft);
    };

    // Draw the 6 modular synth bays with Analog Lab accents
    drawSection(24, 146, 488, 240, "Oscillators & 7-Voice Stereo Unison", juce::Colour(0xff00d4ff));
    drawSection(526, 146, 488, 240, "Dynamic Ladder Filter & Custom Formants", juce::Colour(0xffffaa00));
    drawSection(24, 396, 488, 240, "Amp Envelope & Dual Synced LFO Modulators", juce::Colour(0xff00d4ff));
    drawSection(526, 396, 488, 240, "Master Post FX Chain (Drive, Chorus, Delay & Reverb)", juce::Colour(0xffff5500));
    drawSection(24, 646, 488, 255, "XY Performance Pad & Drop Macro Section", juce::Colour(0xffff2a85));
    drawSection(526, 646, 488, 255, "Voicing, Portamento Glide, Sub Anchor & Transient Shaper", juce::Colour(0xff00ffaa));
}

void KeshaZeddSynthAudioProcessorEditor::resized()
{
    // ----------------------------------------------------
    // Header Bar Layout (y: 0 to 54)
    // ----------------------------------------------------
    prevPresetButton.setBounds(155, 14, 22, 26);
    presetBox.setBounds(180, 14, 190, 26);
    nextPresetButton.setBounds(372, 14, 22, 26);

    savePresetButton.setBounds(400, 14, 46, 26);
    loadPresetButton.setBounds(450, 14, 46, 26);
    diceButton.setBounds(500, 14, 65, 26);

    zeddifyButton.setBounds(575, 14, 85, 26);
    dragMidiButton.setBounds(665, 14, 90, 26);

    autoMasterButton.setBounds(762, 14, 105, 26);

    autoMasterIntensityLabel.setBounds(872, 2, 45, 14);
    autoMasterIntensitySlider.setBounds(872, 14, 45, 36);

    masterVolLabel.setBounds(922, 2, 45, 14);
    masterVolSlider.setBounds(922, 14, 45, 36);

    vuMeter.setBounds(974, 14, 10, 34);
    voiceCountLabel.setBounds(988, 14, 38, 26);

    // ----------------------------------------------------
    // Central OLED Visualizer Glass Screen (y: 64 to 138)
    // ----------------------------------------------------
    audioProcessor.getVisualizer().setBounds(26, 64, getWidth() - 52, 72);

    // ----------------------------------------------------
    // Section 1: Oscillators & Unison (x: 24, y: 146, w: 488, h: 240)
    // ----------------------------------------------------
    osc1ShapeLabel.setBounds(36, 172, 72, 14);
    osc1ShapeSlider.setBounds(36, 186, 72, 46);

    osc1OctaveLabel.setBounds(112, 172, 48, 14);
    osc1OctaveBox.setBounds(112, 188, 48, 22);

    osc1DetuneLabel.setBounds(164, 172, 54, 14);
    osc1DetuneSlider.setBounds(164, 186, 54, 46);

    osc1LevelLabel.setBounds(222, 172, 54, 14);
    osc1LevelSlider.setBounds(222, 186, 54, 46);

    unisonCountLabel.setBounds(282, 172, 65, 14);
    unisonCountBox.setBounds(282, 188, 65, 22);

    unisonDetuneLabel.setBounds(352, 172, 58, 14);
    unisonDetuneSlider.setBounds(352, 186, 58, 46);

    // Oscillator 2 Row (y: 255)
    osc2ShapeLabel.setBounds(36, 255, 72, 14);
    osc2ShapeSlider.setBounds(36, 269, 72, 46);

    osc2OctaveLabel.setBounds(112, 255, 48, 14);
    osc2OctaveBox.setBounds(112, 271, 48, 22);

    osc2DetuneLabel.setBounds(164, 255, 54, 14);
    osc2DetuneSlider.setBounds(164, 269, 54, 46);

    osc2LevelLabel.setBounds(222, 255, 54, 14);
    osc2LevelSlider.setBounds(222, 269, 54, 46);

    oscFmLabel.setBounds(282, 255, 65, 14);
    oscFmSlider.setBounds(282, 269, 65, 46);

    oscSyncLabel.setBounds(352, 255, 65, 14);
    oscSyncBox.setBounds(352, 271, 65, 22);

    // ----------------------------------------------------
    // Section 2: Dynamic Filter & Envelope (x: 526, y: 146, w: 488, h: 240)
    // ----------------------------------------------------
    filterModeLabel.setBounds(538, 172, 75, 14);
    filterModeBox.setBounds(538, 188, 75, 22);

    filterCutoffLabel.setBounds(618, 172, 52, 14);
    filterCutoffSlider.setBounds(618, 186, 52, 46);

    filterResLabel.setBounds(674, 172, 52, 14);
    filterResSlider.setBounds(674, 186, 52, 46);

    filterDriveLabel.setBounds(730, 172, 50, 14);
    filterDriveSlider.setBounds(730, 186, 50, 46);

    filterEnvAmtLabel.setBounds(784, 172, 52, 14);
    filterEnvAmtSlider.setBounds(784, 186, 52, 46);

    filterKeyTrackLabel.setBounds(840, 172, 54, 14);
    filterKeyTrackSlider.setBounds(840, 186, 54, 46);

    filterLfoModLabel.setBounds(898, 172, 50, 14);
    filterLfoModSlider.setBounds(898, 186, 50, 46);

    formantMorphLabel.setBounds(952, 172, 48, 14);
    formantMorphSlider.setBounds(952, 186, 48, 46);

    // Filter Envelope ADSR Row (y: 255)
    filterAttackLabel.setBounds(548, 255, 52, 14);
    filterAttackSlider.setBounds(548, 269, 52, 46);

    filterDecayLabel.setBounds(608, 255, 52, 14);
    filterDecaySlider.setBounds(608, 269, 52, 46);

    filterSustainLabel.setBounds(668, 255, 52, 14);
    filterSustainSlider.setBounds(668, 269, 52, 46);

    filterReleaseLabel.setBounds(728, 255, 52, 14);
    filterReleaseSlider.setBounds(728, 269, 52, 46);

    filterCurveLabel.setBounds(788, 255, 52, 14);
    filterCurveSlider.setBounds(788, 269, 52, 46);

    // ----------------------------------------------------
    // Section 3: Amp Envelope & Dual LFOs (x: 24, y: 396, w: 488, h: 240)
    // ----------------------------------------------------
    ampAttackLabel.setBounds(36, 422, 54, 14);
    ampAttackSlider.setBounds(36, 436, 54, 46);

    ampDecayLabel.setBounds(94, 422, 54, 14);
    ampDecaySlider.setBounds(94, 436, 54, 46);

    ampSustainLabel.setBounds(152, 422, 54, 14);
    ampSustainSlider.setBounds(152, 436, 54, 46);

    ampReleaseLabel.setBounds(210, 422, 54, 14);
    ampReleaseSlider.setBounds(210, 436, 54, 46);

    ampCurveLabel.setBounds(268, 422, 54, 14);
    ampCurveSlider.setBounds(268, 436, 54, 46);

    // LFO 1 Row
    lfo1WaveLabel.setBounds(36, 502, 68, 14);
    lfo1WaveBox.setBounds(36, 518, 68, 22);

    lfo1RateLabel.setBounds(110, 502, 60, 14);
    lfo1RateBox.setBounds(110, 518, 60, 22);

    lfo1ToCutoffLabel.setBounds(176, 502, 58, 14);
    lfo1ToCutoffSlider.setBounds(176, 516, 58, 46);

    lfo1ToShapeLabel.setBounds(238, 502, 58, 14);
    lfo1ToShapeSlider.setBounds(238, 516, 58, 46);

    // LFO 2 Row
    lfo2WaveLabel.setBounds(302, 502, 68, 14);
    lfo2WaveBox.setBounds(302, 518, 68, 22);

    lfo2RateLabel.setBounds(374, 502, 60, 14);
    lfo2RateBox.setBounds(374, 518, 60, 22);

    lfo2ToPitchLabel.setBounds(438, 502, 58, 14);
    lfo2ToPitchSlider.setBounds(438, 516, 58, 46);

    // ----------------------------------------------------
    // Section 4: Post FX Chain (x: 526, y: 396, w: 488, h: 240)
    // ----------------------------------------------------
    fxDriveLabel.setBounds(538, 422, 54, 14);
    fxDriveSlider.setBounds(538, 436, 54, 46);

    fxChorusRateLabel.setBounds(600, 422, 58, 14);
    fxChorusRateSlider.setBounds(600, 436, 58, 46);

    fxChorusDepthLabel.setBounds(664, 422, 58, 14);
    fxChorusDepthSlider.setBounds(664, 436, 58, 46);

    fxChorusMixLabel.setBounds(728, 422, 58, 14);
    fxChorusMixSlider.setBounds(728, 436, 58, 46);

    fxDelayTimeLabel.setBounds(792, 422, 58, 14);
    fxDelayTimeSlider.setBounds(792, 436, 58, 46);

    fxDelayFeedbackLabel.setBounds(856, 422, 58, 14);
    fxDelayFeedbackSlider.setBounds(856, 436, 58, 46);

    fxDelayColorLabel.setBounds(920, 422, 58, 14);
    fxDelayColorSlider.setBounds(920, 436, 58, 46);

    // Delay Mix & Reverbs Row (y: 502)
    fxDelayMixLabel.setBounds(538, 502, 58, 14);
    fxDelayMixSlider.setBounds(538, 516, 58, 46);

    fxReverbDecayLabel.setBounds(608, 502, 58, 14);
    fxReverbDecaySlider.setBounds(608, 516, 58, 46);

    fxReverbDampingLabel.setBounds(678, 502, 58, 14);
    fxReverbDampingSlider.setBounds(678, 516, 58, 46);

    fxReverbWidthLabel.setBounds(748, 502, 58, 14);
    fxReverbWidthSlider.setBounds(748, 516, 58, 46);

    fxReverbMixLabel.setBounds(818, 502, 58, 14);
    fxReverbMixSlider.setBounds(818, 516, 58, 46);

    // ----------------------------------------------------
    // Section 5: Performance Pad & Macros (x: 24, y: 646, w: 488, h: 255)
    // ----------------------------------------------------
    trashGlossPad.setBounds(36, 676, 145, 140);

    macroDropLabel.setBounds(190, 674, 58, 14);
    macroDropSlider.setBounds(190, 688, 58, 50);

    punchLabel.setBounds(252, 674, 58, 14);
    punchSlider.setBounds(252, 688, 58, 50);

    gritLabel.setBounds(314, 674, 58, 14);
    gritSlider.setBounds(314, 688, 58, 50);

    spaceLabel.setBounds(376, 674, 58, 14);
    spaceSlider.setBounds(376, 688, 58, 50);

    widthLabel.setBounds(438, 674, 58, 14);
    widthSlider.setBounds(438, 688, 58, 50);

    // ----------------------------------------------------
    // Section 6: Voicing, Glide & Sub (x: 526, y: 646, w: 488, h: 255)
    // ----------------------------------------------------
    playModeLabel.setBounds(538, 674, 75, 14);
    playModeBox.setBounds(538, 690, 75, 22);

    glideModeLabel.setBounds(618, 674, 75, 14);
    glideModeBox.setBounds(618, 690, 75, 22);

    glideTimeLabel.setBounds(698, 674, 55, 14);
    glideTimeSlider.setBounds(698, 688, 55, 46);

    chordModeLabel.setBounds(760, 674, 75, 14);
    chordModeBox.setBounds(760, 690, 75, 22);

    chordTypeLabel.setBounds(840, 674, 75, 14);
    chordTypeBox.setBounds(840, 690, 75, 22);

    // Sub Anchor Row (y: 755)
    subWaveLabel.setBounds(538, 755, 65, 14);
    subWaveBox.setBounds(538, 770, 65, 22);

    subOctaveLabel.setBounds(608, 755, 55, 14);
    subOctaveBox.setBounds(608, 770, 55, 22);

    subLevelLabel.setBounds(668, 755, 55, 14);
    subLevelSlider.setBounds(668, 768, 55, 46);

    subDriveLabel.setBounds(728, 755, 55, 14);
    subDriveSlider.setBounds(728, 768, 55, 46);

    // Transient Shaper Row
    transientTypeLabel.setBounds(788, 755, 75, 14);
    transientTypeBox.setBounds(788, 770, 75, 22);

    clickLevelLabel.setBounds(868, 755, 60, 14);
    clickLevelSlider.setBounds(868, 768, 60, 46);

    transientDecayLabel.setBounds(932, 755, 60, 14);
    transientDecaySlider.setBounds(932, 768, 60, 46);
}
