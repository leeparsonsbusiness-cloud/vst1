#include "PluginProcessor.h"
#include "PluginEditor.h"

// ==============================================================================
// ModernSynthLookAndFeel (Supports 4 Distinct Luxury & Retro Themes)
// ==============================================================================
ModernSynthLookAndFeel::ModernSynthLookAndFeel()
{
    setTheme(0);
}

void ModernSynthLookAndFeel::setTheme(int themeIndex)
{
    currentTheme = juce::jlimit(0, 3, themeIndex);

    if (currentTheme == CyberNeon)
    {
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff090a10));
        setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff12131f));
        setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff302b4d));
        setColour(juce::ComboBox::focusedOutlineColourId, juce::Colour(0xffd400ff));
        setColour(juce::ComboBox::textColourId, juce::Colour(0xfff0e6ff));
        setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff0f1018));
        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xffd400ff).withAlpha(0.3f));
        setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
        setColour(juce::PopupMenu::textColourId, juce::Colour(0xffdfd0f8));
    }
    else if (currentTheme == VintageCream)
    {
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xffeae7dc));
        setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xfff8f7f2));
        setColour(juce::ComboBox::outlineColourId, juce::Colour(0xffd8c3a5));
        setColour(juce::ComboBox::focusedOutlineColourId, juce::Colour(0xffe85a4f));
        setColour(juce::ComboBox::textColourId, juce::Colour(0xff2b2a29));
        setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xfffaf8f5));
        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xffe85a4f).withAlpha(0.2f));
        setColour(juce::PopupMenu::highlightedTextColourId, juce::Colour(0xff22201e));
        setColour(juce::PopupMenu::textColourId, juce::Colour(0xff4a4846));
    }
    else if (currentTheme == StealthBlackout)
    {
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff0a0a0b));
        setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff141416));
        setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff26262a));
        setColour(juce::ComboBox::focusedOutlineColourId, juce::Colour(0xff888892));
        setColour(juce::ComboBox::textColourId, juce::Colour(0xffcccccc));
        setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff121214));
        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xff333338));
        setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
        setColour(juce::PopupMenu::textColourId, juce::Colour(0xffaaaaaa));
    }
    else // Analog Lab Luxury
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
}

juce::Colour ModernSynthLookAndFeel::getBgColour() const
{
    if (currentTheme == CyberNeon) return juce::Colour(0xff090a10);
    if (currentTheme == VintageCream) return juce::Colour(0xffeae7dc);
    if (currentTheme == StealthBlackout) return juce::Colour(0xff0a0a0b);
    return juce::Colour(0xff111216);
}

juce::Colour ModernSynthLookAndFeel::getCardBgColour() const
{
    if (currentTheme == CyberNeon) return juce::Colour(0xff10111a);
    if (currentTheme == VintageCream) return juce::Colour(0xfff5f3ec);
    if (currentTheme == StealthBlackout) return juce::Colour(0xff131316);
    return juce::Colour(0xff161820);
}

juce::Colour ModernSynthLookAndFeel::getCardBorderColour() const
{
    if (currentTheme == CyberNeon) return juce::Colour(0xff261f3d);
    if (currentTheme == VintageCream) return juce::Colour(0xffd5cfbe);
    if (currentTheme == StealthBlackout) return juce::Colour(0xff222226);
    return juce::Colour(0xff242734);
}

juce::Colour ModernSynthLookAndFeel::getAccentColour(int bayIndex) const
{
    if (currentTheme == CyberNeon)
    {
        if (bayIndex == 0) return juce::Colour(0xff00f0ff);
        if (bayIndex == 1) return juce::Colour(0xffd400ff);
        return juce::Colour(0xffff0055);
    }
    if (currentTheme == VintageCream)
    {
        if (bayIndex == 0) return juce::Colour(0xffe85a4f);
        if (bayIndex == 1) return juce::Colour(0xffe98074);
        return juce::Colour(0xffd8c3a5);
    }
    if (currentTheme == StealthBlackout)
    {
        return juce::Colour(0xffaaaaaa);
    }
    if (bayIndex == 0) return juce::Colour(0xff00d4ff);
    if (bayIndex == 1) return juce::Colour(0xffffaa00);
    return juce::Colour(0xffff5500);
}

void ModernSynthLookAndFeel::drawSidePanels(juce::Graphics& g, int width, int height) const
{
    float panelWidth = 10.0f;
    if (currentTheme == VintageCream)
    {
        auto drawTeak = [&](float x) {
            juce::ColourGradient grad(juce::Colour(0xffb87d4b), x, 0.0f,
                                      juce::Colour(0xff6e4726), x + panelWidth, 0.0f, false);
            g.setGradientFill(grad);
            g.fillRect(x, 0.0f, panelWidth, (float) height);
        };
        drawTeak(0.0f);
        drawTeak((float) (width - 10));
    }
    else if (currentTheme == CyberNeon)
    {
        auto drawCyber = [&](float x) {
            juce::ColourGradient grad(juce::Colour(0xff1d0a30), x, 0.0f,
                                      juce::Colour(0xff080210), x + panelWidth, 0.0f, false);
            g.setGradientFill(grad);
            g.fillRect(x, 0.0f, panelWidth, (float) height);
            g.setColour(juce::Colour(0xffd400ff).withAlpha(0.6f));
            g.drawVerticalLine(static_cast<int>(x > 5 ? x : x + panelWidth - 1), 0.0f, (float) height);
        };
        drawCyber(0.0f);
        drawCyber((float) (width - 10));
    }
    else if (currentTheme == StealthBlackout)
    {
        g.setColour(juce::Colour(0xff16161a));
        g.fillRect(0.0f, 0.0f, panelWidth, (float) height);
        g.fillRect((float) (width - 10), 0.0f, panelWidth, (float) height);
    }
    else
    {
        auto drawWood = [&](float x) {
            juce::ColourGradient woodGrad(juce::Colour(0xff452314), x, 0.0f,
                                          juce::Colour(0xff200f07), x + panelWidth, 0.0f, false);
            g.setGradientFill(woodGrad);
            g.fillRect(x, 0.0f, panelWidth, (float) height);

            g.setColour(juce::Colour(0xff5c301c).withAlpha(0.35f));
            for (float ly = 12.0f; ly < height; ly += 24.0f)
                g.drawHorizontalLine(static_cast<int>(ly), x + 1.0f, x + panelWidth - 1.0f);
        };
        drawWood(0.0f);
        drawWood((float) (width - 10));
    }
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

    // 1. Drop Shadow
    g.setColour(juce::Colour(0x55000000));
    g.fillEllipse(rx + 1.0f, ry + 2.5f, rw, rw);

    // 2. Track Arc
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(currentTheme == VintageCream ? juce::Colour(0xffdcd6c8) : juce::Colour(0xff1e202a));
    g.strokePath(backgroundArc, juce::PathStrokeType(3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // 3. Illuminated LED Value Arc
    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
        
        juce::Colour col1 = (currentTheme == CyberNeon) ? juce::Colour(0xff00f0ff) :
                            (currentTheme == VintageCream) ? juce::Colour(0xffe85a4f) :
                            (currentTheme == StealthBlackout) ? juce::Colour(0xffffffff) : juce::Colour(0xff00d4ff);
        juce::Colour col2 = (currentTheme == CyberNeon) ? juce::Colour(0xffd400ff) :
                            (currentTheme == VintageCream) ? juce::Colour(0xffe98074) :
                            (currentTheme == StealthBlackout) ? juce::Colour(0xff888888) : juce::Colour(0xffffaa00);

        juce::ColourGradient grad(col1, centreX - radius, centreY, col2, centreX + radius, centreY, false);
        g.setGradientFill(grad);
        g.strokePath(valueArc, juce::PathStrokeType(3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // 4. Dial Cap
    auto dialRadius = radius - 4.5f;
    auto drx = centreX - dialRadius;
    auto dry = centreY - dialRadius;
    auto drw = dialRadius * 2.0f;

    juce::Colour cap1 = (currentTheme == VintageCream) ? juce::Colour(0xfff5f3ec) : juce::Colour(0xff252834);
    juce::Colour cap2 = (currentTheme == VintageCream) ? juce::Colour(0xffdfd9cb) : juce::Colour(0xff14151b);

    juce::ColourGradient faceGrad(cap1, centreX - dialRadius, centreY - dialRadius,
                                  cap2, centreX + dialRadius, centreY + dialRadius, false);
    g.setGradientFill(faceGrad);
    g.fillEllipse(drx, dry, drw, drw);

    g.setColour((currentTheme == VintageCream) ? juce::Colour(0xffc5bead) : juce::Colour(0xff3c4052).withAlpha(0.7f));
    g.drawEllipse(drx, dry, drw, drw, 1.0f);

    // 5. Pointer Line
    juce::Path p;
    auto pointerLength = dialRadius * 0.70f;
    auto pointerThickness = 2.2f;
    p.addRoundedRectangle(-pointerThickness * 0.5f, -dialRadius + 2.0f, pointerThickness, pointerLength, 1.0f);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    
    g.setColour((currentTheme == VintageCream) ? juce::Colour(0xffe85a4f) : juce::Colour(0xffffffff));
    g.fillPath(p);

    // Center Pip
    g.setColour((currentTheme == VintageCream) ? juce::Colour(0xff22201e) : juce::Colour(0xff00d4ff));
    g.fillEllipse(centreX - 1.2f, centreY - 1.2f, 2.4f, 2.4f);
}

void ModernSynthLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool /*isButtonDown*/,
                                          int buttonX, int buttonY, int buttonW, int buttonH,
                                          juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<float>(0.0f, 0.0f, (float) width, (float) height);
    
    g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(box.hasKeyboardFocus(true) ? box.findColour(juce::ComboBox::focusedOutlineColourId) : box.findColour(juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    // Chevron Arrow
    juce::Path path;
    float ax = (float) buttonX + (float) buttonW * 0.5f;
    float ay = (float) buttonY + (float) buttonH * 0.5f;
    path.startNewSubPath(ax - 3.0f, ay - 1.5f);
    path.lineTo(ax, ay + 2.0f);
    path.lineTo(ax + 3.0f, ay - 1.5f);

    g.setColour(currentTheme == VintageCream ? juce::Colour(0xff6e675f) : juce::Colour(0xff8c92a6));
    g.strokePath(path, juce::PathStrokeType(1.4f));
}

void ModernSynthLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                              bool shouldDrawButtonAsHighlighted, bool /*shouldDrawButtonAsDown*/)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.5f);
    bool isOn = button.getToggleState();

    juce::Colour activeBg = (currentTheme == VintageCream) ? juce::Colour(0xfffceeed) : juce::Colour(0xff222735);
    juce::Colour inactiveBg = (currentTheme == VintageCream) ? juce::Colour(0xffedeae1) : (shouldDrawButtonAsHighlighted ? juce::Colour(0xff1c1e27) : juce::Colour(0xff161820));

    g.setColour(isOn ? activeBg : inactiveBg);
    g.fillRoundedRectangle(bounds, 4.0f);

    juce::Colour activeBorder = (currentTheme == VintageCream) ? juce::Colour(0xffe85a4f) : (currentTheme == CyberNeon ? juce::Colour(0xffd400ff) : juce::Colour(0xff00d4ff));
    g.setColour(isOn ? activeBorder : (currentTheme == VintageCream ? juce::Colour(0xffd5cfbe) : juce::Colour(0xff2b2f3e)));
    g.drawRoundedRectangle(bounds, 4.0f, isOn ? 1.4f : 1.0f);

    // Glowing LED pip
    float ledX = bounds.getX() + 9.0f;
    float ledY = bounds.getCentreY();
    
    if (isOn)
    {
        g.setColour(activeBorder.withAlpha(0.35f));
        g.fillEllipse(ledX - 4.5f, ledY - 4.5f, 9.0f, 9.0f);
        g.setColour(activeBorder);
        g.fillEllipse(ledX - 2.5f, ledY - 2.5f, 5.0f, 5.0f);
    }
    else
    {
        g.setColour(currentTheme == VintageCream ? juce::Colour(0xffb5ad9b) : juce::Colour(0xff303444));
        g.fillEllipse(ledX - 2.5f, ledY - 2.5f, 5.0f, 5.0f);
    }

    g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    g.setColour(isOn ? (currentTheme == VintageCream ? juce::Colour(0xff22201e) : juce::Colour(0xffffffff)) 
                     : (currentTheme == VintageCream ? juce::Colour(0xff6e675f) : juce::Colour(0xff8a90a4)));
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

    juce::Colour borderCol = (currentTheme == VintageCream) ? juce::Colour(0xffd5cfbe) : juce::Colour(0xff303444);
    g.setColour(shouldDrawButtonAsHighlighted ? (currentTheme == VintageCream ? juce::Colour(0xffe85a4f) : juce::Colour(0xff00d4ff)) : borderCol);
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
}

// ==============================================================================
// Plugin Editor Implementation
// ==============================================================================
KeshaZeddSynthAudioProcessorEditor::KeshaZeddSynthAudioProcessorEditor(KeshaZeddSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), dragMidiButton(p, false), dragChordButton(p, true), vuMeter(p)
{
    setLookAndFeel(&lookAndFeel);
    setSize(860, 560);

    // 1. Preset Dropdown & Header Buttons
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

    // Zeddify 2.0 Toggles & Style Selector
    zeddifyButton.setButtonText("ZEDDIFY");
    addAndMakeVisible(zeddifyButton);
    zeddifyAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "zeddify_active", zeddifyButton);

    setupComboBox(zeddifyStyleBox, presetLabel, "");
    zeddifyStyleBox.addItemList({"Zedd 16th", "Avicii Anthem", "Eurodance Riff", "Synthwave Roll", 
                                 "Tropical Strum", "Nu-Disco Stabs", "Future Rave", "Slap House", 
                                 "Hyperpop Glitch", "Trap Half-Time", "Acid 303 Roll", "Stadium Stabs"}, 1);
    zeddifyStyleAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "zeddify_style", zeddifyStyleBox);

    mutateButton.setButtonText("MUTATE");
    mutateButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff332244));
    addAndMakeVisible(mutateButton);
    mutateButton.onClick = [this]() {
        audioProcessor.getZeddifyEngine().mutateCurrentPattern();
    };

    autoMasterButton.setButtonText("AUTO-MASTER");
    addAndMakeVisible(autoMasterButton);
    autoMasterAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "auto_master_active", autoMasterButton);

    setupComboBox(themeBox, presetLabel, "");
    themeBox.addItemList({"Analog Lab", "Cyber Neon", "Vintage Cream", "Stealth"}, 1);
    themeBox.onChange = [this]() {
        lookAndFeel.setTheme(themeBox.getSelectedItemIndex());
        repaint();
    };
    themeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "ui_theme", themeBox);

    addAndMakeVisible(dragMidiButton);
    addAndMakeVisible(dragChordButton);
    addAndMakeVisible(vuMeter);
    addAndMakeVisible(audioProcessor.getVisualizer());

    setupSlider(masterVolSlider, masterVolLabel, "VOLUME");
    masterVolAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "master_vol", masterVolSlider);

    voiceCountLabel.setText("8 VOICES", juce::dontSendNotification);
    voiceCountLabel.setFont(juce::FontOptions(9.5f, juce::Font::bold));
    voiceCountLabel.setColour(juce::Label::textColourId, juce::Colour(0xff757a8e));
    addAndMakeVisible(voiceCountLabel);

    // ----------------------------------------------------
    // SECTION 1: SOUND ENGINE & HYBRID LAYER (Left Bay)
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

    setupSlider(layerBMixSlider, layerBMixLabel, "LAYER B MIX");
    layerBMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "layer_b_mix", layerBMixSlider);

    setupComboBox(osc1OctaveBox, osc1OctaveLabel, "OCTAVE");
    osc1OctaveBox.addItemList({"-2 Oct", "-1 Oct", "0 Oct", "+1 Oct", "+2 Oct"}, 1);
    osc1OctaveAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "osc1_octave", osc1OctaveBox);

    setupComboBox(filterModeBox, filterModeLabel, "FILTER TYPE");
    filterModeBox.addItemList({"LPF 12dB", "LPF 24dB", "BPF 12dB", "HPF 12dB", "Notch", "Formant"}, 1);
    filterModeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "filter_mode", filterModeBox);

    setupComboBox(layerBTypeBox, layerBTypeLabel, "LAYER B SOUND");
    layerBTypeBox.addItemList({"Layer B Off", "Grand Piano Strike", "Glass FM Bell", "Vocal Formant Chop", "White Noise Layer", "Vinyl Dust"}, 1);
    layerBTypeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "layer_b_type", layerBTypeBox);

    // ----------------------------------------------------
    // SECTION 2: SONGWRITING, FLAVOR & SLIDE (Center Bay)
    // ----------------------------------------------------
    slideToggle.setButtonText("SLIDE / GLIDE");
    addAndMakeVisible(slideToggle);
    slideToggle.onClick = [this]() {
        float currentMode = audioProcessor.getAPVTS().getRawParameterValue("play_mode")->load();
        float newMode = (currentMode == 0.0f) ? 1.0f : 0.0f;
        if (auto* param = audioProcessor.getAPVTS().getParameter("play_mode"))
            param->setValueNotifyingHost(param->getNormalisableRange().convertTo0to1(newMode));
    };

    setupSlider(glideTimeSlider, glideTimeLabel, "GLIDE TIME");
    glideTimeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "glide_time", glideTimeSlider);

    // Songwriting Dropdowns
    setupComboBox(chordProgBox, chordProgLabel, "CHORD SETS");
    chordProgBox.addItemList({"Chords Off", "Pop Axis (I-V-vi-IV)", "Dark EDM (i-VI-III-VII)", 
                              "Emotional (vi-IV-I-V)", "Future Bass (IV-I-vi-V)", "80s Synthwave", 
                              "50s Doo-Wop", "Royal Road", "Kesha Party Dance"}, 1);
    chordProgAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "chord_prog_preset", chordProgBox);

    setupComboBox(harmonizerBox, harmonizerLabel, "HARMONIZE");
    harmonizerBox.addItemList({"Harmonizer Off", "Diatonic 3rds", "Stadium 5ths", "Octave Spread", "Pop Vocal Stack"}, 1);
    harmonizerAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "harmonizer_mode", harmonizerBox);

    setupComboBox(autoBassBox, autoBassLabel, "AUTO-BASS");
    autoBassBox.addItemList({"Auto-Bass Off", "Rolling 16ths", "Offbeat 8ths", "4-on-the-Floor", "Nu-Disco Sync"}, 1);
    autoBassAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "auto_bass_mode", autoBassBox);

    setupComboBox(producerFlavorBox, producerFlavorLabel, "PRODUCER FLAVOR");
    producerFlavorBox.addItemList({"Neutral", "Zedd Crunch", "Avicii Warmth", "Max Martin Polish", "Kesha Glitter"}, 1);
    producerFlavorAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "producer_flavor", producerFlavorBox);

    setupSlider(producerFlavorIntensitySlider, producerFlavorIntensityLabel, "FLAVOR AMT");
    producerFlavorIntensityAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "producer_flavor_intensity", producerFlavorIntensitySlider);

    riserToggle.setButtonText("THE RISER / TENSION");
    addAndMakeVisible(riserToggle);
    riserAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "riser_active", riserToggle);

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

    setupComboBox(scaleRootBox, scaleRootLabel, "ROOT");
    scaleRootBox.addItemList({"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"}, 1);
    scaleRootAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "scale_root", scaleRootBox);

    setupComboBox(scaleTypeBox, scaleTypeLabel, "SCALE LOCK");
    scaleTypeBox.addItemList({"Scale Off", "Major", "Natural Minor", "Harmonic Minor", "Dorian", "Mixolydian", "Pentatonic"}, 1);
    scaleTypeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "scale_type", scaleTypeBox);

    // ----------------------------------------------------
    // SECTION 3: EFFECTS, SPACE & ANALOG DRIFT (Right Bay)
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

    setupSlider(analogDriftSlider, analogDriftLabel, "VCO DRIFT");
    analogDriftAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "analog_drift", analogDriftSlider);

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

    float mode = audioProcessor.getAPVTS().getRawParameterValue("play_mode")->load();
    slideToggle.setToggleState(mode > 0.5f, juce::dontSendNotification);

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
    label.setFont(juce::FontOptions(9.0f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, (lookAndFeel.getTheme() == ModernSynthLookAndFeel::VintageCream) ? juce::Colour(0xff55524c) : juce::Colour(0xff8c92a6));
    addAndMakeVisible(label);
}

void KeshaZeddSynthAudioProcessorEditor::setupComboBox(juce::ComboBox& box, juce::Label& label, const juce::String& text)
{
    addAndMakeVisible(box);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::FontOptions(8.5f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centredLeft);
    label.setColour(juce::Label::textColourId, (lookAndFeel.getTheme() == ModernSynthLookAndFeel::VintageCream) ? juce::Colour(0xff55524c) : juce::Colour(0xff8c92a6));
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
    g.fillAll(lookAndFeel.getBgColour());

    // 2. Themed Side Panels
    lookAndFeel.drawSidePanels(g, w, h);

    // 3. Top Header Bar
    juce::ColourGradient headerGrad(lookAndFeel.getCardBgColour().brighter(0.08f), 0.0f, 0.0f,
                                    lookAndFeel.getCardBgColour().darker(0.12f), 0.0f, 46.0f, false);
    g.setGradientFill(headerGrad);
    g.fillRect(10, 0, w - 20, 46);

    g.setColour(lookAndFeel.getCardBorderColour());
    g.drawHorizontalLine(46, 10.0f, (float) (w - 10));

    // Logo Typography
    g.setFont(juce::FontOptions(17.0f, juce::Font::bold));
    g.setColour((lookAndFeel.getTheme() == ModernSynthLookAndFeel::VintageCream) ? juce::Colour(0xff22201e) : juce::Colour(0xffffffff));
    g.drawText("KZ-SYNTH", 22, 6, 95, 20, juce::Justification::centredLeft);

    g.setFont(juce::FontOptions(8.0f, juce::Font::bold));
    g.setColour(lookAndFeel.getAccentColour(1));
    g.drawText("HITMAKER EDITION", 22, 24, 95, 12, juce::Justification::centredLeft);

    // 4. Central OLED Screen Bezel Frame
    g.setColour(juce::Colour(0xff090a0e));
    g.fillRoundedRectangle(16.0f, 52.0f, (float) (w - 32), 60.0f, 5.0f);

    g.setColour(lookAndFeel.getCardBorderColour());
    g.drawRoundedRectangle(16.0f, 52.0f, (float) (w - 32), 60.0f, 5.0f, 1.0f);

    // 5. Modular Section Cards
    auto drawSection = [&](int sx, int sy, int sw, int sh, const juce::String& title, const juce::Colour& accent) {
        g.setColour(lookAndFeel.getCardBgColour());
        g.fillRoundedRectangle((float) sx, (float) sy, (float) sw, (float) sh, 5.0f);
        
        g.setColour(lookAndFeel.getCardBorderColour());
        g.drawRoundedRectangle((float) sx, (float) sy, (float) sw, (float) sh, 5.0f, 1.0f);
        
        g.setColour(accent);
        g.fillRoundedRectangle((float) (sx + 12), (float) (sy + 10), 3.0f, 10.0f, 1.5f);

        g.setFont(juce::FontOptions(9.5f, juce::Font::bold));
        g.setColour((lookAndFeel.getTheme() == ModernSynthLookAndFeel::VintageCream) ? juce::Colour(0xff22201e) : juce::Colour(0xffd0d4e4));
        g.drawText(title.toUpperCase(), sx + 20, sy + 6, sw - 30, 18, juce::Justification::centredLeft);
    };

    drawSection(16, 118, 268, 430, "Sound Engine & Hybrid", lookAndFeel.getAccentColour(0));
    drawSection(292, 118, 276, 430, "Songwriting & Performance", lookAndFeel.getAccentColour(1));
    drawSection(576, 118, 268, 430, "Effects & Space", lookAndFeel.getAccentColour(2));
}

void KeshaZeddSynthAudioProcessorEditor::resized()
{
    // ----------------------------------------------------
    // Header Bar Layout (y: 0 to 46)
    // ----------------------------------------------------
    prevPresetButton.setBounds(116, 11, 18, 24);
    presetBox.setBounds(136, 11, 165, 24);
    nextPresetButton.setBounds(303, 11, 18, 24);

    savePresetButton.setBounds(324, 11, 36, 24);
    loadPresetButton.setBounds(362, 11, 36, 24);
    diceButton.setBounds(400, 11, 36, 24);

    zeddifyButton.setBounds(440, 11, 58, 24);
    zeddifyStyleBox.setBounds(500, 11, 74, 24);
    mutateButton.setBounds(576, 11, 44, 24);

    autoMasterButton.setBounds(624, 11, 72, 24);
    themeBox.setBounds(698, 11, 62, 24);
    dragMidiButton.setBounds(762, 11, 56, 24);
    dragChordButton.setBounds(820, 11, 58, 24);

    // ----------------------------------------------------
    // Central OLED Screen (y: 54 to 110)
    // ----------------------------------------------------
    audioProcessor.getVisualizer().setBounds(18, 54, getWidth() - 36, 56);

    // ----------------------------------------------------
    // Section 1: Sound Engine & Hybrid Layer (x: 16, y: 118, w: 268, h: 430)
    // ----------------------------------------------------
    // Row 1 (y: 145)
    osc1ShapeLabel.setBounds(24, 142, 74, 13);
    osc1ShapeSlider.setBounds(24, 155, 74, 54);

    unisonDetuneLabel.setBounds(112, 142, 74, 13);
    unisonDetuneSlider.setBounds(112, 155, 74, 54);

    subLevelLabel.setBounds(200, 142, 74, 13);
    subLevelSlider.setBounds(200, 155, 74, 54);

    // Row 2 (y: 225)
    filterCutoffLabel.setBounds(24, 222, 74, 13);
    filterCutoffSlider.setBounds(24, 235, 74, 54);

    filterResLabel.setBounds(112, 222, 74, 13);
    filterResSlider.setBounds(112, 235, 74, 54);

    layerBMixLabel.setBounds(200, 222, 74, 13);
    layerBMixSlider.setBounds(200, 235, 74, 54);

    // Row 3 Dropdowns (y: 310)
    osc1OctaveLabel.setBounds(26, 308, 110, 13);
    osc1OctaveBox.setBounds(26, 322, 110, 22);

    filterModeLabel.setBounds(150, 308, 120, 13);
    filterModeBox.setBounds(150, 322, 120, 22);

    layerBTypeLabel.setBounds(26, 360, 244, 13);
    layerBTypeBox.setBounds(26, 375, 244, 22);

    // ----------------------------------------------------
    // Section 2: Songwriting & Performance (x: 292, y: 118, w: 276, h: 430)
    // ----------------------------------------------------
    // Row 1: Songwriting Dropdowns (y: 135)
    chordProgLabel.setBounds(300, 134, 130, 13);
    chordProgBox.setBounds(300, 147, 130, 22);

    harmonizerLabel.setBounds(435, 134, 125, 13);
    harmonizerBox.setBounds(435, 147, 125, 22);

    // Row 2: Auto-Bass & Slide (y: 175)
    autoBassLabel.setBounds(300, 175, 130, 13);
    autoBassBox.setBounds(300, 188, 130, 22);

    slideToggle.setBounds(435, 186, 125, 24);

    // Row 3: Producer Flavor & Intensity (y: 218)
    producerFlavorLabel.setBounds(300, 218, 130, 13);
    producerFlavorBox.setBounds(300, 231, 130, 22);

    producerFlavorIntensityLabel.setBounds(435, 218, 60, 13);
    producerFlavorIntensitySlider.setBounds(435, 230, 60, 48);

    glideTimeLabel.setBounds(500, 218, 60, 13);
    glideTimeSlider.setBounds(500, 230, 60, 48);

    // Envelopes (y: 285)
    ampAttackLabel.setBounds(300, 282, 58, 13);
    ampAttackSlider.setBounds(300, 294, 58, 46);

    ampDecayLabel.setBounds(362, 282, 58, 13);
    ampDecaySlider.setBounds(362, 294, 58, 46);

    ampSustainLabel.setBounds(424, 282, 58, 13);
    ampSustainSlider.setBounds(424, 294, 58, 46);

    ampReleaseLabel.setBounds(486, 282, 58, 13);
    ampReleaseSlider.setBounds(486, 294, 58, 46);

    // Macros & Scale Lock (y: 350)
    macroDropLabel.setBounds(300, 348, 65, 13);
    macroDropSlider.setBounds(300, 360, 65, 52);

    punchLabel.setBounds(370, 348, 65, 13);
    punchSlider.setBounds(370, 360, 65, 52);

    scaleRootLabel.setBounds(445, 348, 45, 13);
    scaleRootBox.setBounds(445, 362, 45, 22);

    scaleTypeLabel.setBounds(495, 348, 65, 13);
    scaleTypeBox.setBounds(495, 362, 65, 22);

    riserToggle.setBounds(300, 418, 255, 22);

    // ----------------------------------------------------
    // Section 3: Effects & Space (x: 576, y: 118, w: 268, h: 430)
    // ----------------------------------------------------
    // Row 1 (y: 145)
    fxDriveLabel.setBounds(584, 142, 74, 13);
    fxDriveSlider.setBounds(584, 155, 74, 54);

    fxChorusMixLabel.setBounds(672, 142, 74, 13);
    fxChorusMixSlider.setBounds(672, 155, 74, 54);

    fxDelayTimeLabel.setBounds(760, 142, 74, 13);
    fxDelayTimeSlider.setBounds(760, 155, 74, 54);

    // Row 2 (y: 225)
    fxDelayMixLabel.setBounds(584, 222, 74, 13);
    fxDelayMixSlider.setBounds(584, 235, 74, 54);

    fxReverbDecayLabel.setBounds(672, 222, 74, 13);
    fxReverbDecaySlider.setBounds(672, 235, 74, 54);

    fxReverbMixLabel.setBounds(760, 222, 74, 13);
    fxReverbMixSlider.setBounds(760, 235, 74, 54);

    // Row 3 (y: 310)
    analogDriftLabel.setBounds(584, 308, 74, 13);
    analogDriftSlider.setBounds(584, 322, 74, 54);

    pumpToggle.setBounds(670, 318, 115, 22);
    monoMakerToggle.setBounds(670, 346, 115, 22);
}
