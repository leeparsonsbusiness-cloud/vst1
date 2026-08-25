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
    : AudioProcessorEditor(&p), audioProcessor(p),
      dragMidiButton(p, DragMidiButton::Zeddify),
      dragChordButton(p, DragMidiButton::Chords),
      dragHookButton(p, DragMidiButton::Hook),
      dragVaultButton(p, DragMidiButton::Vault),
      vuMeter(p),
      envDisplay(p)
{
    setLookAndFeel(&lookAndFeel);
    setSize(980, 560);

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
    savePresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1e2820));
    addAndMakeVisible(savePresetButton);
    savePresetButton.onClick = [this]() {
        showSavePresetDialog();
    };

    loadPresetButton.setButtonText("LOAD");
    loadPresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1e2230));
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

    // Producer Flavor Suite
    setupComboBox(producerFlavorBox, producerFlavorLabel, "PRODUCER FLAVOR");
    producerFlavorBox.addItemList({"Neutral Clean", "Kesha Glitter", "Max Martin Polish", "Zedd Complextro", "Hyperpop Trash"}, 1);
    producerFlavorAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "producer_flavor", producerFlavorBox);

    setupSlider(producerFlavorIntensitySlider, producerFlavorIntensityLabel, "FLAVOR MIX");
    producerFlavorIntensityAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "producer_flavor_intensity", producerFlavorIntensitySlider);

    addAndMakeVisible(dragMidiButton);
    addAndMakeVisible(dragChordButton);
    addAndMakeVisible(dragHookButton);
    addAndMakeVisible(dragVaultButton);
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
    // SECTION 2: FL STUDIO AHDSR ENVELOPE & ECHO/FAT MODE (Center Bay)
    // ----------------------------------------------------
    addAndMakeVisible(envDisplay);

    setupSlider(envDelaySlider, envDelayLabel, "DELAY");
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

    // FL Studio Echo Delay & Fat Mode
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

    // FL Studio Time Shift, Gate, Cut Self & Slide
    setupSlider(timeShiftSlider, timeShiftLabel, "SHIFT");
    timeShiftAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fl_time_shift", timeShiftSlider);

    cutSelfToggle.setButtonText("CUT SELF");
    addAndMakeVisible(cutSelfToggle);
    cutSelfAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "fl_cut_self", cutSelfToggle);

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

    // Hook Generator & MIDI Vault
    hookGenToggle.setButtonText("HOOK GEN");
    addAndMakeVisible(hookGenToggle);
    hookGenAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "hook_generator_active", hookGenToggle);

    setupComboBox(hookMoodBox, presetLabel, "");
    hookMoodBox.addItemList({"Radio Catchy", "High-Energy Drop", "Emotional Ballad", "Syncopated Pluck", "Vocal Topline"}, 1);
    hookMoodAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "hook_mood", hookMoodBox);

    generateHookButton.setButtonText("GEN HOOK");
    generateHookButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2b3a4a));
    addAndMakeVisible(generateHookButton);
    generateHookButton.onClick = [this]() {
        int mood = hookMoodBox.getSelectedItemIndex();
        int root = static_cast<int>(audioProcessor.getAPVTS().getRawParameterValue("scale_root")->load());
        int scale = static_cast<int>(audioProcessor.getAPVTS().getRawParameterValue("scale_type")->load());
        audioProcessor.getHookEngine().generateNewHook(mood, root, scale);
    };

    easyKeyToggle.setButtonText("EASY KEY");
    addAndMakeVisible(easyKeyToggle);
    easyKeyAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "easy_key_active", easyKeyToggle);

    counterMelodyToggle.setButtonText("ANSWER FILL");
    addAndMakeVisible(counterMelodyToggle);
    counterMelodyAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "counter_melody_active", counterMelodyToggle);

    setupComboBox(midiVaultBox, midiVaultLabel, "MIDI VAULT");
    const auto& vaultItems = audioProcessor.getMidiVault().getItems();
    for (size_t i = 0; i < vaultItems.size(); ++i)
        midiVaultBox.addItem(vaultItems[i].name, static_cast<int>(i + 1));
    midiVaultBox.onChange = [this]() {
        dragVaultButton.setVaultIndex(midiVaultBox.getSelectedItemIndex());
    };
    midiVaultBox.setSelectedItemIndex(0);

    // 4 Momentary Glitch Trigger Pads
    tapeStopPad = std::make_unique<MomentaryPadButton>("TAPE STOP", juce::Colour(0xffff3366), [this](bool down) {
        if (auto* param = audioProcessor.getAPVTS().getParameter("glitch_mode"))
            param->setValueNotifyingHost(down ? 0.25f : 0.0f);
    });
    addAndMakeVisible(*tapeStopPad);

    stutterPad = std::make_unique<MomentaryPadButton>("STUTTER", juce::Colour(0xff00d4ff), [this](bool down) {
        if (auto* param = audioProcessor.getAPVTS().getParameter("glitch_mode"))
            param->setValueNotifyingHost(down ? 0.50f : 0.0f);
    });
    addAndMakeVisible(*stutterPad);

    divePad = std::make_unique<MomentaryPadButton>("DIVE", juce::Colour(0xffffaa00), [this](bool down) {
        if (auto* param = audioProcessor.getAPVTS().getParameter("glitch_mode"))
            param->setValueNotifyingHost(down ? 0.75f : 0.0f);
    });
    addAndMakeVisible(*divePad);

    reversePad = std::make_unique<MomentaryPadButton>("REVERSE", juce::Colour(0xffd400ff), [this](bool down) {
        if (auto* param = audioProcessor.getAPVTS().getParameter("glitch_mode"))
            param->setValueNotifyingHost(down ? 1.00f : 0.0f);
    });
    addAndMakeVisible(*reversePad);

    setupSlider(macroDropSlider, macroDropLabel, "THE DROP");
    macroDropAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_drop", macroDropSlider);

    setupSlider(punchSlider, punchLabel, "PUNCH");
    punchAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "macro_punch", punchSlider);

    setupSlider(humanizeSlider, humanizeLabel, "HUMANIZE");
    humanizeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "humanize_amount", humanizeSlider);

    setupComboBox(scaleRootBox, scaleRootLabel, "ROOT");
    scaleRootBox.addItemList({"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"}, 1);
    scaleRootAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "scale_root", scaleRootBox);

    setupComboBox(scaleTypeBox, scaleTypeLabel, "SCALE LOCK");
    scaleTypeBox.addItemList({"Scale Off", "Major", "Natural Minor", "Harmonic Minor", "Dorian", "Mixolydian", "Pentatonic"}, 1);
    scaleTypeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "scale_type", scaleTypeBox);

    // ----------------------------------------------------
    // SECTION 3: EFFECTS, SPACE & GLITTER CLOUD (Right Bay)
    // ----------------------------------------------------
    setupSlider(fxDriveSlider, fxDriveLabel, "GRIT");
    fxDriveAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_drive", fxDriveSlider);

    setupSlider(fxChorusMixSlider, fxChorusMixLabel, "CHORUS");
    fxChorusMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_chorus_mix", fxChorusMixSlider);

    analogDriftLabel.setText("VCO DRIFT", juce::dontSendNotification);
    setupSlider(analogDriftSlider, analogDriftLabel, "VCO DRIFT");
    analogDriftAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "analog_drift", analogDriftSlider);

    setupSlider(fxDelayTimeSlider, fxDelayTimeLabel, "DELAY TIME");
    fxDelayTimeAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_delay_time", fxDelayTimeSlider);

    setupSlider(fxDelayMixSlider, fxDelayMixLabel, "DELAY MIX");
    fxDelayMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_delay_mix", fxDelayMixSlider);

    setupSlider(fxReverbDecaySlider, fxReverbDecayLabel, "REVERB SIZE");
    fxReverbDecayAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_reverb_decay", fxReverbDecaySlider);

    setupSlider(fxReverbMixSlider, fxReverbMixLabel, "REVERB MIX");
    fxReverbMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fx_reverb_mix", fxReverbMixSlider);

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

    envDisplay.repaint();
}

void KeshaZeddSynthAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text, juce::Slider::SliderStyle style)
{
    slider.setSliderStyle(style);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 48, 12);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffc5cad8));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::FontOptions(8.5f, juce::Font::bold));
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

    drawSection(16, 118, 304, 430, "Sound Engine & Hybrid", lookAndFeel.getAccentColour(0));
    drawSection(332, 118, 316, 430, "FL Envelope & Echo Suite", lookAndFeel.getAccentColour(1));
    drawSection(660, 118, 304, 430, "Effects & Producer Suite", lookAndFeel.getAccentColour(2));
}

void KeshaZeddSynthAudioProcessorEditor::resized()
{
    // ----------------------------------------------------
    // Header Bar Layout (y: 0 to 46) - Clean & Wide
    // ----------------------------------------------------
    prevPresetButton.setBounds(116, 11, 20, 24);
    presetBox.setBounds(138, 11, 156, 24);
    nextPresetButton.setBounds(296, 11, 20, 24);

    savePresetButton.setBounds(320, 11, 40, 24);
    loadPresetButton.setBounds(362, 11, 40, 24);
    diceButton.setBounds(404, 11, 38, 24);

    zeddifyButton.setBounds(446, 11, 64, 24);
    zeddifyStyleBox.setBounds(512, 11, 84, 24);
    mutateButton.setBounds(598, 11, 50, 24);

    autoMasterButton.setBounds(652, 11, 92, 24);
    themeBox.setBounds(746, 11, 74, 24);
    
    dragMidiButton.setBounds(824, 11, 36, 24);
    dragChordButton.setBounds(862, 11, 36, 24);
    dragHookButton.setBounds(900, 11, 36, 24);
    dragVaultButton.setBounds(938, 11, 36, 24);

    // ----------------------------------------------------
    // Central OLED Screen (y: 54 to 110)
    // ----------------------------------------------------
    audioProcessor.getVisualizer().setBounds(18, 54, getWidth() - 36, 56);

    // ----------------------------------------------------
    // Section 1: Sound Engine & Hybrid Layer (x: 16, y: 118, w: 304, h: 430)
    // ----------------------------------------------------
    // Row 1 (y: 145)
    osc1ShapeLabel.setBounds(24, 142, 85, 13);
    osc1ShapeSlider.setBounds(24, 155, 85, 54);

    unisonDetuneLabel.setBounds(124, 142, 85, 13);
    unisonDetuneSlider.setBounds(124, 155, 85, 54);

    subLevelLabel.setBounds(224, 142, 85, 13);
    subLevelSlider.setBounds(224, 155, 85, 54);

    // Row 2 (y: 225)
    filterCutoffLabel.setBounds(24, 222, 85, 13);
    filterCutoffSlider.setBounds(24, 235, 85, 54);

    filterResLabel.setBounds(124, 222, 85, 13);
    filterResSlider.setBounds(124, 235, 85, 54);

    layerBMixLabel.setBounds(224, 222, 85, 13);
    layerBMixSlider.setBounds(224, 235, 85, 54);

    // Row 3 Dropdowns (y: 310)
    osc1OctaveLabel.setBounds(26, 308, 130, 13);
    osc1OctaveBox.setBounds(26, 322, 130, 22);

    filterModeLabel.setBounds(168, 308, 140, 13);
    filterModeBox.setBounds(168, 322, 140, 22);

    layerBTypeLabel.setBounds(26, 360, 282, 13);
    layerBTypeBox.setBounds(26, 375, 282, 22);

    // ----------------------------------------------------
    // Section 2: FL Studio AHDSR & Echo Suite (x: 332, y: 118, w: 316, h: 430)
    // ----------------------------------------------------
    // FL Envelope Graph Screen (y: 136)
    envDisplay.setBounds(340, 136, 300, 48);

    // Row 1 AHDSR Knobs (y: 190)
    envDelayLabel.setBounds(340, 188, 48, 12);
    envDelaySlider.setBounds(340, 200, 48, 44);

    ampAttackLabel.setBounds(390, 188, 48, 12);
    ampAttackSlider.setBounds(390, 200, 48, 44);

    envHoldLabel.setBounds(440, 188, 48, 12);
    envHoldSlider.setBounds(440, 200, 48, 44);

    ampDecayLabel.setBounds(490, 188, 48, 12);
    ampDecaySlider.setBounds(490, 200, 48, 44);

    ampSustainLabel.setBounds(540, 188, 48, 12);
    ampSustainSlider.setBounds(540, 200, 48, 44);

    ampReleaseLabel.setBounds(590, 188, 48, 12);
    ampReleaseSlider.setBounds(590, 200, 48, 44);

    // Row 2 Tension & Echo Knobs (y: 248)
    envDecTensionLabel.setBounds(340, 246, 56, 12);
    envDecTensionSlider.setBounds(340, 258, 56, 44);

    envRelTensionLabel.setBounds(400, 246, 56, 12);
    envRelTensionSlider.setBounds(400, 258, 56, 44);

    echoFeedLabel.setBounds(460, 246, 56, 12);
    echoFeedSlider.setBounds(460, 258, 56, 44);

    echoTimeLabel.setBounds(520, 246, 56, 12);
    echoTimeSlider.setBounds(520, 258, 56, 44);

    echoPitchLabel.setBounds(580, 246, 56, 12);
    echoPitchSlider.setBounds(580, 258, 56, 44);

    // Row 3 Echo Pan, Echoes Count, Ping Pong, Fat Mode (y: 306)
    echoPanLabel.setBounds(340, 304, 54, 12);
    echoPanSlider.setBounds(340, 316, 54, 44);

    echoCountLabel.setBounds(400, 304, 54, 12);
    echoCountSlider.setBounds(400, 316, 54, 44);

    timeShiftLabel.setBounds(460, 304, 54, 12);
    timeShiftSlider.setBounds(460, 316, 54, 44);

    echoPingPongToggle.setBounds(520, 306, 116, 20);
    echoFatToggle.setBounds(520, 328, 116, 20);

    // Row 4 Momentary Performance Ribbon (y: 366)
    if (tapeStopPad) tapeStopPad->setBounds(340, 366, 70, 22);
    if (stutterPad)  stutterPad->setBounds(415, 366, 70, 22);
    if (divePad)     divePad->setBounds(490, 366, 70, 22);
    if (reversePad)  reversePad->setBounds(565, 366, 74, 22);

    // Row 5 Songwriting & Function toggles (y: 394)
    cutSelfToggle.setBounds(340, 394, 88, 20);
    slideToggle.setBounds(434, 394, 96, 20);
    easyKeyToggle.setBounds(536, 394, 96, 20);

    // Row 6 Chords & Hook dropdowns (y: 418)
    chordProgLabel.setBounds(340, 416, 140, 12);
    chordProgBox.setBounds(340, 428, 140, 20);

    hookGenToggle.setBounds(488, 428, 70, 20);
    generateHookButton.setBounds(562, 428, 72, 20);

    // Row 7 Macros & Scale (y: 456)
    macroDropLabel.setBounds(340, 452, 54, 12);
    macroDropSlider.setBounds(340, 464, 54, 44);

    punchLabel.setBounds(400, 452, 54, 12);
    punchSlider.setBounds(400, 464, 54, 44);

    humanizeLabel.setBounds(460, 452, 54, 12);
    humanizeSlider.setBounds(460, 464, 54, 44);

    scaleRootLabel.setBounds(520, 452, 50, 12);
    scaleRootBox.setBounds(520, 464, 50, 20);

    scaleTypeLabel.setBounds(574, 452, 64, 12);
    scaleTypeBox.setBounds(574, 464, 64, 20);

    // ----------------------------------------------------
    // Section 3: Effects & Space (x: 660, y: 118, w: 304, h: 430)
    // ----------------------------------------------------
    // Row 1 (y: 145)
    fxDriveLabel.setBounds(668, 142, 85, 13);
    fxDriveSlider.setBounds(668, 155, 85, 54);

    fxChorusMixLabel.setBounds(768, 142, 85, 13);
    fxChorusMixSlider.setBounds(768, 155, 85, 54);

    analogDriftLabel.setBounds(868, 142, 85, 13);
    analogDriftSlider.setBounds(868, 155, 85, 54);

    // Row 2: Delay & Reverb (y: 225)
    fxDelayTimeLabel.setBounds(668, 222, 85, 13);
    fxDelayTimeSlider.setBounds(668, 235, 85, 54);

    fxDelayMixLabel.setBounds(768, 222, 85, 13);
    fxDelayMixSlider.setBounds(768, 235, 85, 54);

    fxReverbDecayLabel.setBounds(868, 222, 85, 13);
    fxReverbDecaySlider.setBounds(868, 235, 85, 54);

    // Row 3: Reverb Mix & Glitter Cloud (y: 305)
    fxReverbMixLabel.setBounds(668, 302, 85, 13);
    fxReverbMixSlider.setBounds(668, 315, 85, 54);

    glitterMixLabel.setBounds(768, 302, 85, 13);
    glitterMixSlider.setBounds(768, 315, 85, 54);

    glitterGrainLabel.setBounds(868, 302, 85, 13);
    glitterGrainSlider.setBounds(868, 315, 85, 54);

    // Row 4 Toggles (y: 375)
    pumpToggle.setBounds(668, 375, 136, 22);
    monoMakerToggle.setBounds(810, 375, 140, 22);

    // Row 5 Producer Flavor Suite (y: 410)
    producerFlavorLabel.setBounds(668, 410, 160, 13);
    producerFlavorBox.setBounds(668, 424, 160, 24);

    producerFlavorIntensityLabel.setBounds(850, 408, 95, 13);
    producerFlavorIntensitySlider.setBounds(850, 420, 95, 50);

    // Row 6 Master Volume & VU Meter (y: 476)
    masterVolLabel.setBounds(668, 465, 75, 13);
    masterVolSlider.setBounds(668, 478, 75, 48);
    vuMeter.setBounds(755, 480, 195, 44);
}
