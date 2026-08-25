#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include <functional>
#include <vector>
#include <string>

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

// Drag-and-Drop MIDI Export Handle Component
class DragMidiButton : public juce::Component
{
public:
    DragMidiButton(KeshaZeddSynthAudioProcessor& p) : processor(p) {}
    ~DragMidiButton() override = default;

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(1.0f);
        
        juce::Colour bgColour = isHovered ? juce::Colour(0xff2d3142) : juce::Colour(0xff1a1c24);
        g.setColour(bgColour);
        g.fillRoundedRectangle(bounds, 4.0f);

        g.setColour(isHovered ? juce::Colour(0xffff9900) : juce::Colour(0xff3a3e52));
        g.drawRoundedRectangle(bounds, 4.0f, 1.2f);

        g.setColour(isHovered ? juce::Colour(0xffffaa33) : juce::Colour(0xffc5cad8));
        g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
        g.drawText("EXPORT MIDI", bounds, juce::Justification::centred, false);
    }

    void mouseEnter(const juce::MouseEvent&) override { isHovered = true; repaint(); }
    void mouseExit(const juce::MouseEvent&) override { isHovered = false; repaint(); }

    void mouseDrag(const juce::MouseEvent&) override
    {
        juce::File tempFile = juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("zeddify_riff.mid");
        if (processor.getZeddifyEngine().exportToMidiFile(60, tempFile))
        {
            juce::DragAndDropContainer::performExternalDragDropOfFiles({ tempFile.getFullPathName() }, false, this);
        }
    }

private:
    KeshaZeddSynthAudioProcessor& processor;
    bool isHovered = false;
};

// Stereo Peak/RMS Output VU Meter Component
class VUMeterComponent : public juce::Component, private juce::Timer
{
public:
    VUMeterComponent(KeshaZeddSynthAudioProcessor& p) : processor(p)
    {
        startTimerHz(30);
    }

    ~VUMeterComponent() override
    {
        stopTimer();
    }

    void timerCallback() override
    {
        float targetL = processor.getOutputLevelL();
        float targetR = processor.getOutputLevelR();

        levelL = std::max(targetL, levelL * 0.85f);
        levelR = std::max(targetR, levelR * 0.85f);

        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        g.fillAll(juce::Colour(0xff0d0e12));

        g.setColour(juce::Colour(0xff22242e));
        g.drawRoundedRectangle(bounds, 2.0f, 1.0f);

        float barWidth = (bounds.getWidth() - 5.0f) * 0.5f;
        float h = bounds.getHeight() - 4.0f;

        auto drawBar = [&](float x, float level)
        {
            float fillH = juce::jlimit(0.0f, h, level * h);
            float y = bounds.getBottom() - 2.0f - fillH;

            juce::ColourGradient grad(juce::Colour(0xff00d4ff), x, bounds.getBottom(),
                                      juce::Colour(0xffff3366), x, bounds.getY(), false);
            grad.addColour(0.6f, juce::Colour(0xffffaa00));

            g.setGradientFill(grad);
            g.fillRect(x, y, barWidth, fillH);
        };

        drawBar(bounds.getX() + 1.5f, levelL);
        drawBar(bounds.getX() + 3.5f + barWidth, levelR);
    }

private:
    KeshaZeddSynthAudioProcessor& processor;
    float levelL = 0.0f;
    float levelR = 0.0f;
};

// Categorized Preset Dropdown Menu Component
class PresetComboBox : public juce::ComboBox
{
public:
    PresetComboBox()
    {
        setEditableText(false);
    }
    ~PresetComboBox() override = default;
    
    std::function<void(int)> onPresetSelected;
    std::function<void(const juce::File&)> onUserPresetSelected;

    void showPopup() override { openPresetMenu(); }
    void mouseDown(const juce::MouseEvent&) override { openPresetMenu(); }
    
    void openPresetMenu()
    {
        if (menuShowing) return;
        menuShowing = true;

        juce::PopupMenu menu;
        
        // 1. Basses
        juce::PopupMenu bassesMenu;
        bassesMenu.addItem(1, "Dirty Electro Saw Bass");
        bassesMenu.addItem(2, "Complextro Growl Bass");
        bassesMenu.addItem(3, "Distorted Square Bass");
        bassesMenu.addItem(4, "Acid Squelch Bass");
        bassesMenu.addItem(5, "Sub Smasher");
        bassesMenu.addItem(6, "Metallic FM Reeses Bass");
        bassesMenu.addItem(7, "Talking Formant Bass");
        bassesMenu.addItem(8, "Rubber Band Pluck Bass");
        menu.addSubMenu("01_Basses", bassesMenu);
        
        // 2. Leads
        juce::PopupMenu leadsMenu;
        leadsMenu.addItem(9, "Clarity Supersaw");
        leadsMenu.addItem(10, "8-Bit Glitch Lead");
        leadsMenu.addItem(11, "Vocal Formant Lead");
        leadsMenu.addItem(12, "Stadium Anthem Lead");
        leadsMenu.addItem(13, "Dirty Sync Lead");
        leadsMenu.addItem(14, "Glitter Pop Screamer");
        leadsMenu.addItem(15, "Laser Beam Lead");
        leadsMenu.addItem(16, "Eurodance Rave Saw");
        menu.addSubMenu("02_Leads", leadsMenu);
        
        // 3. Plucks
        juce::PopupMenu plucksMenu;
        plucksMenu.addItem(17, "Zedd Punch Pluck");
        plucksMenu.addItem(18, "Trashy Pop Pluck");
        plucksMenu.addItem(19, "Glass Bell Pluck");
        plucksMenu.addItem(20, "Marimba Synth Strike");
        plucksMenu.addItem(21, "Staccato Arp Bite");
        plucksMenu.addItem(22, "Hollow Square Pluck");
        plucksMenu.addItem(23, "Club Drop Pluck");
        menu.addSubMenu("03_Plucks", plucksMenu);
        
        // 4. Keys & Chords
        juce::PopupMenu keysMenu;
        keysMenu.addItem(24, "Euphoric Pop Chords");
        keysMenu.addItem(25, "Radio Piano-Synth Hybrid");
        keysMenu.addItem(26, "Pumping Synth Brass");
        keysMenu.addItem(27, "Wobbly Lo-Fi Keys");
        keysMenu.addItem(28, "Anthem Organ Stab");
        keysMenu.addItem(29, "Bright EDM Piano Stab");
        keysMenu.addItem(30, "Velvet Neo-Pop Chords");
        menu.addSubMenu("04_Keys & Chords", keysMenu);
        
        // 5. Pads & Textures
        juce::PopupMenu padsMenu;
        padsMenu.addItem(31, "Stadium Sidechain Swell");
        padsMenu.addItem(32, "Shimmering Pop Air");
        padsMenu.addItem(33, "Sunset Warmth Pad");
        padsMenu.addItem(34, "Dark Cinema Swell");
        padsMenu.addItem(35, "Euphoria Choir Wash");
        menu.addSubMenu("05_Pads & Textures", padsMenu);
        
        // 6. Transitions & FX
        juce::PopupMenu fxMenu;
        fxMenu.addItem(36, "Hyperpop Bubble FX");
        fxMenu.addItem(37, "Tension Noise Riser");
        fxMenu.addItem(38, "Sub Drop Boom");
        fxMenu.addItem(39, "Downlifter Laser Fall");
        fxMenu.addItem(40, "Pre-Drop Impact");
        menu.addSubMenu("06_Transitions & FX", fxMenu);

        // 7. Billboard Hits & Anthems
        juce::PopupMenu billboardMenu;
        billboardMenu.addItem(41, "Safe and Sound (Brass Lead)");
        billboardMenu.addItem(42, "Beautiful Now (Zedd Anthem Lead)");
        billboardMenu.addItem(43, "Glad You Came (Club Accordion Pluck)");
        billboardMenu.addItem(44, "Let It Rock (Distorted Rock Lead)");
        billboardMenu.addItem(45, "Right Round (Club Pulse Stab)");
        billboardMenu.addItem(46, "Blow (Glitter Squelch Lead)");
        menu.addSubMenu("07_Billboard Hits & Anthems", billboardMenu);
        
        // User Presets
        juce::File userDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                                .getChildFile("KeshaAndZeddSynth")
                                .getChildFile("UserPresets");
        
        std::vector<juce::File> userFiles;
        
        if (userDir.exists() && userDir.isDirectory())
        {
            juce::PopupMenu userMenu;
            int itemId = 1000;
            
            juce::Array<juce::File> categories;
            userDir.findChildFiles(categories, juce::File::findDirectories, false);
            
            for (auto& cat : categories)
            {
                juce::PopupMenu catMenu;
                juce::Array<juce::File> files;
                cat.findChildFiles(files, juce::File::findFiles, false, "*.json");
                for (auto& file : files)
                {
                    catMenu.addItem(itemId, file.getFileNameWithoutExtension());
                    userFiles.push_back(file);
                    itemId++;
                }
                userMenu.addSubMenu(cat.getFileName(), catMenu);
            }
            
            juce::Array<juce::File> rootFiles;
            userDir.findChildFiles(rootFiles, juce::File::findFiles, false, "*.json");
            for (auto& file : rootFiles)
            {
                userMenu.addItem(itemId, file.getFileNameWithoutExtension());
                userFiles.push_back(file);
                itemId++;
            }
            
            menu.addSubMenu("User Presets", userMenu);
        }

        menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(this),
            [this, userFiles](int result)
            {
                menuShowing = false;
                if (result == 0) return;
                if (result >= 1 && result <= 46)
                {
                    if (onPresetSelected)
                        onPresetSelected(result - 1);
                }
                else if (result >= 1000)
                {
                    size_t fileIdx = static_cast<size_t>(result - 1000);
                    if (fileIdx < userFiles.size() && onUserPresetSelected)
                    {
                        onUserPresetSelected(userFiles[fileIdx]);
                    }
                }
            });
    }

private:
    bool menuShowing = false;
};

// Modern Synth LookAndFeel with 4-Skin Theme Engine
class ModernSynthLookAndFeel : public juce::LookAndFeel_V4
{
public:
    enum Theme
    {
        AnalogLab = 0,
        CyberNeon = 1,
        VintageCream = 2,
        StealthBlackout = 3
    };

    ModernSynthLookAndFeel();
    ~ModernSynthLookAndFeel() override = default;

    void setTheme(int themeIndex);
    int getTheme() const { return currentTheme; }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override;

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox& box) override;

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    juce::Colour getBgColour() const;
    juce::Colour getCardBgColour() const;
    juce::Colour getCardBorderColour() const;
    juce::Colour getAccentColour(int bayIndex) const;
    void drawSidePanels(juce::Graphics& g, int width, int height) const;

private:
    int currentTheme = 0;
};

class KeshaZeddSynthAudioProcessorEditor : public juce::AudioProcessorEditor,
                                           private juce::Timer
{
public:
    KeshaZeddSynthAudioProcessorEditor(KeshaZeddSynthAudioProcessor&);
    ~KeshaZeddSynthAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text, 
                     juce::Slider::SliderStyle style = juce::Slider::RotaryHorizontalVerticalDrag);
    void setupComboBox(juce::ComboBox& box, juce::Label& label, const juce::String& text);
    
    void showSavePresetDialog();

    KeshaZeddSynthAudioProcessor& audioProcessor;
    ModernSynthLookAndFeel lookAndFeel;

    // Header Controls
    PresetComboBox presetBox;
    juce::Label presetLabel;
    juce::TextButton prevPresetButton;
    juce::TextButton nextPresetButton;
    juce::ToggleButton zeddifyButton;
    juce::ComboBox zeddifyStyleBox;
    DragMidiButton dragMidiButton;

    // Master Volume, Auto-Master & Theme
    juce::Slider masterVolSlider;
    juce::Label masterVolLabel;
    juce::ToggleButton autoMasterButton;
    juce::ComboBox themeBox;
    VUMeterComponent vuMeter;
    juce::Label voiceCountLabel;

    // Save, Load, and Dice buttons
    juce::TextButton diceButton;
    juce::TextButton savePresetButton;
    juce::TextButton loadPresetButton;

    // ----------------------------------------------------
    // SECTION 1: SOUND ENGINE & HYBRID LAYER (Left Bay)
    // ----------------------------------------------------
    juce::Slider osc1ShapeSlider;
    juce::Label osc1ShapeLabel;
    juce::Slider unisonDetuneSlider;
    juce::Label unisonDetuneLabel;
    juce::Slider subLevelSlider;
    juce::Label subLevelLabel;
    juce::Slider filterCutoffSlider;
    juce::Label filterCutoffLabel;
    juce::Slider filterResSlider;
    juce::Label filterResLabel;
    juce::Slider layerBMixSlider;
    juce::Label layerBMixLabel;

    juce::ComboBox osc1OctaveBox;
    juce::Label osc1OctaveLabel;
    juce::ComboBox filterModeBox;
    juce::Label filterModeLabel;
    juce::ComboBox layerBTypeBox;
    juce::Label layerBTypeLabel;

    // ----------------------------------------------------
    // SECTION 2: SLIDE, FLAVOR & PERFORMANCE (Center Bay)
    // ----------------------------------------------------
    juce::ToggleButton slideToggle;
    juce::Slider glideTimeSlider;
    juce::Label glideTimeLabel;

    juce::ComboBox producerFlavorBox;
    juce::Label producerFlavorLabel;
    juce::Slider producerFlavorIntensitySlider;
    juce::Label producerFlavorIntensityLabel;

    juce::ToggleButton riserToggle;

    juce::Slider ampAttackSlider;
    juce::Label ampAttackLabel;
    juce::Slider ampDecaySlider;
    juce::Label ampDecayLabel;
    juce::Slider ampSustainSlider;
    juce::Label ampSustainLabel;
    juce::Slider ampReleaseSlider;
    juce::Label ampReleaseLabel;

    juce::Slider macroDropSlider;
    juce::Label macroDropLabel;
    juce::Slider punchSlider;
    juce::Label punchLabel;

    juce::ComboBox scaleRootBox;
    juce::Label scaleRootLabel;
    juce::ComboBox scaleTypeBox;
    juce::Label scaleTypeLabel;

    // ----------------------------------------------------
    // SECTION 3: EFFECTS, SPACE & ANALOG DRIFT (Right Bay)
    // ----------------------------------------------------
    juce::Slider fxDriveSlider;
    juce::Label fxDriveLabel;
    juce::Slider fxChorusMixSlider;
    juce::Label fxChorusMixLabel;
    juce::Slider fxDelayTimeSlider;
    juce::Label fxDelayTimeLabel;
    juce::Slider fxDelayMixSlider;
    juce::Label fxDelayMixLabel;
    juce::Slider fxReverbDecaySlider;
    juce::Label fxReverbDecayLabel;
    juce::Slider fxReverbMixSlider;
    juce::Label fxReverbMixLabel;
    juce::Slider analogDriftSlider;
    juce::Label analogDriftLabel;

    juce::ToggleButton pumpToggle;
    juce::ToggleButton monoMakerToggle;

    // Parameter Attachments
    std::unique_ptr<ComboBoxAttachment> presetAttachment;
    std::unique_ptr<SliderAttachment> masterVolAttachment;
    std::unique_ptr<ButtonAttachment> zeddifyAttachment;
    std::unique_ptr<ComboBoxAttachment> zeddifyStyleAttachment;
    std::unique_ptr<ButtonAttachment> autoMasterAttachment;
    std::unique_ptr<ComboBoxAttachment> themeAttachment;

    std::unique_ptr<SliderAttachment> osc1ShapeAttachment;
    std::unique_ptr<ComboBoxAttachment> osc1OctaveAttachment;
    std::unique_ptr<SliderAttachment> unisonDetuneAttachment;
    std::unique_ptr<SliderAttachment> subLevelAttachment;
    std::unique_ptr<SliderAttachment> filterCutoffAttachment;
    std::unique_ptr<SliderAttachment> filterResAttachment;
    std::unique_ptr<SliderAttachment> layerBMixAttachment;
    std::unique_ptr<ComboBoxAttachment> filterModeAttachment;
    std::unique_ptr<ComboBoxAttachment> layerBTypeAttachment;

    std::unique_ptr<SliderAttachment> glideTimeAttachment;
    std::unique_ptr<ComboBoxAttachment> producerFlavorAttachment;
    std::unique_ptr<SliderAttachment> producerFlavorIntensityAttachment;
    std::unique_ptr<ButtonAttachment> riserAttachment;

    std::unique_ptr<SliderAttachment> ampAttackAttachment;
    std::unique_ptr<SliderAttachment> ampDecayAttachment;
    std::unique_ptr<SliderAttachment> ampSustainAttachment;
    std::unique_ptr<SliderAttachment> ampReleaseAttachment;
    std::unique_ptr<SliderAttachment> macroDropAttachment;
    std::unique_ptr<SliderAttachment> punchAttachment;
    std::unique_ptr<ComboBoxAttachment> scaleRootAttachment;
    std::unique_ptr<ComboBoxAttachment> scaleTypeAttachment;

    std::unique_ptr<SliderAttachment> fxDriveAttachment;
    std::unique_ptr<SliderAttachment> fxChorusMixAttachment;
    std::unique_ptr<SliderAttachment> fxDelayTimeAttachment;
    std::unique_ptr<SliderAttachment> fxDelayMixAttachment;
    std::unique_ptr<SliderAttachment> fxReverbDecayAttachment;
    std::unique_ptr<SliderAttachment> fxReverbMixAttachment;
    std::unique_ptr<SliderAttachment> analogDriftAttachment;
    std::unique_ptr<ButtonAttachment> pumpAttachment;
    std::unique_ptr<ButtonAttachment> monoMakerAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeshaZeddSynthAudioProcessorEditor)
};
