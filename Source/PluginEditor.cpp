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

void ModernSynthLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
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
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&lookAndFeel);
    setSize(960, 860);

    // Header Controls
    setupComboBox(presetBox, presetLabel, "PRESET");
    auto& presetNames = audioProcessor.getPresetManager().getPresetNames();
    for (size_t i = 0; i < presetNames.size(); ++i)
        presetBox.addItem(juce::String(i + 1) + ". " + presetNames[i], static_cast<int>(i + 1));
    presetBox.onChange = [this]() {
        audioProcessor.loadPreset(presetBox.getSelectedItemIndex());
    };
    presetAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "preset", presetBox);

    setupSlider(masterVolSlider, masterVolLabel, "MASTER");
    masterVolAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "master_vol", masterVolSlider);

    // Dice & User Presets Buttons
    savePresetButton.setButtonText("SAVE");
    savePresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff181a24));
    savePresetButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    addAndMakeVisible(savePresetButton);
    savePresetButton.onClick = [this]() { showSavePresetDialog(); };

    loadPresetButton.setButtonText("LOAD");
    loadPresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff181a24));
    loadPresetButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    addAndMakeVisible(loadPresetButton);
    loadPresetButton.onClick = [this]() { showLoadPresetDialog(); };

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

    // Osc 1 & Unison Controls
    setupComboBox(osc1WaveBox, osc1WaveLabel, "OSC 1 WAVE");
    osc1WaveBox.addItemList({"Sine", "Saw", "Square", "Triangle", "Noise"}, 1);
    osc1WaveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "osc1_wave", osc1WaveBox);

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

    // Osc 2 Controls
    setupComboBox(osc2WaveBox, osc2WaveLabel, "OSC 2 WAVE");
    osc2WaveBox.addItemList({"Sine", "Saw", "Square", "Triangle", "Noise"}, 1);
    osc2WaveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "osc2_wave", osc2WaveBox);

    setupComboBox(osc2OctaveBox, osc2OctaveLabel, "OCTAVE");
    osc2OctaveBox.addItemList({"-2", "-1", "0", "+1", "+2"}, 1);
    osc2OctaveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "osc2_octave", osc2OctaveBox);

    setupSlider(osc2DetuneSlider, osc2DetuneLabel, "DETUNE");
    osc2DetuneAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "osc2_detune", osc2DetuneSlider);

    setupSlider(osc2LevelSlider, osc2LevelLabel, "LEVEL");
    osc2LevelAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "osc2_level", osc2LevelSlider);

    // Filter Controls
    setupComboBox(filterModeBox, filterModeLabel, "FILTER MODE");
    filterModeBox.addItemList({"LPF 12", "LPF 24", "HPF 12", "HPF 24", "BPF 12", "BPF 24", "Vowel Formant"}, 1);
    filterModeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "filter_mode", filterModeBox);

    setupSlider(filterCutoffSlider, filterCutoffLabel, "CUTOFF");
    filterCutoffAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_cutoff", filterCutoffSlider);

    setupSlider(filterResSlider, filterResLabel, "RESONANCE");
    filterResAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_res", filterResSlider);

    setupSlider(filterEnvAmtSlider, filterEnvAmtLabel, "ENV AMT");
    filterEnvAmtAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_env_amt", filterEnvAmtSlider);

    // Filter ADSR Controls
    setupSlider(filterAttackSlider, filterAttackLabel, "F-ATT", juce::Slider::LinearVertical);
    filterAttackAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_attack", filterAttackSlider);

    setupSlider(filterDecaySlider, filterDecayLabel, "F-DEC", juce::Slider::LinearVertical);
    filterDecayAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_decay", filterDecaySlider);

    setupSlider(filterSustainSlider, filterSustainLabel, "F-SUS", juce::Slider::LinearVertical);
    filterSustainAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_sustain", filterSustainSlider);

    setupSlider(filterReleaseSlider, filterReleaseLabel, "F-REL", juce::Slider::LinearVertical);
    filterReleaseAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_release", filterReleaseSlider);

    // Amp ADSR Controls
    setupSlider(ampAttackSlider, ampAttackLabel, "ATTACK", juce::Slider::LinearVertical);
    ampAttackAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_attack", ampAttackSlider);

    setupSlider(ampDecaySlider, ampDecayLabel, "DECAY", juce::Slider::LinearVertical);
    ampDecayAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_decay", ampDecaySlider);

    setupSlider(ampSustainSlider, ampSustainLabel, "SUSTAIN", juce::Slider::LinearVertical);
    ampSustainAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_sustain", ampSustainSlider);

    setupSlider(ampReleaseSlider, ampReleaseLabel, "RELEASE", juce::Slider::LinearVertical);
    ampReleaseAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_release", ampReleaseSlider);

    // Post FX Controls
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

    setupSlider(fxDelayMixSlider, fxDelayMixLabel, "D-MIX");
    fxDelayMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_delay_mix", fxDelayMixSlider);

    setupSlider(fxReverbMixSlider, fxReverbMixLabel, "R-MIX");
    fxReverbMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_reverb_mix", fxReverbMixSlider);

    // ----------------------------------------------------
    // New Upgraded Features Controls
    // ----------------------------------------------------

    // XY Pad
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

    // Drop Slider
    setupSlider(macroDropSlider, macroDropLabel, "THE DROP", juce::Slider::LinearVertical);
    macroDropAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_drop", macroDropSlider);

    // Macro Knobs
    setupSlider(punchSlider, punchLabel, "PUNCH");
    punchAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_punch", punchSlider);

    setupSlider(gritSlider, gritLabel, "GRIT");
    gritAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_grit", gritSlider);

    setupSlider(spaceSlider, spaceLabel, "SPACE");
    spaceAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_space", spaceSlider);

    setupSlider(widthSlider, widthLabel, "WIDTH");
    widthAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_width", widthSlider);

    // Sub Anchor, Click, Formant, Glide
    setupSlider(subLevelSlider, subLevelLabel, "SUB LEVEL");
    subLevelAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "sub_level", subLevelSlider);

    setupSlider(formantMorphSlider, formantMorphLabel, "VOWEL MORPH");
    formantMorphAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "formant_morph", formantMorphSlider);

    setupSlider(clickLevelSlider, clickLevelLabel, "CLICK LAYER");
    clickLevelAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "click_level", clickLevelSlider);

    setupSlider(glideTimeSlider, glideTimeLabel, "GLIDE GLITCH");
    glideTimeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "glide_time", glideTimeSlider);

    // Voice Combo Boxes
    setupComboBox(playModeBox, playModeLabel, "PLAYBACK VOICING");
    playModeBox.addItemList({"8-Voice Poly", "Mono Legato Glide"}, 1);
    playModeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "play_mode", playModeBox);

    setupComboBox(chordModeBox, chordModeLabel, "SMART CHORD SPREAD");
    chordModeBox.addItemList({"Bypass Engine", "Enabled spreads"}, 1);
    chordModeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "chord_mode", chordModeBox);

    setupComboBox(pumpActiveBox, pumpActiveLabel, "SIDECHAIN PUMP Division");
    pumpActiveBox.addItemList({"Bypass Pumper", "Synced Sidechain"}, 1);
    pumpActiveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "pump_active", pumpActiveBox);

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

    // Update X/Y Pad from parameters when not dragging
    float x = audioProcessor.getAPVTS().getRawParameterValue("trash_gloss_x")->load();
    float y = audioProcessor.getAPVTS().getRawParameterValue("trash_gloss_y")->load();
    if (!juce::ModifierKeys::getCurrentModifiers().isAnyMouseButtonDown())
    {
        trashGlossPad.setPosition(x, y);
    }
}

void KeshaZeddSynthAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text, juce::Slider::SliderStyle style)
{
    slider.setSliderStyle(style);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    label.setColour(juce::Label::textColourId, juce::Colour(0xffc5cad6));
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}

void KeshaZeddSynthAudioProcessorEditor::setupComboBox(juce::ComboBox& box, juce::Label& label, const juce::String& text)
{
    addAndMakeVisible(box);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    label.setColour(juce::Label::textColourId, juce::Colour(0xffc5cad6));
    label.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(label);
}

void KeshaZeddSynthAudioProcessorEditor::showSavePresetDialog()
{
    auto* w = new juce::AlertWindow ("Save User Preset", "Enter a name for your custom preset:", juce::AlertWindow::QuestionIcon);
    w->addTextEditor ("presetName", "My Preset", "");
    w->addButton ("Save", 1, juce::KeyPress (juce::KeyPress::returnKey, 0, 0));
    w->addButton ("Cancel", 0, juce::KeyPress (juce::KeyPress::escapeKey, 0, 0));
    w->setLookAndFeel(&getLookAndFeel());
    
    w->enterModalState (true, juce::ModalCallbackFunction::create([this, w](int result) {
        if (result == 1) // Save clicked
        {
            juce::String name = w->getTextEditorContents ("presetName");
            if (name.isNotEmpty())
                audioProcessor.getPresetManager().saveUserPreset(name);
        }
        delete w;
    }));
}

void KeshaZeddSynthAudioProcessorEditor::showLoadPresetDialog()
{
    juce::File appData = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                            .getChildFile("KeshaAndZeddSynth")
                            .getChildFile("UserPresets");
                            
    juce::Array<juce::File> files;
    appData.findChildFiles(files, juce::File::findFiles, false, "*.json");
    
    if (files.isEmpty())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon, "Load Preset", "No user presets found. Save a preset first!");
        return;
    }
    
    juce::PopupMenu menu;
    for (int i = 0; i < files.size(); ++i)
        menu.addItem(i + 1, files[i].getFileNameWithoutExtension());
        
    menu.showMenuAsync(juce::PopupMenu::Options(), [this, files](int result) {
        if (result > 0 && result <= files.size())
        {
            juce::String name = files[result - 1].getFileNameWithoutExtension();
            audioProcessor.getPresetManager().loadUserPreset(name);
        }
    });
}

void KeshaZeddSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff12131a));

    // Top Header Bar Background
    g.setColour(juce::Colour(0xff1a1c26));
    g.fillRect(0, 0, getWidth(), 55);

    // Title Logo
    g.setColour(juce::Colour(0xff00f2fe));
    g.setFont(juce::FontOptions(18.0f, juce::Font::bold));
    g.drawText("KESHAZEDDSYNTH", 15, 10, 200, 35, juce::Justification::centredLeft);

    // Draw Section Cards helper
    auto drawSectionCard = [&](int x, int y, int w, int h, const juce::String& title) {
        g.setColour(juce::Colour(0xff1a1c26));
        g.fillRoundedRectangle((float)x, (float)y, (float)w, (float)h, 8.0f);

        g.setColour(juce::Colour(0xff232533));
        g.drawRoundedRectangle((float)x, (float)y, (float)w, (float)h, 8.0f, 1.2f);

        g.setColour(juce::Colour(0xff00f2fe));
        g.setFont(juce::FontOptions(12.0f, juce::Font::bold));
        g.drawText(title, x + 15, y + 10, w - 30, 20, juce::Justification::left);
    };

    // Draw the 6 Panel Layout Cards
    drawSectionCard(15, 65, 450, 260, "OSCILLATORS & 7-VOICE UNISON");
    drawSectionCard(480, 65, 465, 260, "LADDER FILTER & FILTER ENV");
    drawSectionCard(15, 340, 450, 260, "AMPLITUDE ENVELOPE (ADSR)");
    drawSectionCard(480, 340, 465, 260, "POST FX (DRIVE, CHORUS, DELAY, REVERB)");
    drawSectionCard(15, 615, 450, 230, "XY MORPH, THE DROP & MACROS");
    drawSectionCard(480, 615, 465, 230, "SUB ANCHOR, FORMANT & ENGINE SETTINGS");
}

void KeshaZeddSynthAudioProcessorEditor::resized()
{
    // Header Bar Layout
    presetLabel.setBounds(230, 5, 80, 20);
    presetBox.setBounds(230, 22, 220, 26);

    savePresetButton.setBounds(465, 22, 55, 26);
    loadPresetButton.setBounds(525, 22, 55, 26);
    diceButton.setBounds(590, 22, 60, 26);

    masterVolLabel.setBounds(665, 5, 60, 15);
    masterVolSlider.setBounds(665, 18, 60, 34);

    voiceCountLabel.setBounds(760, 18, 185, 26);

    // Section 1: Oscillators & Unison (x: 15, y: 65, w: 450, h: 260)
    osc1WaveLabel.setBounds(30, 95, 80, 15);
    osc1WaveBox.setBounds(30, 112, 100, 24);

    osc1OctaveLabel.setBounds(140, 95, 60, 15);
    osc1OctaveBox.setBounds(140, 112, 60, 24);

    osc1DetuneLabel.setBounds(210, 95, 55, 15);
    osc1DetuneSlider.setBounds(210, 110, 55, 50);

    osc1LevelLabel.setBounds(275, 95, 55, 15);
    osc1LevelSlider.setBounds(275, 110, 55, 50);

    unisonCountLabel.setBounds(340, 95, 80, 15);
    unisonCountBox.setBounds(340, 112, 100, 24);

    unisonDetuneLabel.setBounds(340, 140, 100, 15);
    unisonDetuneSlider.setBounds(362, 155, 55, 50);

    osc2WaveLabel.setBounds(30, 175, 80, 15);
    osc2WaveBox.setBounds(30, 192, 100, 24);

    osc2OctaveLabel.setBounds(140, 175, 60, 15);
    osc2OctaveBox.setBounds(140, 192, 60, 24);

    osc2DetuneLabel.setBounds(210, 175, 55, 15);
    osc2DetuneSlider.setBounds(210, 190, 55, 50);

    osc2LevelLabel.setBounds(275, 175, 55, 15);
    osc2LevelSlider.setBounds(275, 190, 55, 50);

    // Section 2: Filter & Filter Env (x: 480, y: 65, w: 465, h: 260)
    filterModeLabel.setBounds(495, 95, 90, 15);
    filterModeBox.setBounds(495, 112, 110, 24);

    filterCutoffLabel.setBounds(620, 95, 60, 15);
    filterCutoffSlider.setBounds(620, 110, 60, 55);

    filterResLabel.setBounds(690, 95, 75, 15);
    filterResSlider.setBounds(690, 110, 60, 55);

    filterEnvAmtLabel.setBounds(770, 95, 60, 15);
    filterEnvAmtSlider.setBounds(770, 110, 60, 55);

    int filterAdsrY = 175;
    filterAttackLabel.setBounds(505, filterAdsrY, 50, 15);
    filterAttackSlider.setBounds(505, filterAdsrY + 18, 50, 115);

    filterDecayLabel.setBounds(565, filterAdsrY, 50, 15);
    filterDecaySlider.setBounds(565, filterAdsrY + 18, 50, 115);

    filterSustainLabel.setBounds(625, filterAdsrY, 50, 15);
    filterSustainSlider.setBounds(625, filterAdsrY + 18, 50, 115);

    filterReleaseLabel.setBounds(685, filterAdsrY, 50, 15);
    filterReleaseSlider.setBounds(685, filterAdsrY + 18, 50, 115);

    // Section 3: Amp ADSR (x: 15, y: 340, w: 450, h: 260)
    int ampAdsrY = 375;
    int ampAdsrHeight = 175;
    ampAttackLabel.setBounds(40, ampAdsrY, 70, 15);
    ampAttackSlider.setBounds(40, ampAdsrY + 20, 70, ampAdsrHeight);

    ampDecayLabel.setBounds(140, ampAdsrY, 70, 15);
    ampDecaySlider.setBounds(140, ampAdsrY + 20, 70, ampAdsrHeight);

    ampSustainLabel.setBounds(240, ampAdsrY, 70, 15);
    ampSustainSlider.setBounds(240, ampAdsrY + 20, 70, ampAdsrHeight);

    ampReleaseLabel.setBounds(340, ampAdsrY, 70, 15);
    ampReleaseSlider.setBounds(340, ampAdsrY + 20, 70, ampAdsrHeight);

    // Section 4: Post FX (x: 480, y: 340, w: 465, h: 260)
    // Saturation/Reverb
    fxDriveLabel.setBounds(495, 370, 70, 15);
    fxDriveSlider.setBounds(495, 388, 65, 60);

    fxReverbMixLabel.setBounds(495, 470, 70, 15);
    fxReverbMixSlider.setBounds(495, 488, 65, 60);

    // Chorus
    fxChorusRateLabel.setBounds(585, 370, 60, 15);
    fxChorusRateSlider.setBounds(585, 388, 60, 60);

    fxChorusDepthLabel.setBounds(655, 370, 65, 15);
    fxChorusDepthSlider.setBounds(655, 388, 60, 60);

    fxChorusMixLabel.setBounds(725, 370, 60, 15);
    fxChorusMixSlider.setBounds(725, 388, 60, 60);

    // Delay
    fxDelayTimeLabel.setBounds(585, 470, 60, 15);
    fxDelayTimeSlider.setBounds(585, 488, 60, 60);

    fxDelayFeedbackLabel.setBounds(655, 470, 75, 15);
    fxDelayFeedbackSlider.setBounds(655, 488, 60, 60);

    fxDelayMixLabel.setBounds(740, 470, 60, 15);
    fxDelayMixSlider.setBounds(740, 488, 60, 60);

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

    spaceLabel.setBounds(280, 725, 60, 15);
    spaceSlider.setBounds(280, 740, 60, 60);

    widthLabel.setBounds(360, 725, 60, 15);
    widthSlider.setBounds(360, 740, 60, 60);

    // Section 6: Sub Anchor, Formant & Settings (x: 480, y: 615, w: 465, h: 230)
    subLevelLabel.setBounds(495, 645, 60, 15);
    subLevelSlider.setBounds(495, 660, 60, 60);

    formantMorphLabel.setBounds(565, 645, 60, 15);
    formantMorphSlider.setBounds(565, 660, 60, 60);

    clickLevelLabel.setBounds(635, 645, 60, 15);
    clickLevelSlider.setBounds(635, 660, 60, 60);

    glideTimeLabel.setBounds(705, 645, 60, 15);
    glideTimeSlider.setBounds(705, 660, 60, 60);

    playModeLabel.setBounds(780, 645, 140, 15);
    playModeBox.setBounds(780, 660, 140, 24);

    chordModeLabel.setBounds(780, 700, 140, 15);
    chordModeBox.setBounds(780, 715, 140, 24);

    pumpActiveLabel.setBounds(780, 755, 140, 15);
    pumpActiveBox.setBounds(780, 770, 140, 24);
}
