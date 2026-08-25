#include "PluginProcessor.h"
#include "PluginEditor.h"

// ==============================================================================
// Analog Lab Inspired LookAndFeel Implementation
// ==============================================================================
ModernSynthLookAndFeel::ModernSynthLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff121318));
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff1b1d26));
    setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff333748));
    setColour(juce::ComboBox::focusedOutlineColourId, juce::Colour(0xff00d4ff));
    setColour(juce::ComboBox::textColourId, juce::Colour(0xffe2e5f0));
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff161720));
    setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xff00d4ff).withAlpha(0.25f));
    setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    setColour(juce::PopupMenu::textColourId, juce::Colour(0xffc5cad8));
}

void ModernSynthLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                              juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f - 6.0f;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // 1. Soft Drop Shadow
    g.setColour(juce::Colour(0x55000000));
    g.fillEllipse(rx + 1.0f, ry + 2.5f, rw, rw);

    // 2. Outer Track Arc
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colour(0xff1e202a));
    g.strokePath(backgroundArc, juce::PathStrokeType(3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // 3. Illuminated LED Value Arc
    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
        
        juce::ColourGradient grad(juce::Colour(0xff00d4ff), centreX - radius, centreY,
                                  juce::Colour(0xffffaa00), centreX + radius, centreY, false);
        g.setGradientFill(grad);
        g.strokePath(valueArc, juce::PathStrokeType(3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // 4. Outer Dial Metallic Cap
    auto dialRadius = radius - 4.5f;
    auto drx = centreX - dialRadius;
    auto dry = centreY - dialRadius;
    auto drw = dialRadius * 2.0f;

    g.setColour(juce::Colour(0xff2a2d3b));
    g.fillEllipse(drx, dry, drw, drw);

    juce::ColourGradient faceGrad(juce::Colour(0xff252834), centreX - dialRadius, centreY - dialRadius,
                                  juce::Colour(0xff14151b), centreX + dialRadius, centreY + dialRadius, false);
    g.setGradientFill(faceGrad);
    g.fillEllipse(drx + 1.2f, dry + 1.2f, drw - 2.4f, drw - 2.4f);

    g.setColour(juce::Colour(0xff3c4052).withAlpha(0.7f));
    g.drawEllipse(drx + 1.2f, dry + 1.2f, drw - 2.4f, drw - 2.4f, 1.0f);

    // 5. High-Visibility Pointer Line
    juce::Path p;
    auto pointerLength = dialRadius * 0.70f;
    auto pointerThickness = 2.2f;
    p.addRoundedRectangle(-pointerThickness * 0.5f, -dialRadius + 2.0f, pointerThickness, pointerLength, 1.0f);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    
    g.setColour(juce::Colour(0xffffffff));
    g.fillPath(p);

    // Center illuminated pip
    g.setColour(juce::Colour(0xff111217));
    g.fillEllipse(centreX - 2.5f, centreY - 2.5f, 5.0f, 5.0f);
    g.setColour(juce::Colour(0xff00d4ff));
    g.fillEllipse(centreX - 1.2f, centreY - 1.2f, 2.4f, 2.4f);
}

void ModernSynthLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPos, float minSliderPos, float maxSliderPos,
                                              const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
}

void ModernSynthLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool /*isButtonDown*/,
                                          int buttonX, int buttonY, int buttonW, int buttonH,
                                          juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<float>(0.0f, 0.0f, (float) width, (float) height);
    
    g.setColour(juce::Colour(0xff1a1c24));
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(box.hasKeyboardFocus(true) ? juce::Colour(0xff00d4ff) : juce::Colour(0xff2d3140));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    // Chevron Arrow
    juce::Path path;
    float ax = (float) buttonX + (float) buttonW * 0.5f;
    float ay = (float) buttonY + (float) buttonH * 0.5f;
    path.startNewSubPath(ax - 3.0f, ay - 1.5f);
    path.lineTo(ax, ay + 2.0f);
    path.lineTo(ax + 3.0f, ay - 1.5f);

    g.setColour(juce::Colour(0xff8c92a6));
    g.strokePath(path, juce::PathStrokeType(1.4f));
}

void ModernSynthLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                              bool shouldDrawButtonAsHighlighted, bool /*shouldDrawButtonAsDown*/)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.5f);
    bool isOn = button.getToggleState();

    g.setColour(isOn ? juce::Colour(0xff222735) : (shouldDrawButtonAsHighlighted ? juce::Colour(0xff1c1e27) : juce::Colour(0xff161820)));
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(isOn ? juce::Colour(0xff00d4ff) : juce::Colour(0xff2b2f3e));
    g.drawRoundedRectangle(bounds, 4.0f, isOn ? 1.4f : 1.0f);

    // Glowing LED pip
    float ledX = bounds.getX() + 9.0f;
    float ledY = bounds.getCentreY();
    
    if (isOn)
    {
        g.setColour(juce::Colour(0x5500d4ff));
        g.fillEllipse(ledX - 4.5f, ledY - 4.5f, 9.0f, 9.0f);
        g.setColour(juce::Colour(0xff00d4ff));
        g.fillEllipse(ledX - 2.5f, ledY - 2.5f, 5.0f, 5.0f);
    }
    else
    {
        g.setColour(juce::Colour(0xff303444));
        g.fillEllipse(ledX - 2.5f, ledY - 2.5f, 5.0f, 5.0f);
    }

    g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    g.setColour(isOn ? juce::Colour(0xffffffff) : juce::Colour(0xff8a90a4));
    g.drawText(button.getButtonText(), bounds.withTrimmedLeft(18.0f), juce::Justification::centredLeft, true);
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

    g.setColour(shouldDrawButtonAsHighlighted ? juce::Colour(0xff00d4ff) : juce::Colour(0xff303444));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
}

// ==============================================================================
// Plugin Editor Constructor & Setup
// ==============================================================================
KeshaZeddSynthAudioProcessorEditor::KeshaZeddSynthAudioProcessorEditor(KeshaZeddSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), dragMidiButton(p), vuMeter(p)
{
    setLookAndFeel(&lookAndFeel);
    setSize(860, 560);

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

    savePresetButton.setButtonText("SAVE");
    savePresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1d202b));
    addAndMakeVisible(savePresetButton);
    savePresetButton.onClick = [this]() { showSavePresetDialog(); };

    loadPresetButton.setButtonText("LOAD");
    loadPresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1d202b));
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

    diceButton.setButtonText("DICE");
    diceButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff281e30));
    addAndMakeVisible(diceButton);
    diceButton.onClick = [this]() {
        audioProcessor.randomizeParameters();
        presetBox.setText("Custom Random", juce::dontSendNotification);
    };

    zeddifyButton.setButtonText("ZEDDIFY");
    addAndMakeVisible(zeddifyButton);
    zeddifyAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "zeddify_active", zeddifyButton);

    autoMasterButton.setButtonText("AUTO-MASTER");
    addAndMakeVisible(autoMasterButton);
    autoMasterAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "auto_master_active", autoMasterButton);

    addAndMakeVisible(dragMidiButton);
    addAndMakeVisible(vuMeter);
    addAndMakeVisible(audioProcessor.getVisualizer());

    setupSlider(masterVolSlider, masterVolLabel, "VOLUME");
    masterVolAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "master_vol", masterVolSlider);

    voiceCountLabel.setText("8 VOICES", juce::dontSendNotification);
    voiceCountLabel.setFont(juce::FontOptions(9.5f, juce::Font::bold));
    voiceCountLabel.setColour(juce::Label::textColourId, juce::Colour(0xff757a8e));
    addAndMakeVisible(voiceCountLabel);

    // ----------------------------------------------------
    // SECTION 1: SOUND ENGINE (Left Bay)
    // ----------------------------------------------------
    setupSlider(osc1ShapeSlider, osc1ShapeLabel, "MORPH");
    osc1ShapeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "osc1_shape", osc1ShapeSlider);

    setupSlider(unisonDetuneSlider, unisonDetuneLabel, "UNISON");
    unisonDetuneAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "unison_detune", unisonDetuneSlider);

    setupSlider(subLevelSlider, subLevelLabel, "SUB BASS");
    subLevelAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "sub_level", subLevelSlider);

    setupSlider(filterCutoffSlider, filterCutoffLabel, "CUTOFF");
    filterCutoffAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_cutoff", filterCutoffSlider);

    setupSlider(filterResSlider, filterResLabel, "RESONANCE");
    filterResAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_res", filterResSlider);

    setupSlider(oscFmSlider, oscFmLabel, "BRIGHTNESS");
    oscFmAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "osc_fm_depth", oscFmSlider);

    setupComboBox(osc1OctaveBox, osc1OctaveLabel, "OCTAVE");
    osc1OctaveBox.addItemList({"-2 Oct", "-1 Oct", "0 Oct", "+1 Oct", "+2 Oct"}, 1);
    osc1OctaveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "osc1_octave", osc1OctaveBox);

    setupComboBox(filterModeBox, filterModeLabel, "FILTER TYPE");
    filterModeBox.addItemList({"LPF 12dB", "LPF 24dB", "BPF 12dB", "HPF 12dB", "Notch", "Formant"}, 1);
    filterModeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "filter_mode", filterModeBox);

    // ----------------------------------------------------
    // SECTION 2: SLIDE & PERFORMANCE (Center Bay)
    // ----------------------------------------------------
    slideToggle.setButtonText("SLIDE / GLIDE");
    addAndMakeVisible(slideToggle);
    slideToggle.onClick = [this]() {
        // Toggle play mode between Mono Legato (1) and Poly (0)
        float currentMode = audioProcessor.getAPVTS().getRawParameterValue("play_mode")->load();
        float newMode = (currentMode == 0.0f) ? 1.0f : 0.0f;
        if (auto* param = audioProcessor.getAPVTS().getParameter("play_mode"))
            param->setValueNotifyingHost(param->getNormalisableRange().convertTo0to1(newMode));
    };

    setupSlider(glideTimeSlider, glideTimeLabel, "GLIDE TIME");
    glideTimeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "glide_time", glideTimeSlider);

    setupSlider(ampAttackSlider, ampAttackLabel, "ATTACK");
    ampAttackAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_attack", ampAttackSlider);

    setupSlider(ampDecaySlider, ampDecayLabel, "DECAY");
    ampDecayAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_decay", ampDecaySlider);

    setupSlider(ampSustainSlider, ampSustainLabel, "SUSTAIN");
    ampSustainAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_sustain", ampSustainSlider);

    setupSlider(ampReleaseSlider, ampReleaseLabel, "RELEASE");
    ampReleaseAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_release", ampReleaseSlider);

    setupSlider(macroDropSlider, macroDropLabel, "THE DROP");
    macroDropAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_drop", macroDropSlider);

    setupSlider(punchSlider, punchLabel, "PUNCH");
    punchAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_punch", punchSlider);

    setupComboBox(playModeBox, playModeLabel, "VOICING");
    playModeBox.addItemList({"Poly (8 Voices)", "Mono Legato", "Mono Retrig"}, 1);
    playModeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "play_mode", playModeBox);

    // ----------------------------------------------------
    // SECTION 3: EFFECTS & SPACE (Right Bay)
    // ----------------------------------------------------
    setupSlider(fxDriveSlider, fxDriveLabel, "GRIT");
    fxDriveAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_drive", fxDriveSlider);

    setupSlider(fxChorusMixSlider, fxChorusMixLabel, "CHORUS");
    fxChorusMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_chorus_mix", fxChorusMixSlider);

    setupSlider(fxDelayTimeSlider, fxDelayTimeLabel, "DELAY TIME");
    fxDelayTimeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_delay_time", fxDelayTimeSlider);

    setupSlider(fxDelayMixSlider, fxDelayMixLabel, "DELAY MIX");
    fxDelayMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_delay_mix", fxDelayMixSlider);

    setupSlider(fxReverbDecaySlider, fxReverbDecayLabel, "REVERB SIZE");
    fxReverbDecayAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_reverb_decay", fxReverbDecaySlider);

    setupSlider(fxReverbMixSlider, fxReverbMixLabel, "REVERB MIX");
    fxReverbMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_reverb_mix", fxReverbMixSlider);

    pumpToggle.setButtonText("SIDECHAIN PUMP");
    addAndMakeVisible(pumpToggle);
    pumpAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "pump_active", pumpToggle);

    monoMakerToggle.setButtonText("SUB MONO LOCK");
    addAndMakeVisible(monoMakerToggle);
    monoMakerAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "mono_maker_active", monoMakerToggle);

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
    voiceCountLabel.setText(juce::String(activeCount) + " / 8", juce::dontSendNotification);

    // Sync slide toggle state with play_mode (active if Mono Legato)
    float mode = audioProcessor.getAPVTS().getRawParameterValue("play_mode")->load();
    slideToggle.setToggleState(mode > 0.5f, juce::dontSendNotification);

    // Sync active preset selection in combobox asynchronously
    int activePreset = static_cast<int>(audioProcessor.getAPVTS().getRawParameterValue("preset")->load());
    if (activePreset != presetBox.getSelectedItemIndex() && activePreset >= 0 && activePreset < audioProcessor.getPresetManager().getNumPresets())
    {
        presetBox.setSelectedItemIndex(activePreset, juce::dontSendNotification);
    }
}

void KeshaZeddSynthAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text, juce::Slider::SliderStyle style)
{
    slider.setSliderStyle(style);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 13);
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
    label.setFont(juce::FontOptions(9.0f, juce::Font::bold));
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

    // 1. Background
    g.fillAll(juce::Colour(0xff111216));

    // 2. Vintage Wooden Side Cheeks
    auto drawWoodPanel = [&](float x, float width) {
        juce::ColourGradient woodGrad(juce::Colour(0xff452314), x, 0.0f,
                                      juce::Colour(0xff200f07), x + width, 0.0f, false);
        g.setGradientFill(woodGrad);
        g.fillRect(x, 0.0f, width, (float) h);

        g.setColour(juce::Colour(0xff5c301c).withAlpha(0.35f));
        for (float ly = 12.0f; ly < h; ly += 24.0f)
            g.drawHorizontalLine(static_cast<int>(ly), x + 1.0f, x + width - 1.0f);
    };

    drawWoodPanel(0.0f, 10.0f);
    drawWoodPanel((float) (w - 10), 10.0f);

    // 3. Top Header Bar
    juce::ColourGradient headerGrad(juce::Colour(0xff1f212a), 0.0f, 0.0f,
                                    juce::Colour(0xff14151c), 0.0f, 46.0f, false);
    g.setGradientFill(headerGrad);
    g.fillRect(10, 0, w - 20, 46);

    g.setColour(juce::Colour(0xff2a2d3a));
    g.drawHorizontalLine(46, 10.0f, (float) (w - 10));

    // Logo Typography
    g.setFont(juce::FontOptions(17.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffffffff));
    g.drawText("KZ-SYNTH", 22, 6, 95, 20, juce::Justification::centredLeft);

    g.setFont(juce::FontOptions(8.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffffaa00));
    g.drawText("ANALOG LAB", 22, 24, 95, 12, juce::Justification::centredLeft);

    // 4. Central OLED Screen Bezel Frame
    g.setColour(juce::Colour(0xff090a0e));
    g.fillRoundedRectangle(16.0f, 52.0f, (float) (w - 32), 60.0f, 5.0f);

    g.setColour(juce::Colour(0xff222532));
    g.drawRoundedRectangle(16.0f, 52.0f, (float) (w - 32), 60.0f, 5.0f, 1.0f);

    // 5. Modular Section Cards
    auto drawSection = [&](int sx, int sy, int sw, int sh, const juce::String& title, const juce::Colour& accent) {
        g.setColour(juce::Colour(0xff161820));
        g.fillRoundedRectangle((float) sx, (float) sy, (float) sw, (float) sh, 5.0f);
        
        g.setColour(juce::Colour(0xff242734));
        g.drawRoundedRectangle((float) sx, (float) sy, (float) sw, (float) sh, 5.0f, 1.0f);
        
        g.setColour(accent);
        g.fillRoundedRectangle((float) (sx + 12), (float) (sy + 10), 3.0f, 10.0f, 1.5f);

        g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
        g.setColour(juce::Colour(0xffd0d4e4));
        g.drawText(title.toUpperCase(), sx + 20, sy + 6, sw - 30, 18, juce::Justification::centredLeft);
    };

    drawSection(16, 118, 268, 430, "Sound Engine & Tone", juce::Colour(0xff00d4ff));
    drawSection(292, 118, 276, 430, "Slide & Performance", juce::Colour(0xffffaa00));
    drawSection(576, 118, 268, 430, "Effects & Space", juce::Colour(0xffff5500));
}

void KeshaZeddSynthAudioProcessorEditor::resized()
{
    // ----------------------------------------------------
    // Header Bar Layout (y: 0 to 46)
    // ----------------------------------------------------
    prevPresetButton.setBounds(120, 11, 20, 24);
    presetBox.setBounds(142, 11, 195, 24);
    nextPresetButton.setBounds(339, 11, 20, 24);

    savePresetButton.setBounds(365, 11, 42, 24);
    loadPresetButton.setBounds(410, 11, 42, 24);
    diceButton.setBounds(455, 11, 42, 24);

    zeddifyButton.setBounds(505, 11, 74, 24);
    autoMasterButton.setBounds(583, 11, 92, 24);
    dragMidiButton.setBounds(679, 11, 80, 24);

    masterVolLabel.setBounds(764, 2, 40, 12);
    masterVolSlider.setBounds(764, 12, 40, 32);

    vuMeter.setBounds(810, 12, 8, 30);
    voiceCountLabel.setBounds(822, 12, 26, 24);

    // ----------------------------------------------------
    // Central OLED Screen (y: 54 to 110)
    // ----------------------------------------------------
    audioProcessor.getVisualizer().setBounds(18, 54, getWidth() - 36, 56);

    // ----------------------------------------------------
    // Section 1: Sound Engine (x: 16, y: 118, w: 268, h: 430)
    // ----------------------------------------------------
    // Row 1 (y: 145)
    osc1ShapeLabel.setBounds(26, 142, 72, 13);
    osc1ShapeSlider.setBounds(26, 155, 72, 54);

    unisonDetuneLabel.setBounds(112, 142, 72, 13);
    unisonDetuneSlider.setBounds(112, 155, 72, 54);

    subLevelLabel.setBounds(198, 142, 72, 13);
    subLevelSlider.setBounds(198, 155, 72, 54);

    // Row 2 (y: 225)
    filterCutoffLabel.setBounds(26, 222, 72, 13);
    filterCutoffSlider.setBounds(26, 235, 72, 54);

    filterResLabel.setBounds(112, 222, 72, 13);
    filterResSlider.setBounds(112, 235, 72, 54);

    oscFmLabel.setBounds(198, 222, 72, 13);
    oscFmSlider.setBounds(198, 235, 72, 54);

    // Row 3 Dropdowns (y: 310)
    osc1OctaveLabel.setBounds(30, 310, 105, 13);
    osc1OctaveBox.setBounds(30, 325, 105, 22);

    filterModeLabel.setBounds(155, 310, 115, 13);
    filterModeBox.setBounds(155, 325, 115, 22);

    // ----------------------------------------------------
    // Section 2: Slide & Performance (x: 292, y: 118, w: 276, h: 430)
    // ----------------------------------------------------
    // Prominent Slide Controls (y: 145)
    slideToggle.setBounds(306, 145, 115, 24);

    glideTimeLabel.setBounds(430, 132, 65, 13);
    glideTimeSlider.setBounds(430, 145, 65, 52);

    playModeLabel.setBounds(306, 178, 115, 13);
    playModeBox.setBounds(306, 192, 115, 22);

    // Amp ADSR Envelopes (y: 230)
    ampAttackLabel.setBounds(302, 230, 56, 13);
    ampAttackSlider.setBounds(302, 243, 56, 50);

    ampDecayLabel.setBounds(364, 230, 56, 13);
    ampDecaySlider.setBounds(364, 243, 56, 50);

    ampSustainLabel.setBounds(426, 230, 56, 13);
    ampSustainSlider.setBounds(426, 243, 56, 50);

    ampReleaseLabel.setBounds(488, 230, 56, 13);
    ampReleaseSlider.setBounds(488, 243, 56, 50);

    // Macros (y: 310)
    macroDropLabel.setBounds(330, 308, 85, 13);
    macroDropSlider.setBounds(330, 322, 85, 58);

    punchLabel.setBounds(435, 308, 85, 13);
    punchSlider.setBounds(435, 322, 85, 58);

    // ----------------------------------------------------
    // Section 3: Effects & Space (x: 576, y: 118, w: 268, h: 430)
    // ----------------------------------------------------
    // Row 1 (y: 145)
    fxDriveLabel.setBounds(586, 142, 72, 13);
    fxDriveSlider.setBounds(586, 155, 72, 54);

    fxChorusMixLabel.setBounds(672, 142, 72, 13);
    fxChorusMixSlider.setBounds(672, 155, 72, 54);

    fxDelayTimeLabel.setBounds(758, 142, 72, 13);
    fxDelayTimeSlider.setBounds(758, 155, 72, 54);

    // Row 2 (y: 225)
    fxDelayMixLabel.setBounds(586, 222, 72, 13);
    fxDelayMixSlider.setBounds(586, 235, 72, 54);

    fxReverbDecayLabel.setBounds(672, 222, 72, 13);
    fxReverbDecaySlider.setBounds(672, 235, 72, 54);

    fxReverbMixLabel.setBounds(758, 222, 72, 13);
    fxReverbMixSlider.setBounds(758, 235, 72, 54);

    // Toggles (y: 320)
    pumpToggle.setBounds(590, 318, 115, 24);
    monoMakerToggle.setBounds(715, 318, 115, 24);
}
