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

// Drag-and-Drop MIDI Export Handle Component (Analog Lab Style Badge)
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
        g.setFont(juce::FontOptions(11.0f, juce::Font::bold));
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

// Stereo Peak/RMS Output VU Meter Component (Analog Lab Style LED ladder)
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

// Interactive 2D X/Y Pad Component (Analog Lab Dark Glass Style)
class XYPad : public juce::Component
{
public:
    XYPad() = default;
    ~XYPad() override = default;
    
    std::function<void(float, float)> onPositionChanged;
    
    void setPosition(float newX, float newY)
    {
        xPos = juce::jlimit(0.0f, 1.0f, newX);
        yPos = juce::jlimit(0.0f, 1.0f, newY);
        repaint();
    }
    
    float getXPosition() const { return xPos; }
    float getYPosition() const { return yPos; }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff101116));
        
        g.setColour(juce::Colour(0xff2b2e3c));
        g.drawRoundedRectangle(getLocalBounds().toFloat(), 4.0f, 1.2f);
        
        // Draw crosshair grid lines
        g.setColour(juce::Colour(0xff222530));
        float dashes[2] = { 3.0f, 3.0f };
        g.drawDashedLine(juce::Line<float>(getWidth() * 0.5f, 0.0f, getWidth() * 0.5f, getHeight()), dashes, 2);
        g.drawDashedLine(juce::Line<float>(0.0f, getHeight() * 0.5f, getWidth(), getHeight() * 0.5f), dashes, 2);
        
        // Grid Labels
        g.setColour(juce::Colour(0xff757a8e));
        g.setFont(juce::FontOptions(9.5f, juce::Font::bold));
        g.drawText("GRIT", 6, getHeight() / 2 - 10, 40, 10, juce::Justification::left);
        g.drawText("TRASH", getWidth() - 46, getHeight() / 2 - 10, 40, 10, juce::Justification::right);
        g.drawText("GLOSS", getWidth() / 2 - 20, 5, 40, 10, juce::Justification::centred);
        g.drawText("CLEAN", getWidth() / 2 - 20, getHeight() - 14, 40, 10, juce::Justification::centred);

        // Draw glowing puck
        float thumbX = xPos * getWidth();
        float thumbY = (1.0f - yPos) * getHeight();
        
        g.setColour(juce::Colour(0x33ffaa00));
        g.fillEllipse(thumbX - 12.0f, thumbY - 12.0f, 24.0f, 24.0f);

        juce::ColourGradient puckGrad(juce::Colour(0xffffaa00), thumbX - 6.0f, thumbY - 6.0f,
                                     juce::Colour(0xffff5500), thumbX + 6.0f, thumbY + 6.0f, false);
        g.setGradientFill(puckGrad);
        g.fillEllipse(thumbX - 7.0f, thumbY - 7.0f, 14.0f, 14.0f);

        g.setColour(juce::Colours::white);
        g.fillEllipse(thumbX - 2.0f, thumbY - 2.0f, 4.0f, 4.0f);
    }
    
    void mouseDown(const juce::MouseEvent& e) override { updatePosition(e.position); }
    void mouseDrag(const juce::MouseEvent& e) override { updatePosition(e.position); }
    
private:
    void updatePosition(juce::Point<float> p)
    {
        xPos = juce::jlimit(0.0f, 1.0f, p.getX() / getWidth());
        yPos = juce::jlimit(0.0f, 1.0f, 1.0f - (p.getY() / getHeight()));
        if (onPositionChanged)
            onPositionChanged(xPos, yPos);
        repaint();
    }
    
    float xPos = 0.5f;
    float yPos = 0.5f;
};

// Categorized Preset Dropdown Menu Component (Analog Lab Browser Style)
class PresetComboBox : public juce::ComboBox
{
public:
    PresetComboBox() = default;
    ~PresetComboBox() override = default;
    
    std::function<void(int)> onPresetSelected;
    std::function<void(const juce::File&)> onUserPresetSelected;
    
    void showPopup() override
    {
        juce::PopupMenu menu;
        
        // Factory Presets
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
        
        juce::PopupMenu plucksMenu;
        plucksMenu.addItem(17, "Zedd Punch Pluck");
        plucksMenu.addItem(18, "Trashy Pop Pluck");
        plucksMenu.addItem(19, "Glass Bell Pluck");
        plucksMenu.addItem(20, "Marimba Synth Strike");
        plucksMenu.addItem(21, "Staccato Arp Bite");
        plucksMenu.addItem(22, "Hollow Square Pluck");
        plucksMenu.addItem(23, "Club Drop Pluck");
        menu.addSubMenu("03_Plucks", plucksMenu);
        
        juce::PopupMenu keysMenu;
        keysMenu.addItem(24, "Euphoric Pop Chords");
        keysMenu.addItem(25, "Radio Piano-Synth Hybrid");
        keysMenu.addItem(26, "Pumping Synth Brass");
        keysMenu.addItem(27, "Wobbly Lo-Fi Keys");
        keysMenu.addItem(28, "Anthem Organ Stab");
        keysMenu.addItem(29, "Bright EDM Piano Stab");
        keysMenu.addItem(30, "Velvet Neo-Pop Chords");
        menu.addSubMenu("04_Keys & Chords", keysMenu);
        
        juce::PopupMenu padsMenu;
        padsMenu.addItem(31, "Stadium Sidechain Swell");
        padsMenu.addItem(32, "Shimmering Pop Air");
        padsMenu.addItem(33, "Sunset Warmth Pad");
        padsMenu.addItem(34, "Dark Cinema Swell");
        padsMenu.addItem(35, "Euphoria Choir Wash");
        menu.addSubMenu("05_Pads & Textures", padsMenu);
        
        juce::PopupMenu fxMenu;
        fxMenu.addItem(36, "Hyperpop Bubble FX");
        fxMenu.addItem(37, "Tension Noise Riser");
        fxMenu.addItem(38, "Sub Drop Boom");
        fxMenu.addItem(39, "Downlifter Laser Fall");
        fxMenu.addItem(40, "Pre-Drop Impact");
        menu.addSubMenu("06_Transitions & FX", fxMenu);
        
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
                if (result == 0) return;
                if (result >= 1 && result <= 40)
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
};

// Analog Lab Inspired LookAndFeel (Brushed Gunmetal, Warm Wood Cheeks, Illuminated Knobs)
class ModernSynthLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ModernSynthLookAndFeel();
    ~ModernSynthLookAndFeel() override = default;

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override;

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          const juce::Slider::SliderStyle style, juce::Slider& slider) override;

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox& box) override;

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
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

    // Header Controls & Zeddify Engine
    PresetComboBox presetBox;
    juce::Label presetLabel;
    juce::TextButton prevPresetButton;
    juce::TextButton nextPresetButton;
    juce::ToggleButton zeddifyButton;
    DragMidiButton dragMidiButton;

    // Master Volume & Auto-Master
    juce::Slider masterVolSlider;
    juce::Label masterVolLabel;
    juce::ToggleButton autoMasterButton;
    juce::Slider autoMasterIntensitySlider;
    juce::Label autoMasterIntensityLabel;
    VUMeterComponent vuMeter;
    juce::Label voiceCountLabel;

    // Save, Load, and Dice buttons
    juce::TextButton diceButton;
    juce::TextButton savePresetButton;
    juce::TextButton loadPresetButton;

    // Oscillator 1 & Unison Controls
    juce::Slider osc1ShapeSlider;
    juce::Label osc1ShapeLabel;
    juce::ComboBox osc1OctaveBox;
    juce::Label osc1OctaveLabel;
    juce::Slider osc1DetuneSlider;
    juce::Label osc1DetuneLabel;
    juce::Slider osc1LevelSlider;
    juce::Label osc1LevelLabel;
    juce::ComboBox unisonCountBox;
    juce::Label unisonCountLabel;
    juce::Slider unisonDetuneSlider;
    juce::Label unisonDetuneLabel;

    // Oscillator 2 & Mod Controls
    juce::Slider osc2ShapeSlider;
    juce::Label osc2ShapeLabel;
    juce::ComboBox osc2OctaveBox;
    juce::Label osc2OctaveLabel;
    juce::Slider osc2DetuneSlider;
    juce::Label osc2DetuneLabel;
    juce::Slider osc2LevelSlider;
    juce::Label osc2LevelLabel;
    juce::Slider oscFmSlider;
    juce::Label oscFmLabel;
    juce::ComboBox oscSyncBox;
    juce::Label oscSyncLabel;

    // Filter Controls
    juce::ComboBox filterModeBox;
    juce::Label filterModeLabel;
    juce::Slider filterCutoffSlider;
    juce::Label filterCutoffLabel;
    juce::Slider filterResSlider;
    juce::Label filterResLabel;
    juce::Slider filterDriveSlider;
    juce::Label filterDriveLabel;
    juce::Slider filterEnvAmtSlider;
    juce::Label filterEnvAmtLabel;
    juce::Slider filterKeyTrackSlider;
    juce::Label filterKeyTrackLabel;
    juce::Slider filterLfoModSlider;
    juce::Label filterLfoModLabel;
    juce::Slider formantMorphSlider;
    juce::Label formantMorphLabel;

    // Filter ADSR & Curve
    juce::Slider filterAttackSlider, filterDecaySlider, filterSustainSlider, filterReleaseSlider, filterCurveSlider;
    juce::Label filterAttackLabel, filterDecayLabel, filterSustainLabel, filterReleaseLabel, filterCurveLabel;

    // Amp ADSR & Curve Controls
    juce::Slider ampAttackSlider, ampDecaySlider, ampSustainSlider, ampReleaseSlider, ampCurveSlider;
    juce::Label ampAttackLabel, ampDecayLabel, ampSustainLabel, ampReleaseLabel, ampCurveLabel;

    // LFO Modulation Bay
    juce::ComboBox lfo1WaveBox, lfo2WaveBox;
    juce::Label lfo1WaveLabel, lfo2WaveLabel;
    juce::ComboBox lfo1RateBox, lfo2RateBox;
    juce::Label lfo1RateLabel, lfo2RateLabel;
    juce::Slider lfo1ToCutoffSlider, lfo1ToShapeSlider;
    juce::Label lfo1ToCutoffLabel, lfo1ToShapeLabel;
    juce::Slider lfo2ToPitchSlider, lfo2ToPanSlider;
    juce::Label lfo2ToPitchLabel, lfo2ToPanLabel;

    // Post FX Controls
    juce::Slider fxDriveSlider;
    juce::Label fxDriveLabel;
    juce::Slider fxChorusRateSlider, fxChorusDepthSlider, fxChorusMixSlider;
    juce::Label fxChorusRateLabel, fxChorusDepthLabel, fxChorusMixLabel;
    juce::Slider fxDelayTimeSlider, fxDelayFeedbackSlider, fxDelayColorSlider, fxDelayMixSlider;
    juce::Label fxDelayTimeLabel, fxDelayFeedbackLabel, fxDelayColorLabel, fxDelayMixLabel;
    juce::Slider fxReverbDecaySlider, fxReverbDampingSlider, fxReverbWidthSlider, fxReverbMixSlider;
    juce::Label fxReverbDecayLabel, fxReverbDampingLabel, fxReverbWidthLabel, fxReverbMixLabel;

    // Sub Anchor Bay
    juce::ComboBox subWaveBox, subOctaveBox;
    juce::Label subWaveLabel, subOctaveLabel;
    juce::Slider subLevelSlider, subDriveSlider;
    juce::Label subLevelLabel, subDriveLabel;

    // Settings & Voicing
    juce::ComboBox playModeBox, chordModeBox, chordTypeBox, glideModeBox;
    juce::Label playModeLabel, chordModeLabel, chordTypeLabel, glideModeLabel;
    juce::ComboBox pitchDropActiveBox;
    juce::Label pitchDropActiveLabel;
    juce::Slider pitchDropOctavesSlider, pitchDropTimeSlider;
    juce::Label pitchDropOctavesLabel, pitchDropTimeLabel;
    juce::Slider glideTimeSlider;
    juce::Label glideTimeLabel;

    // Ducking Pumper
    juce::ComboBox pumpActiveBox, pumpDivisionBox;
    juce::Label pumpActiveLabel, pumpDivisionLabel;
    juce::Slider pumpDepthSlider, pumpCurveSlider;
    juce::Label pumpDepthLabel, pumpCurveLabel;

    // Mono Maker
    juce::ComboBox monoMakerActiveBox;
    juce::Label monoMakerActiveLabel;
    juce::Slider monoMakerFreqSlider;
    juce::Label monoMakerFreqLabel;

    // Transient Shaper Bay
    juce::ComboBox transientTypeBox;
    juce::Label transientTypeLabel;
    juce::Slider clickLevelSlider, transientDecaySlider;
    juce::Label clickLevelLabel, transientDecayLabel;

    // Interactive 2D Pad (Trash vs Gloss)
    XYPad trashGlossPad;

    // Macro Controls
    juce::Slider macroDropSlider, punchSlider, gritSlider, spaceSlider, widthSlider;
    juce::Label macroDropLabel, punchLabel, gritLabel, spaceLabel, widthLabel;

    // Parameter Attachments
    std::unique_ptr<ComboBoxAttachment> presetAttachment;
    std::unique_ptr<SliderAttachment> masterVolAttachment;
    std::unique_ptr<ButtonAttachment> zeddifyAttachment;
    std::unique_ptr<ButtonAttachment> autoMasterAttachment;
    std::unique_ptr<SliderAttachment> autoMasterIntensityAttachment;

    std::unique_ptr<SliderAttachment> osc1ShapeAttachment;
    std::unique_ptr<ComboBoxAttachment> osc1OctaveAttachment, unisonCountAttachment;
    std::unique_ptr<SliderAttachment> osc1DetuneAttachment, osc1LevelAttachment, unisonDetuneAttachment;

    std::unique_ptr<SliderAttachment> osc2ShapeAttachment;
    std::unique_ptr<ComboBoxAttachment> osc2OctaveAttachment;
    std::unique_ptr<SliderAttachment> osc2DetuneAttachment, osc2LevelAttachment, oscFmAttachment;
    std::unique_ptr<ComboBoxAttachment> oscSyncAttachment;

    std::unique_ptr<ComboBoxAttachment> filterModeAttachment;
    std::unique_ptr<SliderAttachment> filterCutoffAttachment, filterResAttachment, filterDriveAttachment, filterEnvAmtAttachment, filterKeyTrackAttachment, filterLfoModAttachment, formantMorphAttachment;
    std::unique_ptr<SliderAttachment> filterAttackAttachment, filterDecayAttachment, filterSustainAttachment, filterReleaseAttachment, filterCurveAttachment;

    std::unique_ptr<SliderAttachment> ampAttackAttachment, ampDecayAttachment, ampSustainAttachment, ampReleaseAttachment, ampCurveAttachment;

    std::unique_ptr<ComboBoxAttachment> lfo1WaveAttachment, lfo1RateAttachment, lfo2WaveAttachment, lfo2RateAttachment;
    std::unique_ptr<SliderAttachment> lfo1ToCutoffAttachment, lfo1ToShapeAttachment, lfo2ToPitchAttachment, lfo2ToPanAttachment;

    std::unique_ptr<SliderAttachment> fxDriveAttachment;
    std::unique_ptr<SliderAttachment> fxChorusRateAttachment, fxChorusDepthAttachment, fxChorusMixAttachment;
    std::unique_ptr<SliderAttachment> fxDelayTimeAttachment, fxDelayFeedbackAttachment, fxDelayColorAttachment, fxDelayMixAttachment;
    std::unique_ptr<SliderAttachment> fxReverbDecayAttachment, fxReverbDampingAttachment, fxReverbWidthAttachment, fxReverbMixAttachment;

    std::unique_ptr<ComboBoxAttachment> subWaveAttachment, subOctaveAttachment;
    std::unique_ptr<SliderAttachment> subLevelAttachment, subDriveAttachment;

    std::unique_ptr<ComboBoxAttachment> playModeAttachment, chordModeAttachment, chordTypeAttachment, glideModeAttachment;
    std::unique_ptr<ComboBoxAttachment> pitchDropActiveAttachment;
    std::unique_ptr<SliderAttachment> pitchDropOctavesAttachment, pitchDropTimeAttachment, glideTimeAttachment;

    std::unique_ptr<ComboBoxAttachment> pumpActiveAttachment, pumpDivisionAttachment;
    std::unique_ptr<SliderAttachment> pumpDepthAttachment, pumpCurveAttachment;

    std::unique_ptr<ComboBoxAttachment> monoMakerActiveAttachment;
    std::unique_ptr<SliderAttachment> monoMakerFreqAttachment;

    std::unique_ptr<ComboBoxAttachment> transientTypeAttachment;
    std::unique_ptr<SliderAttachment> clickLevelAttachment, transientDecayAttachment;

    std::unique_ptr<SliderAttachment> macroDropAttachment;
    std::unique_ptr<SliderAttachment> punchAttachment, gritAttachment, spaceAttachment, widthAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeshaZeddSynthAudioProcessorEditor)
};
