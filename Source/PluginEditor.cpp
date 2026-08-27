#include "PluginProcessor.h"
#include "PluginEditor.h"

// ==============================================================================
// ModernSynthLookAndFeel (Supports Beach Festival & Luxury Themes)
// ==============================================================================
ModernSynthLookAndFeel::ModernSynthLookAndFeel()
{
    setTheme(BeachFestival);
}

void ModernSynthLookAndFeel::setTheme(int themeIndex)
{
    currentTheme = juce::jlimit(0, 3, themeIndex);

    if (currentTheme == MiamiVice)
    {
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff090a14));
        setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff121324));
        setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff3a2b5d));
        setColour(juce::ComboBox::focusedOutlineColourId, juce::Colour(0xffff007f));
        setColour(juce::ComboBox::textColourId, juce::Colour(0xfff0e6ff));
        setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff0f1020));
        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xffff007f).withAlpha(0.3f));
        setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
        setColour(juce::PopupMenu::textColourId, juce::Colour(0xffdfd0f8));
    }
    else if (currentTheme == IbizaGold)
    {
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff14120e));
        setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff221e17));
        setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff4a3f2d));
        setColour(juce::ComboBox::focusedOutlineColourId, juce::Colour(0xffffd166));
        setColour(juce::ComboBox::textColourId, juce::Colour(0xfff5eccb));
        setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff1a1712));
        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xffffd166).withAlpha(0.25f));
        setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
        setColour(juce::PopupMenu::textColourId, juce::Colour(0xffeed9a2));
    }
    else if (currentTheme == StealthBlackout)
    {
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff0a0a0c));
        setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff131317));
        setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff26262d));
        setColour(juce::ComboBox::focusedOutlineColourId, juce::Colour(0xff05ffa1));
        setColour(juce::ComboBox::textColourId, juce::Colour(0xffcccccc));
        setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff111114));
        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xff05ffa1).withAlpha(0.2f));
        setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
        setColour(juce::PopupMenu::textColourId, juce::Colour(0xffaaaaaa));
    }
    else // BeachFestival (Default Sunset Vibes)
    {
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff0c1017));
        setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff141923));
        setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff283348));
        setColour(juce::ComboBox::focusedOutlineColourId, juce::Colour(0xffff6b35));
        setColour(juce::ComboBox::textColourId, juce::Colour(0xffe2e8f5));
        setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff121620));
        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xffff6b35).withAlpha(0.3f));
        setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
        setColour(juce::PopupMenu::textColourId, juce::Colour(0xffc5d2e8));
    }
}

juce::Colour ModernSynthLookAndFeel::getBgColour() const
{
    if (currentTheme == MiamiVice) return juce::Colour(0xff090a14);
    if (currentTheme == IbizaGold) return juce::Colour(0xff14120e);
    if (currentTheme == StealthBlackout) return juce::Colour(0xff0a0a0c);
    return juce::Colour(0xff0c1017);
}

juce::Colour ModernSynthLookAndFeel::getCardBgColour() const
{
    if (currentTheme == MiamiVice) return juce::Colour(0xff101222);
    if (currentTheme == IbizaGold) return juce::Colour(0xff1c1812);
    if (currentTheme == StealthBlackout) return juce::Colour(0xff121216);
    return juce::Colour(0xff141924);
}

juce::Colour ModernSynthLookAndFeel::getCardBorderColour() const
{
    if (currentTheme == MiamiVice) return juce::Colour(0xff2c224a);
    if (currentTheme == IbizaGold) return juce::Colour(0xff3d3423);
    if (currentTheme == StealthBlackout) return juce::Colour(0xff222228);
    return juce::Colour(0xff222c3e);
}

juce::Colour ModernSynthLookAndFeel::getAccentColour(int bayIndex) const
{
    if (currentTheme == MiamiVice)
    {
        if (bayIndex == 0) return juce::Colour(0xff00f0ff);
        if (bayIndex == 1) return juce::Colour(0xffff007f);
        return juce::Colour(0xffffe600);
    }
    if (currentTheme == IbizaGold)
    {
        if (bayIndex == 0) return juce::Colour(0xffffd166);
        if (bayIndex == 1) return juce::Colour(0xfff77f00);
        return juce::Colour(0xff06d6a0);
    }
    if (currentTheme == StealthBlackout)
    {
        if (bayIndex == 0) return juce::Colour(0xff05ffa1);
        if (bayIndex == 1) return juce::Colour(0xff00e5ff);
        return juce::Colour(0xffcccccc);
    }
    // BeachFestival Sunset
    if (bayIndex == 0) return juce::Colour(0xffff6b35); // Sunset Orange
    if (bayIndex == 1) return juce::Colour(0xff00f0ff); // Tropical Cyan
    return juce::Colour(0xffff2a6d); // Neon Coral Pink
}

void ModernSynthLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                              juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(2.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Track background
    g.setColour(juce::Colour(0xff121622));
    g.fillEllipse(rx, ry, rw, rw);

    g.setColour(juce::Colour(0xff222b3e));
    g.drawEllipse(rx, ry, rw, rw, 1.2f);

    // Track arc
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(centreX, centreY, radius - 3.5f, radius - 3.5f, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colour(0xff1b2333));
    g.strokePath(backgroundArc, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Value active arc
    if (sliderPos > 0.001f)
    {
        juce::Path valueArc;
        valueArc.addCentredArc(centreX, centreY, radius - 3.5f, radius - 3.5f, 0.0f, rotaryStartAngle, angle, true);
        
        juce::ColourGradient grad(juce::Colour(0xffff6b35), centreX - radius, centreY,
                                  juce::Colour(0xff00f0ff), centreX + radius, centreY, false);
        if (sliderPos > 0.6f)
            grad.addColour(0.8, juce::Colour(0xffff2a6d));

        g.setGradientFill(grad);
        g.strokePath(valueArc, juce::PathStrokeType(4.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Dial Cap
    float innerR = radius - 8.0f;
    juce::ColourGradient dialGrad(juce::Colour(0xff222a3a), centreX, centreY - innerR,
                                  juce::Colour(0xff141924), centreX, centreY + innerR, false);
    g.setGradientFill(dialGrad);
    g.fillEllipse(centreX - innerR, centreY - innerR, innerR * 2.0f, innerR * 2.0f);

    g.setColour(juce::Colour(0xff2d3950));
    g.drawEllipse(centreX - innerR, centreY - innerR, innerR * 2.0f, innerR * 2.0f, 1.0f);

    // Pointer line
    juce::Path p;
    auto pointerLength = innerR - 2.0f;
    auto pointerThickness = 2.4f;
    p.addRoundedRectangle(-pointerThickness * 0.5f, -innerR + 2.0f, pointerThickness, pointerLength * 0.65f, 1.0f);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    
    g.setColour(slider.isMouseOverOrDragging() ? juce::Colour(0xff00f0ff) : juce::Colour(0xffff6b35));
    g.fillPath(p);
}

void ModernSynthLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool /*isButtonDown*/,
                                          int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                                          juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat().reduced(0.5f);

    g.setColour(findColour(juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(box.hasKeyboardFocus(true) ? findColour(juce::ComboBox::focusedOutlineColourId) 
                                           : findColour(juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    // Arrow
    auto arrowZone = juce::Rectangle<float>((float) (width - 18), 0.0f, 14.0f, (float) height);
    juce::Path path;
    path.startNewSubPath(arrowZone.getX() + 2.0f, arrowZone.getCentreY() - 2.0f);
    path.lineTo(arrowZone.getCentreX(), arrowZone.getCentreY() + 3.0f);
    path.lineTo(arrowZone.getRight() - 2.0f, arrowZone.getCentreY() - 2.0f);

    g.setColour(box.findColour(juce::ComboBox::textColourId).withAlpha(0.7f));
    g.strokePath(path, juce::PathStrokeType(1.5f));
}

void ModernSynthLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                                  const juce::Colour& backgroundColour,
                                                  bool shouldDrawButtonAsHighlighted,
                                                  bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);
    auto baseCol = backgroundColour;

    if (button.getToggleState())
        baseCol = juce::Colour(0xffff6b35);
    else if (shouldDrawButtonAsDown)
        baseCol = baseCol.brighter(0.2f);
    else if (shouldDrawButtonAsHighlighted)
        baseCol = baseCol.brighter(0.1f);

    g.setColour(baseCol);
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(button.getToggleState() ? juce::Colour(0xffffd166) : baseCol.brighter(0.3f));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
}

void ModernSynthLookAndFeel::drawSidePanels(juce::Graphics& g, int width, int height) const
{
    float panelWidth = 8.0f;
    juce::ColourGradient leftGrad(juce::Colour(0xffff6b35).withAlpha(0.25f), 0.0f, 0.0f,
                                  juce::Colour(0xff0c1017), panelWidth, 0.0f, false);
    g.setGradientFill(leftGrad);
    g.fillRect(0.0f, 0.0f, panelWidth, (float) height);

    juce::ColourGradient rightGrad(juce::Colour(0xff0c1017), (float) (width - panelWidth), 0.0f,
                                   juce::Colour(0xff00f0ff).withAlpha(0.25f), (float) width, 0.0f, false);
    g.setGradientFill(rightGrad);
    g.fillRect((float) (width - panelWidth), 0.0f, panelWidth, (float) height);
}

// ==============================================================================
// KeshaZeddSynthAudioProcessorEditor Implementation
// ==============================================================================
KeshaZeddSynthAudioProcessorEditor::KeshaZeddSynthAudioProcessorEditor(KeshaZeddSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
      dragMidiButton(p, DragMidiButton::Zeddify),
      dragChordButton(p, DragMidiButton::Chords),
      dragHookButton(p, DragMidiButton::Hook),
      dragVaultButton(p, DragMidiButton::Vault),
      vuMeter(p),
      envDisplay(p)
{
    setLookAndFeel(&lookAndFeel);
    setSize(980, 580);

    // ----------------------------------------------------
    // HEADER BAR: MODE SWITCHER & LOGO
    // ----------------------------------------------------
    producerModeButton.setButtonText("⚡ PRODUCER MODE");
    producerModeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffff6b35));
    producerModeButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    addAndMakeVisible(producerModeButton);
    producerModeButton.onClick = [this]() {
        if (isAdvancedMode)
        {
            isAdvancedMode = false;
            producerModeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffff6b35));
            advancedModeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff18202e));
            updateModeVisibility();
            resized();
            repaint();
        }
    };

    advancedModeButton.setButtonText("🛠️ ADVANCED DESIGNER");
    advancedModeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff18202e));
    advancedModeButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    addAndMakeVisible(advancedModeButton);
    advancedModeButton.onClick = [this]() {
        if (!isAdvancedMode)
        {
            isAdvancedMode = true;
            producerModeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff18202e));
            advancedModeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff00f0ff));
            updateModeVisibility();
            resized();
            repaint();
        }
    };

    // Preset Controls
    prevPresetButton.setButtonText("<");
    addAndMakeVisible(prevPresetButton);
    prevPresetButton.onClick = [this]() {
        int cur = static_cast<int>(audioProcessor.getAPVTS().getRawParameterValue("preset")->load());
        int num = audioProcessor.getPresetManager().getNumPresets();
        int prev = (cur - 1 + num) % num;
        if (auto* param = audioProcessor.getAPVTS().getParameter("preset"))
            param->setValueNotifyingHost(param->getNormalisableRange().convertTo0to1(prev));
    };

    nextPresetButton.setButtonText(">");
    addAndMakeVisible(nextPresetButton);
    nextPresetButton.onClick = [this]() {
        int cur = static_cast<int>(audioProcessor.getAPVTS().getRawParameterValue("preset")->load());
        int num = audioProcessor.getPresetManager().getNumPresets();
        int next = (cur + 1) % num;
        if (auto* param = audioProcessor.getAPVTS().getParameter("preset"))
            param->setValueNotifyingHost(param->getNormalisableRange().convertTo0to1(next));
    };

    presetBox.setText("Select Preset...", juce::dontSendNotification);
    presetBox.onPresetSelected = [this](int index) {
        if (auto* param = audioProcessor.getAPVTS().getParameter("preset"))
            param->setValueNotifyingHost(param->getNormalisableRange().convertTo0to1(index));
    };
    addAndMakeVisible(presetBox);

    savePresetButton.setButtonText("SAVE");
    savePresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1f2738));
    addAndMakeVisible(savePresetButton);
    savePresetButton.onClick = [this]() { showSavePresetDialog(); };

    loadPresetButton.setButtonText("LOAD");
    loadPresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1f2738));
    addAndMakeVisible(loadPresetButton);
    loadPresetButton.onClick = [this]() {
        auto chooser = std::make_shared<juce::FileChooser>("Load User Preset...",
                                  juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("LeesHouse").getChildFile("UserPresets"),
                                  "*.kzpreset;*.json");
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

    diceButton.setButtonText("🎲 RAND");
    diceButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2a344a));
    addAndMakeVisible(diceButton);
    diceButton.onClick = [this]() { audioProcessor.randomizeParameters(); };

    mutateButton.setButtonText("MUTATE");
    mutateButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2a344a));
    addAndMakeVisible(mutateButton);
    mutateButton.onClick = [this]() { audioProcessor.getZeddifyEngine().mutateCurrentPattern(); };

    autoMasterButton.setButtonText("• AUTO-MASTER");
    autoMasterButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffffd166));
    autoMasterButton.setColour(juce::TextButton::textColourOnId, juce::Colour(0xff10141e));
    autoMasterButton.setClickingTogglesState(true);
    addAndMakeVisible(autoMasterButton);
    autoMasterAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "auto_master_active", autoMasterButton);

    themeBox.addItemList({"Beach Sunset", "Miami Vice", "Ibiza Gold", "Stealth Carbon"}, 1);
    addAndMakeVisible(themeBox);
    themeBox.onChange = [this]() {
        lookAndFeel.setTheme(themeBox.getSelectedItemIndex());
        repaint();
    };
    themeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "ui_theme", themeBox);

    setupSlider(masterVolSlider, masterVolLabel, "MASTER");
    masterVolAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "master_vol", masterVolSlider);

    addAndMakeVisible(vuMeter);

    voiceCountLabel.setText("0 / 8", juce::dontSendNotification);
    voiceCountLabel.setFont(juce::FontOptions(9.0f, juce::Font::bold));
    voiceCountLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00f0ff));
    addAndMakeVisible(voiceCountLabel);

    // Visualizer Component
    addAndMakeVisible(audioProcessor.getVisualizer());

    // ----------------------------------------------------
    // PRODUCER MODE (SIMPLE) 8 MACROS & QUICK TOOLS
    // ----------------------------------------------------
    setupSlider(beachPunchSlider, beachPunchLabel, "BEACH PUNCH");
    beachPunchAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_punch", beachPunchSlider);

    setupSlider(festivalFilterSlider, festivalFilterLabel, "FESTIVAL FILTER");
    festivalFilterAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_cutoff", festivalFilterSlider);

    setupSlider(sidechainPumpSlider, sidechainPumpLabel, "SIDECHAIN PUMP");
    sidechainPumpAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "pump_depth", sidechainPumpSlider);
    sidechainPumpSlider.onValueChange = [this]() {
        if (sidechainPumpSlider.getValue() > 0.05f)
        {
            if (auto* param = audioProcessor.getAPVTS().getParameter("pump_active"))
                param->setValueNotifyingHost(1.0f);
        }
    };

    setupSlider(spaceReverbSlider, spaceReverbLabel, "SPACE / REVERB");
    spaceReverbAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_reverb_mix", spaceReverbSlider);

    setupSlider(echoDelaySlider, echoDelayLabel, "ECHO DELAY");
    echoDelayAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_delay_mix", echoDelaySlider);

    setupSlider(stereoWideSlider, stereoWideLabel, "STEREO WIDE");
    stereoWideAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_width", stereoWideSlider);

    setupSlider(glossAirSlider, glossAirLabel, "GLOSS / AIR");
    glossAirAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "trash_gloss_y", glossAirSlider);

    setupSlider(warmthDriveSlider, warmthDriveLabel, "WARMTH / DRIVE");
    warmthDriveAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_drive", warmthDriveSlider);

    // Instant Quick-FX Buttons
    phaserQuickToggle.setButtonText("🌊 PHASER");
    phaserQuickToggle.setColour(juce::ToggleButton::textColourId, juce::Colour(0xff00f0ff));
    addAndMakeVisible(phaserQuickToggle);
    phaserQuickAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "phaser_active", phaserQuickToggle);
    phaserQuickToggle.onClick = [this]() {
        if (phaserQuickToggle.getToggleState())
        {
            if (auto* pMix = audioProcessor.getAPVTS().getParameter("phaser_mix"))
            {
                if (pMix->getValue() < 0.1f)
                    pMix->setValueNotifyingHost(0.5f);
            }
        }
    };

    glitterQuickToggle.setButtonText("✨ GLITTER");
    glitterQuickToggle.setColour(juce::ToggleButton::textColourId, juce::Colour(0xffffd166));
    addAndMakeVisible(glitterQuickToggle);
    glitterQuickToggle.onClick = [this]() {
        float cur = audioProcessor.getAPVTS().getRawParameterValue("glitter_mix")->load();
        float target = (cur > 0.2f) ? 0.0f : 0.65f;
        if (auto* param = audioProcessor.getAPVTS().getParameter("glitter_mix"))
            param->setValueNotifyingHost(param->getNormalisableRange().convertTo0to1(target));
        glitterQuickToggle.setToggleState(target > 0.1f, juce::dontSendNotification);
    };

    trashQuickToggle.setButtonText("🔥 TRASH GRIT");
    trashQuickToggle.setColour(juce::ToggleButton::textColourId, juce::Colour(0xffff2a6d));
    addAndMakeVisible(trashQuickToggle);
    trashQuickToggle.onClick = [this]() {
        float cur = audioProcessor.getAPVTS().getRawParameterValue("trash_gloss_x")->load();
        float target = (cur > 0.2f) ? 0.0f : 0.75f;
        if (auto* param = audioProcessor.getAPVTS().getParameter("trash_gloss_x"))
            param->setValueNotifyingHost(target);
        trashQuickToggle.setToggleState(target > 0.1f, juce::dontSendNotification);
    };

    monoMakerQuickToggle.setButtonText("🛡️ MONO-MAKER");
    monoMakerQuickToggle.setColour(juce::ToggleButton::textColourId, juce::Colour(0xff00f0ff));
    addAndMakeVisible(monoMakerQuickToggle);
    monoMakerQuickAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "mono_maker_active", monoMakerQuickToggle);

    // 4 Momentary Glitch Trigger Pads
    tapeStopPad = std::make_unique<MomentaryPadButton>("🛑 TAPE STOP", juce::Colour(0xffff2a6d), [this](bool down) {
        if (auto* param = audioProcessor.getAPVTS().getParameter("glitch_mode"))
            param->setValueNotifyingHost(down ? 0.25f : 0.0f);
    });
    addAndMakeVisible(*tapeStopPad);

    stutterPad = std::make_unique<MomentaryPadButton>("⚡ STUTTER", juce::Colour(0xff00f0ff), [this](bool down) {
        if (auto* param = audioProcessor.getAPVTS().getParameter("glitch_mode"))
            param->setValueNotifyingHost(down ? 0.50f : 0.0f);
    });
    addAndMakeVisible(*stutterPad);

    divePad = std::make_unique<MomentaryPadButton>("🔻 DIVE", juce::Colour(0xffffd166), [this](bool down) {
        if (auto* param = audioProcessor.getAPVTS().getParameter("glitch_mode"))
            param->setValueNotifyingHost(down ? 0.75f : 0.0f);
    });
    addAndMakeVisible(*divePad);

    reversePad = std::make_unique<MomentaryPadButton>("⏪ REVERSE", juce::Colour(0xff05ffa1), [this](bool down) {
        if (auto* param = audioProcessor.getAPVTS().getParameter("glitch_mode"))
            param->setValueNotifyingHost(down ? 1.00f : 0.0f);
    });
    addAndMakeVisible(*reversePad);

    // ----------------------------------------------------
    // ADVANCED SOUND DESIGNER COMPONENTS
    // ----------------------------------------------------
    setupSlider(osc1ShapeSlider, osc1ShapeLabel, "SHAPE");
    osc1ShapeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "osc1_shape", osc1ShapeSlider);

    setupSlider(unisonDetuneSlider, unisonDetuneLabel, "UNISON");
    unisonDetuneAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "unison_detune", unisonDetuneSlider);

    setupSlider(subLevelSlider, subLevelLabel, "SUB");
    subLevelAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "sub_level", subLevelSlider);

    setupSlider(filterCutoffSlider, filterCutoffLabel, "CUTOFF");
    filterCutoffAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_cutoff", filterCutoffSlider);

    setupSlider(filterResSlider, filterResLabel, "RES");
    filterResAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filter_res", filterResSlider);

    setupSlider(layerBMixSlider, layerBMixLabel, "HYBRID MIX");
    layerBMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "layer_b_mix", layerBMixSlider);

    setupComboBox(osc1OctaveBox, osc1OctaveLabel, "OCTAVE");
    osc1OctaveBox.addItemList({"-2 Oct", "-1 Oct", "0 (Norm)", "+1 Oct", "+2 Oct"}, 1);
    osc1OctaveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "osc1_octave", osc1OctaveBox);

    setupComboBox(filterModeBox, filterModeLabel, "FILTER MODE");
    filterModeBox.addItemList({"LPF 12dB", "LPF 24dB", "HPF 12dB", "BPF 12dB", "Comb Clean", "Formant Vowel"}, 1);
    filterModeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "filter_mode", filterModeBox);

    setupComboBox(layerBTypeBox, layerBTypeLabel, "LAYER B ATTACK");
    layerBTypeBox.addItemList({"Off", "Punch Click", "Acoustic Slap", "Metallic Glitch", "Noise Transient", "Sub Drop Hit"}, 1);
    layerBTypeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "layer_b_type", layerBTypeBox);

    // FL 6-Stage Envelope
    addAndMakeVisible(envDisplay);

    setupSlider(envDelaySlider, envDelayLabel, "DEL");
    envDelayAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "env_delay", envDelaySlider);

    setupSlider(ampAttackSlider, ampAttackLabel, "ATT");
    ampAttackAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_attack", ampAttackSlider);

    setupSlider(envHoldSlider, envHoldLabel, "HOLD");
    envHoldAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "env_hold", envHoldSlider);

    setupSlider(ampDecaySlider, ampDecayLabel, "DEC");
    ampDecayAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_decay", ampDecaySlider);

    setupSlider(ampSustainSlider, ampSustainLabel, "SUS");
    ampSustainAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_sustain", ampSustainSlider);

    setupSlider(ampReleaseSlider, ampReleaseLabel, "REL");
    ampReleaseAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "amp_release", ampReleaseSlider);

    setupSlider(envDecTensionSlider, envDecTensionLabel, "DEC TENS");
    envDecTensionAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "env_dec_tension", envDecTensionSlider);

    setupSlider(envRelTensionSlider, envRelTensionLabel, "REL TENS");
    envRelTensionAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "env_rel_tension", envRelTensionSlider);

    // Echo Delay
    setupSlider(echoFeedSlider, echoFeedLabel, "FEED");
    echoFeedAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fl_echo_feed", echoFeedSlider);

    setupSlider(echoTimeSlider, echoTimeLabel, "TIME");
    echoTimeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fl_echo_time", echoTimeSlider);

    setupSlider(echoPanSlider, echoPanLabel, "PAN");
    echoPanAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fl_echo_pan", echoPanSlider);

    setupSlider(echoPitchSlider, echoPitchLabel, "PITCH");
    echoPitchAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fl_echo_pitch", echoPitchSlider);

    setupSlider(echoCountSlider, echoCountLabel, "ECHOES");
    echoCountAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fl_echo_count", echoCountSlider);

    echoPingPongToggle.setButtonText("PING PONG");
    addAndMakeVisible(echoPingPongToggle);
    echoPingPongAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "fl_echo_pingpong", echoPingPongToggle);

    echoFatToggle.setButtonText("FAT MODE");
    addAndMakeVisible(echoFatToggle);
    echoFatAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "fl_echo_fat", echoFatToggle);

    setupSlider(glideTimeSlider, glideTimeLabel, "GLIDE TIME");
    glideTimeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "glide_time", glideTimeSlider);

    setupComboBox(chordProgBox, chordProgLabel, "CHORD SETS");
    chordProgBox.addItemList({"Chords Off", "Pop Axis (I-V-vi-IV)", "Dark EDM (i-VI-III-VII)", 
                              "Emotional (vi-IV-I-V)", "Future Bass (IV-I-vi-V)", "80s Synthwave", 
                              "50s Doo-Wop", "Royal Road", "Beach Party"}, 1);
    chordProgAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "chord_prog_preset", chordProgBox);

    setupComboBox(harmonizerBox, harmonizerLabel, "HARMONIZE");
    harmonizerBox.addItemList({"Off", "3rds", "5ths", "Octaves", "Vocal Stack"}, 1);
    harmonizerAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "harmonizer_mode", harmonizerBox);

    setupComboBox(autoBassBox, autoBassLabel, "AUTO-BASS");
    autoBassBox.addItemList({"Off", "Rolling 16ths", "Offbeat 8ths", "4-on-Floor", "Nu-Disco"}, 1);
    autoBassAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "auto_bass_mode", autoBassBox);

    hookGenToggle.setButtonText("HOOK GEN");
    addAndMakeVisible(hookGenToggle);
    hookGenAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "hook_generator_active", hookGenToggle);

    generateHookButton.setButtonText("GEN HOOK");
    generateHookButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2b3a4a));
    addAndMakeVisible(generateHookButton);
    generateHookButton.onClick = [this]() {
        int mood = hookMoodBox.getSelectedItemIndex();
        int root = static_cast<int>(audioProcessor.getAPVTS().getRawParameterValue("scale_root")->load());
        int scale = static_cast<int>(audioProcessor.getAPVTS().getRawParameterValue("scale_type")->load());
        audioProcessor.getHookEngine().generateNewHook(mood, root, scale);
    };

    setupComboBox(scaleRootBox, scaleRootLabel, "ROOT");
    scaleRootBox.addItemList({"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"}, 1);
    scaleRootAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "scale_root", scaleRootBox);

    setupComboBox(scaleTypeBox, scaleTypeLabel, "SCALE");
    scaleTypeBox.addItemList({"Scale Off", "Major", "Minor", "Harmonic Min", "Dorian", "Mixolydian", "Pentatonic"}, 1);
    scaleTypeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "scale_type", scaleTypeBox);

    // Detailed 4-Stage Phaser Controls in Advanced Mode
    setupSlider(phaserRateSlider, phaserRateLabel, "PHASER RATE");
    phaserRateAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "phaser_rate", phaserRateSlider);

    setupSlider(phaserDepthSlider, phaserDepthLabel, "PHASER DEPTH");
    phaserDepthAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "phaser_depth", phaserDepthSlider);

    setupSlider(phaserFeedbackSlider, phaserFeedbackLabel, "PHASER FEEDBACK");
    phaserFeedbackAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "phaser_feedback", phaserFeedbackSlider);

    setupSlider(phaserMixSlider, phaserMixLabel, "PHASER MIX");
    phaserMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "phaser_mix", phaserMixSlider);

    setupSlider(fxDriveSlider, fxDriveLabel, "DRIVE");
    fxDriveAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_drive", fxDriveSlider);

    setupSlider(fxChorusMixSlider, fxChorusMixLabel, "CHORUS");
    fxChorusMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_chorus_mix", fxChorusMixSlider);

    setupSlider(fxDelayTimeSlider, fxDelayTimeLabel, "DELAY TIME");
    fxDelayTimeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_delay_time", fxDelayTimeSlider);

    setupSlider(fxReverbDecaySlider, fxReverbDecayLabel, "REVERB SIZE");
    fxReverbDecayAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_reverb_decay", fxReverbDecaySlider);

    setupSlider(analogDriftSlider, analogDriftLabel, "VCO DRIFT");
    analogDriftAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "analog_drift", analogDriftSlider);

    setupSlider(glitterMixSlider, glitterMixLabel, "GLITTER CLOUD");
    glitterMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "glitter_mix", glitterMixSlider);

    setupSlider(glitterGrainSlider, glitterGrainLabel, "GRAIN SIZE");
    glitterGrainAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "glitter_grain_size", glitterGrainSlider);

    pumpToggle.setButtonText("SIDECHAIN PUMP");
    addAndMakeVisible(pumpToggle);
    pumpAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "pump_active", pumpToggle);

    monoMakerToggle.setButtonText("SUB MONO LOCK");
    addAndMakeVisible(monoMakerToggle);
    monoMakerAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "mono_maker_active", monoMakerToggle);

    setupComboBox(producerFlavorBox, producerFlavorLabel, "PRODUCER FLAVOR");
    producerFlavorBox.addItemList({"Clean Club", "Zedd Complextro", "Kesha Glitter", "Hyperpop Trash", "Tape Warmth"}, 1);
    producerFlavorAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "producer_flavor", producerFlavorBox);

    setupSlider(producerFlavorIntensitySlider, producerFlavorIntensityLabel, "FLAVOR MIX");
    producerFlavorIntensityAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "producer_flavor_intensity", producerFlavorIntensitySlider);

    // Initial visibility state (Default = Producer Mode)
    updateModeVisibility();

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

    int activePreset = static_cast<int>(audioProcessor.getAPVTS().getRawParameterValue("preset")->load());
    if (activePreset != presetBox.getSelectedItemIndex() && activePreset >= 0 && activePreset < audioProcessor.getPresetManager().getNumPresets())
    {
        presetBox.setText(audioProcessor.getPresetManager().getPresetName(activePreset), juce::dontSendNotification);
    }

    if (isAdvancedMode)
        envDisplay.repaint();
}

void KeshaZeddSynthAudioProcessorEditor::updateModeVisibility()
{
    // Producer Mode components
    beachPunchSlider.setVisible(!isAdvancedMode);
    beachPunchLabel.setVisible(!isAdvancedMode);
    festivalFilterSlider.setVisible(!isAdvancedMode);
    festivalFilterLabel.setVisible(!isAdvancedMode);
    sidechainPumpSlider.setVisible(!isAdvancedMode);
    sidechainPumpLabel.setVisible(!isAdvancedMode);
    spaceReverbSlider.setVisible(!isAdvancedMode);
    spaceReverbLabel.setVisible(!isAdvancedMode);
    echoDelaySlider.setVisible(!isAdvancedMode);
    echoDelayLabel.setVisible(!isAdvancedMode);
    stereoWideSlider.setVisible(!isAdvancedMode);
    stereoWideLabel.setVisible(!isAdvancedMode);
    glossAirSlider.setVisible(!isAdvancedMode);
    glossAirLabel.setVisible(!isAdvancedMode);
    warmthDriveSlider.setVisible(!isAdvancedMode);
    warmthDriveLabel.setVisible(!isAdvancedMode);

    phaserQuickToggle.setVisible(!isAdvancedMode);
    glitterQuickToggle.setVisible(!isAdvancedMode);
    trashQuickToggle.setVisible(!isAdvancedMode);
    monoMakerQuickToggle.setVisible(!isAdvancedMode);

    if (tapeStopPad) tapeStopPad->setVisible(!isAdvancedMode);
    if (stutterPad)  stutterPad->setVisible(!isAdvancedMode);
    if (divePad)     divePad->setVisible(!isAdvancedMode);
    if (reversePad)  reversePad->setVisible(!isAdvancedMode);

    // Advanced Mode components
    osc1ShapeSlider.setVisible(isAdvancedMode);
    osc1ShapeLabel.setVisible(isAdvancedMode);
    osc1OctaveBox.setVisible(isAdvancedMode);
    osc1OctaveLabel.setVisible(isAdvancedMode);
    unisonDetuneSlider.setVisible(isAdvancedMode);
    unisonDetuneLabel.setVisible(isAdvancedMode);
    subLevelSlider.setVisible(isAdvancedMode);
    subLevelLabel.setVisible(isAdvancedMode);
    filterCutoffSlider.setVisible(isAdvancedMode);
    filterCutoffLabel.setVisible(isAdvancedMode);
    filterResSlider.setVisible(isAdvancedMode);
    filterResLabel.setVisible(isAdvancedMode);
    filterModeBox.setVisible(isAdvancedMode);
    filterModeLabel.setVisible(isAdvancedMode);
    layerBTypeBox.setVisible(isAdvancedMode);
    layerBTypeLabel.setVisible(isAdvancedMode);
    layerBMixSlider.setVisible(isAdvancedMode);
    layerBMixLabel.setVisible(isAdvancedMode);

    envDisplay.setVisible(isAdvancedMode);
    envDelaySlider.setVisible(isAdvancedMode);
    envDelayLabel.setVisible(isAdvancedMode);
    ampAttackSlider.setVisible(isAdvancedMode);
    ampAttackLabel.setVisible(isAdvancedMode);
    envHoldSlider.setVisible(isAdvancedMode);
    envHoldLabel.setVisible(isAdvancedMode);
    ampDecaySlider.setVisible(isAdvancedMode);
    ampDecayLabel.setVisible(isAdvancedMode);
    ampSustainSlider.setVisible(isAdvancedMode);
    ampSustainLabel.setVisible(isAdvancedMode);
    ampReleaseSlider.setVisible(isAdvancedMode);
    ampReleaseLabel.setVisible(isAdvancedMode);
    envDecTensionSlider.setVisible(isAdvancedMode);
    envDecTensionLabel.setVisible(isAdvancedMode);
    envRelTensionSlider.setVisible(isAdvancedMode);
    envRelTensionLabel.setVisible(isAdvancedMode);

    echoFeedSlider.setVisible(isAdvancedMode);
    echoFeedLabel.setVisible(isAdvancedMode);
    echoTimeSlider.setVisible(isAdvancedMode);
    echoTimeLabel.setVisible(isAdvancedMode);
    echoPanSlider.setVisible(isAdvancedMode);
    echoPanLabel.setVisible(isAdvancedMode);
    echoPitchSlider.setVisible(isAdvancedMode);
    echoPitchLabel.setVisible(isAdvancedMode);
    echoCountSlider.setVisible(isAdvancedMode);
    echoCountLabel.setVisible(isAdvancedMode);
    echoPingPongToggle.setVisible(isAdvancedMode);
    echoFatToggle.setVisible(isAdvancedMode);
    glideTimeSlider.setVisible(isAdvancedMode);
    glideTimeLabel.setVisible(isAdvancedMode);

    chordProgBox.setVisible(isAdvancedMode);
    chordProgLabel.setVisible(isAdvancedMode);
    harmonizerBox.setVisible(isAdvancedMode);
    harmonizerLabel.setVisible(isAdvancedMode);
    autoBassBox.setVisible(isAdvancedMode);
    autoBassLabel.setVisible(isAdvancedMode);
    hookGenToggle.setVisible(isAdvancedMode);
    generateHookButton.setVisible(isAdvancedMode);
    scaleRootBox.setVisible(isAdvancedMode);
    scaleRootLabel.setVisible(isAdvancedMode);
    scaleTypeBox.setVisible(isAdvancedMode);
    scaleTypeLabel.setVisible(isAdvancedMode);

    phaserRateSlider.setVisible(isAdvancedMode);
    phaserRateLabel.setVisible(isAdvancedMode);
    phaserDepthSlider.setVisible(isAdvancedMode);
    phaserDepthLabel.setVisible(isAdvancedMode);
    phaserFeedbackSlider.setVisible(isAdvancedMode);
    phaserFeedbackLabel.setVisible(isAdvancedMode);
    phaserMixSlider.setVisible(isAdvancedMode);
    phaserMixLabel.setVisible(isAdvancedMode);

    fxDriveSlider.setVisible(isAdvancedMode);
    fxDriveLabel.setVisible(isAdvancedMode);
    fxChorusMixSlider.setVisible(isAdvancedMode);
    fxChorusMixLabel.setVisible(isAdvancedMode);
    fxDelayTimeSlider.setVisible(isAdvancedMode);
    fxDelayTimeLabel.setVisible(isAdvancedMode);
    fxReverbDecaySlider.setVisible(isAdvancedMode);
    fxReverbDecayLabel.setVisible(isAdvancedMode);
    analogDriftSlider.setVisible(isAdvancedMode);
    analogDriftLabel.setVisible(isAdvancedMode);
    glitterMixSlider.setVisible(isAdvancedMode);
    glitterMixLabel.setVisible(isAdvancedMode);
    glitterGrainSlider.setVisible(isAdvancedMode);
    glitterGrainLabel.setVisible(isAdvancedMode);
    pumpToggle.setVisible(isAdvancedMode);
    monoMakerToggle.setVisible(isAdvancedMode);
    producerFlavorBox.setVisible(isAdvancedMode);
    producerFlavorLabel.setVisible(isAdvancedMode);
    producerFlavorIntensitySlider.setVisible(isAdvancedMode);
    producerFlavorIntensityLabel.setVisible(isAdvancedMode);
}

void KeshaZeddSynthAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text, juce::Slider::SliderStyle style)
{
    slider.setSliderStyle(style);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 52, 12);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffe2e8f5));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::FontOptions(9.0f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colour(0xff94a3b8));
    addAndMakeVisible(label);
}

void KeshaZeddSynthAudioProcessorEditor::setupComboBox(juce::ComboBox& box, juce::Label& label, const juce::String& text)
{
    addAndMakeVisible(box);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::FontOptions(9.0f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centredLeft);
    label.setColour(juce::Label::textColourId, juce::Colour(0xff94a3b8));
    addAndMakeVisible(label);
}

void KeshaZeddSynthAudioProcessorEditor::showSavePresetDialog()
{
    auto* dialog = new juce::AlertWindow("Save User Preset", "Enter category and preset name:", juce::AlertWindow::QuestionIcon);
    dialog->addTextEditor("category", "01_Custom", "Category:");
    dialog->addTextEditor("name", "My Festival Sound", "Preset Name:");
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
    g.fillAll(lookAndFeel.getBgColour());

    // 2. Themed Side Panels
    lookAndFeel.drawSidePanels(g, w, h);

    // 3. Top Header Bar with Sunset Gradient
    juce::ColourGradient headerGrad(juce::Colour(0xff181e2b), 0.0f, 0.0f,
                                    juce::Colour(0xff10141e), 0.0f, 48.0f, false);
    g.setGradientFill(headerGrad);
    g.fillRect(8, 0, w - 16, 48);

    g.setColour(lookAndFeel.getCardBorderColour());
    g.drawHorizontalLine(48, 8.0f, (float) (w - 8));

    // Logo Typography: LEE'S HOUSE
    g.setFont(juce::FontOptions(18.0f, juce::Font::bold));
    juce::ColourGradient logoGrad(juce::Colour(0xffff6b35), 18.0f, 10.0f,
                                  juce::Colour(0xffffd166), 130.0f, 10.0f, false);
    g.setGradientFill(logoGrad);
    g.drawText("🌴 LEE'S HOUSE", 16, 6, 140, 20, juce::Justification::centredLeft);

    g.setFont(juce::FontOptions(8.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xff00f0ff));
    g.drawText("BEACH FESTIVAL SYNTH", 18, 26, 140, 12, juce::Justification::centredLeft);

    // Section card helper
    auto drawSection = [&](int sx, int sy, int sw, int sh, const juce::String& title, const juce::Colour& accent) {
        g.setColour(lookAndFeel.getCardBgColour());
        g.fillRoundedRectangle((float) sx, (float) sy, (float) sw, (float) sh, 6.0f);
        
        g.setColour(lookAndFeel.getCardBorderColour());
        g.drawRoundedRectangle((float) sx, (float) sy, (float) sw, (float) sh, 6.0f, 1.0f);
        
        g.setColour(accent);
        g.fillRoundedRectangle((float) (sx + 12), (float) (sy + 10), 3.5f, 12.0f, 1.5f);

        g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
        g.setColour(juce::Colour(0xffe2e8f5));
        g.drawText(title.toUpperCase(), sx + 22, sy + 7, sw - 30, 18, juce::Justification::centredLeft);
    };

    if (!isAdvancedMode)
    {
        // ----------------------------------------------------
        // PRODUCER MODE (SIMPLE) CARDS
        // ----------------------------------------------------
        // Top Left: Live Waveform Visualizer & Screen
        drawSection(16, 56, 320, 240, "Live Festival Audio Monitor", juce::Colour(0xff00f0ff));

        // Top Right: 8 Large Festival Macro Knobs
        drawSection(346, 56, 618, 240, "Festival Sound Shaping & Mix Macros", juce::Colour(0xffff6b35));

        // Bottom Bar: Instant Sound FX & DJ Performance Pads
        drawSection(16, 306, 948, 258, "Instant Festival Sound FX & DJ Performance Pads", juce::Colour(0xffff2a6d));
    }
    else
    {
        // ----------------------------------------------------
        // ADVANCED SOUND DESIGNER CARDS
        // ----------------------------------------------------
        drawSection(16, 56, 304, 508, "Sound Engine & Hybrid Layer", lookAndFeel.getAccentColour(0));
        drawSection(330, 56, 320, 508, "FL Envelope & Performance Suite", lookAndFeel.getAccentColour(1));
        drawSection(660, 56, 304, 508, "Effects & 4-Stage Phaser Suite", lookAndFeel.getAccentColour(2));
    }
}

void KeshaZeddSynthAudioProcessorEditor::resized()
{
    // ----------------------------------------------------
    // Header Bar Layout (y: 0 to 48)
    // ----------------------------------------------------
    producerModeButton.setBounds(165, 11, 135, 26);
    advancedModeButton.setBounds(305, 11, 155, 26);

    prevPresetButton.setBounds(470, 11, 20, 26);
    presetBox.setBounds(492, 11, 170, 26);
    nextPresetButton.setBounds(664, 11, 20, 26);

    autoMasterButton.setBounds(690, 11, 105, 26);
    diceButton.setBounds(800, 11, 62, 26);
    themeBox.setBounds(866, 11, 98, 26);

    if (!isAdvancedMode)
    {
        // ====================================================
        // PRODUCER MODE (SIMPLE VIEW)
        // ====================================================
        // Top Left: Visualizer (x: 16, y: 56, w: 320, h: 240)
        audioProcessor.getVisualizer().setBounds(26, 88, 300, 192);

        // Top Right: 8 Festival Macros in 4x2 Grid (x: 346, y: 56, w: 618, h: 240)
        // Row 1 (y: 90)
        int macroW = 68;
        int macroH = 68;
        int spacingX = 148;
        int startX = 360;

        beachPunchLabel.setBounds(startX, 86, macroW + 40, 14);
        beachPunchSlider.setBounds(startX + 6, 100, macroW, macroH);

        festivalFilterLabel.setBounds(startX + spacingX, 86, macroW + 40, 14);
        festivalFilterSlider.setBounds(startX + spacingX + 6, 100, macroW, macroH);

        sidechainPumpLabel.setBounds(startX + spacingX * 2, 86, macroW + 40, 14);
        sidechainPumpSlider.setBounds(startX + spacingX * 2 + 6, 100, macroW, macroH);

        spaceReverbLabel.setBounds(startX + spacingX * 3, 86, macroW + 40, 14);
        spaceReverbSlider.setBounds(startX + spacingX * 3 + 6, 100, macroW, macroH);

        // Row 2 (y: 190)
        echoDelayLabel.setBounds(startX, 186, macroW + 40, 14);
        echoDelaySlider.setBounds(startX + 6, 200, macroW, macroH);

        stereoWideLabel.setBounds(startX + spacingX, 186, macroW + 40, 14);
        stereoWideSlider.setBounds(startX + spacingX + 6, 200, macroW, macroH);

        glossAirLabel.setBounds(startX + spacingX * 2, 186, macroW + 40, 14);
        glossAirSlider.setBounds(startX + spacingX * 2 + 6, 200, macroW, macroH);

        warmthDriveLabel.setBounds(startX + spacingX * 3, 186, macroW + 40, 14);
        warmthDriveSlider.setBounds(startX + spacingX * 3 + 6, 200, macroW, macroH);

        // Bottom Bay: Instant Quick-FX & Performance Pads (x: 16, y: 306, w: 948, h: 258)
        // Row 1: Instant Quick-FX Toggles (y: 340)
        phaserQuickToggle.setBounds(36, 342, 140, 32);
        glitterQuickToggle.setBounds(190, 342, 130, 32);
        trashQuickToggle.setBounds(335, 342, 140, 32);
        monoMakerQuickToggle.setBounds(490, 342, 140, 32);

        savePresetButton.setBounds(650, 344, 55, 28);
        loadPresetButton.setBounds(710, 344, 55, 28);
        mutateButton.setBounds(770, 344, 65, 28);

        // Master Volume & VU Meter (Bottom Right)
        masterVolLabel.setBounds(850, 335, 95, 13);
        masterVolSlider.setBounds(850, 348, 95, 54);

        // Row 2: Momentary DJ Performance Drop Pads (y: 400)
        int padW = 218;
        int padH = 58;
        if (tapeStopPad) tapeStopPad->setBounds(36, 400, padW, padH);
        if (stutterPad)  stutterPad->setBounds(270, 400, padW, padH);
        if (divePad)     divePad->setBounds(504, 400, padW, padH);
        if (reversePad)  reversePad->setBounds(738, 400, padW, padH);

        // Row 3: Drag MIDI Stems & Meter (y: 480)
        dragMidiButton.setBounds(36, 480, 80, 26);
        dragChordButton.setBounds(124, 480, 80, 26);
        dragHookButton.setBounds(212, 480, 80, 26);
        dragVaultButton.setBounds(300, 480, 80, 26);

        vuMeter.setBounds(400, 478, 546, 30);
    }
    else
    {
        // ====================================================
        // ADVANCED SOUND DESIGNER VIEW
        // ====================================================
        // Section 1: Sound Engine (x: 16, y: 56, w: 304, h: 508)
        osc1ShapeLabel.setBounds(24, 86, 85, 13);
        osc1ShapeSlider.setBounds(24, 99, 85, 54);

        unisonDetuneLabel.setBounds(124, 86, 85, 13);
        unisonDetuneSlider.setBounds(124, 99, 85, 54);

        subLevelLabel.setBounds(224, 86, 85, 13);
        subLevelSlider.setBounds(224, 99, 85, 54);

        filterCutoffLabel.setBounds(24, 166, 85, 13);
        filterCutoffSlider.setBounds(24, 179, 85, 54);

        filterResLabel.setBounds(124, 166, 85, 13);
        filterResSlider.setBounds(124, 179, 85, 54);

        layerBMixLabel.setBounds(224, 166, 85, 13);
        layerBMixSlider.setBounds(224, 179, 85, 54);

        osc1OctaveLabel.setBounds(26, 246, 130, 13);
        osc1OctaveBox.setBounds(26, 260, 130, 24);

        filterModeLabel.setBounds(168, 246, 140, 13);
        filterModeBox.setBounds(168, 260, 140, 24);

        layerBTypeLabel.setBounds(26, 298, 282, 13);
        layerBTypeBox.setBounds(26, 312, 282, 24);

        // Interactive FL 6-Stage Envelope
        envDisplay.setBounds(24, 350, 286, 68);

        envDelayLabel.setBounds(24, 426, 45, 12);
        envDelaySlider.setBounds(24, 438, 45, 42);

        ampAttackLabel.setBounds(72, 426, 45, 12);
        ampAttackSlider.setBounds(72, 438, 45, 42);

        envHoldLabel.setBounds(120, 426, 45, 12);
        envHoldSlider.setBounds(120, 438, 45, 42);

        ampDecayLabel.setBounds(168, 426, 45, 12);
        ampDecaySlider.setBounds(168, 438, 45, 42);

        ampSustainLabel.setBounds(216, 426, 45, 12);
        ampSustainSlider.setBounds(216, 438, 45, 42);

        ampReleaseLabel.setBounds(264, 426, 45, 12);
        ampReleaseSlider.setBounds(264, 438, 45, 42);

        envDecTensionLabel.setBounds(24, 488, 135, 12);
        envDecTensionSlider.setBounds(24, 500, 135, 42);

        envRelTensionLabel.setBounds(172, 488, 135, 12);
        envRelTensionSlider.setBounds(172, 500, 135, 42);

        // Section 2: Performance & Echo Suite (x: 330, y: 56, w: 320, h: 508)
        echoFeedLabel.setBounds(338, 86, 58, 12);
        echoFeedSlider.setBounds(338, 98, 58, 44);

        echoTimeLabel.setBounds(400, 86, 58, 12);
        echoTimeSlider.setBounds(400, 98, 58, 44);

        echoPanLabel.setBounds(462, 86, 58, 12);
        echoPanSlider.setBounds(462, 98, 58, 44);

        echoPitchLabel.setBounds(524, 86, 58, 12);
        echoPitchSlider.setBounds(524, 98, 58, 44);

        echoCountLabel.setBounds(586, 86, 58, 12);
        echoCountSlider.setBounds(586, 98, 58, 44);

        echoPingPongToggle.setBounds(338, 150, 145, 22);
        echoFatToggle.setBounds(495, 150, 145, 22);

        glideTimeLabel.setBounds(338, 180, 145, 12);
        glideTimeSlider.setBounds(338, 192, 145, 44);

        chordProgLabel.setBounds(495, 180, 145, 12);
        chordProgBox.setBounds(495, 194, 145, 24);

        harmonizerLabel.setBounds(338, 248, 145, 12);
        harmonizerBox.setBounds(338, 262, 145, 24);

        autoBassLabel.setBounds(495, 248, 145, 12);
        autoBassBox.setBounds(495, 262, 145, 24);

        hookGenToggle.setBounds(338, 302, 145, 22);
        generateHookButton.setBounds(495, 302, 145, 22);

        scaleRootLabel.setBounds(338, 338, 145, 12);
        scaleRootBox.setBounds(338, 352, 145, 24);

        scaleTypeLabel.setBounds(495, 338, 145, 12);
        scaleTypeBox.setBounds(495, 352, 145, 24);

        // Section 3: Effects & 4-Stage Phaser Suite (x: 660, y: 56, w: 304, h: 508)
        // Phaser controls
        phaserRateLabel.setBounds(668, 86, 68, 12);
        phaserRateSlider.setBounds(668, 98, 68, 46);

        phaserDepthLabel.setBounds(742, 86, 68, 12);
        phaserDepthSlider.setBounds(742, 98, 68, 46);

        phaserFeedbackLabel.setBounds(816, 86, 68, 12);
        phaserFeedbackSlider.setBounds(816, 98, 68, 46);

        phaserMixLabel.setBounds(890, 86, 68, 12);
        phaserMixSlider.setBounds(890, 98, 68, 46);

        // Other FX
        fxDriveLabel.setBounds(668, 156, 85, 13);
        fxDriveSlider.setBounds(668, 169, 85, 52);

        fxChorusMixLabel.setBounds(768, 156, 85, 13);
        fxChorusMixSlider.setBounds(768, 169, 85, 52);

        analogDriftLabel.setBounds(868, 156, 85, 13);
        analogDriftSlider.setBounds(868, 169, 85, 52);

        fxDelayTimeLabel.setBounds(668, 230, 85, 13);
        fxDelayTimeSlider.setBounds(668, 243, 85, 52);

        fxReverbDecayLabel.setBounds(768, 230, 85, 13);
        fxReverbDecaySlider.setBounds(768, 243, 85, 52);

        glitterMixLabel.setBounds(868, 230, 85, 13);
        glitterMixSlider.setBounds(868, 243, 85, 52);

        glitterGrainLabel.setBounds(668, 304, 85, 13);
        glitterGrainSlider.setBounds(668, 317, 85, 52);

        pumpToggle.setBounds(768, 312, 185, 22);
        monoMakerToggle.setBounds(768, 338, 185, 22);

        producerFlavorLabel.setBounds(668, 375, 150, 13);
        producerFlavorBox.setBounds(668, 389, 150, 24);

        producerFlavorIntensityLabel.setBounds(830, 375, 125, 13);
        producerFlavorIntensitySlider.setBounds(830, 389, 125, 48);

        masterVolLabel.setBounds(668, 450, 85, 13);
        masterVolSlider.setBounds(668, 464, 85, 54);

        vuMeter.setBounds(765, 464, 190, 48);
    }
}
