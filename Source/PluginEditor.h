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

// Interactive FL Studio Envelope Graph Display Component
class FLEnvelopeDisplayComponent : public juce::Component
{
public:
    FLEnvelopeDisplayComponent(KeshaZeddSynthAudioProcessor& p) : processor(p) {}
    ~FLEnvelopeDisplayComponent() override = default;

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        
        // Dark LCD background
        g.setColour(juce::Colour(0xff0e131d));
        g.fillRoundedRectangle(bounds, 4.0f);

        g.setColour(juce::Colour(0xff222b3d));
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

        // Fetch envelope values
        float delayVal = (processor.getAPVTS().getRawParameterValue("env_delay") != nullptr) ? processor.getAPVTS().getRawParameterValue("env_delay")->load() : 0.0f;
        float attVal   = processor.getAPVTS().getRawParameterValue("amp_attack")->load();
        float holdVal  = (processor.getAPVTS().getRawParameterValue("env_hold") != nullptr) ? processor.getAPVTS().getRawParameterValue("env_hold")->load() : 0.0f;
        float decVal   = processor.getAPVTS().getRawParameterValue("amp_decay")->load();
        float susVal   = processor.getAPVTS().getRawParameterValue("amp_sustain")->load();
        float relVal   = processor.getAPVTS().getRawParameterValue("amp_release")->load();
        float decTension = (processor.getAPVTS().getRawParameterValue("env_dec_tension") != nullptr) ? processor.getAPVTS().getRawParameterValue("env_dec_tension")->load() : 1.0f;

        float totalTime = std::max(0.2f, delayVal + attVal + holdVal + decVal + relVal);
        float w = bounds.getWidth() - 16.0f;
        float startX = bounds.getX() + 8.0f;
        float startY = bounds.getBottom() - 6.0f;
        float topY = bounds.getY() + 6.0f;

        float x0 = startX;
        float y0 = startY;

        float x1 = x0 + (delayVal / totalTime) * w;
        float y1 = startY;

        float x2 = x1 + (attVal / totalTime) * w;
        float y2 = topY;

        float x3 = x2 + (holdVal / totalTime) * w;
        float y3 = topY;

        float x4 = x3 + (decVal / totalTime) * w;
        float y4 = startY - susVal * (startY - topY);

        float x5 = x4 + 20.0f; // Sustain visual segment
        float y5 = y4;

        float x6 = std::min(bounds.getRight() - 8.0f, x5 + (relVal / totalTime) * w);
        float y6 = startY;

        // Draw Envelope Fill with sunset gradient
        juce::Path fillPath;
        fillPath.startNewSubPath(x0, y0);
        fillPath.lineTo(x1, y1);
        fillPath.lineTo(x2, y2);
        fillPath.lineTo(x3, y3);

        int numCurvSteps = 12;
        for (int step = 1; step <= numCurvSteps; ++step)
        {
            float prog = static_cast<float>(step) / static_cast<float>(numCurvSteps);
            float curProg = std::pow(prog, decTension);
            float curX = x3 + prog * (x4 - x3);
            float curY = y3 + curProg * (y4 - y3);
            fillPath.lineTo(curX, curY);
        }

        fillPath.lineTo(x5, y5);
        fillPath.lineTo(x6, y6);
        fillPath.lineTo(x6, startY);
        fillPath.lineTo(x0, startY);
        fillPath.closeSubPath();

        juce::ColourGradient fillGrad(juce::Colour(0xffff6b35).withAlpha(0.35f), 0.0f, topY,
                                      juce::Colour(0xff00f0ff).withAlpha(0.08f), 0.0f, startY, false);
        g.setGradientFill(fillGrad);
        g.fillPath(fillPath);

        // Draw Envelope Outline
        juce::Path strokePath;
        strokePath.startNewSubPath(x0, y0);
        strokePath.lineTo(x1, y1);
        strokePath.lineTo(x2, y2);
        strokePath.lineTo(x3, y3);

        for (int step = 1; step <= numCurvSteps; ++step)
        {
            float prog = static_cast<float>(step) / static_cast<float>(numCurvSteps);
            float curProg = std::pow(prog, decTension);
            float curX = x3 + prog * (x4 - x3);
            float curY = y3 + curProg * (y4 - y3);
            strokePath.lineTo(curX, curY);
        }

        strokePath.lineTo(x5, y5);
        strokePath.lineTo(x6, y6);

        g.setColour(juce::Colour(0xffff6b35));
        g.strokePath(strokePath, juce::PathStrokeType(1.8f));

        // Draw vertex nodes
        auto drawVertex = [&](float vx, float vy) {
            g.setColour(juce::Colour(0xff0e131d));
            g.fillEllipse(vx - 3.5f, vy - 3.5f, 7.0f, 7.0f);
            g.setColour(juce::Colour(0xffffd166));
            g.drawEllipse(vx - 3.5f, vy - 3.5f, 7.0f, 7.0f, 1.4f);
        };

        drawVertex(x1, y1);
        drawVertex(x2, y2);
        drawVertex(x3, y3);
        drawVertex(x4, y4);
        drawVertex(x6, y6);
    }

private:
    KeshaZeddSynthAudioProcessor& processor;
};

// Momentary Performance Trigger Pad Component
class MomentaryPadButton : public juce::Component
{
public:
    MomentaryPadButton(const juce::String& labelText, juce::Colour activeCol, std::function<void(bool)> triggerCb)
        : text(labelText), accentColour(activeCol), callback(triggerCb) {}
    ~MomentaryPadButton() override = default;

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(1.0f);
        
        juce::Colour bg = isDown ? accentColour.withAlpha(0.40f) : (isHovered ? juce::Colour(0xff22293a) : juce::Colour(0xff141824));
        g.setColour(bg);
        g.fillRoundedRectangle(bounds, 4.0f);

        juce::Colour border = isDown ? accentColour : (isHovered ? accentColour.withAlpha(0.6f) : juce::Colour(0xff2a334a));
        g.setColour(border);
        g.drawRoundedRectangle(bounds, 4.0f, isDown ? 1.8f : 1.0f);

        // Text
        g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
        g.setColour(isDown ? juce::Colours::white : (isHovered ? accentColour : juce::Colour(0xffd0d8e8)));
        g.drawText(text, bounds, juce::Justification::centred);
    }

    void mouseDown(const juce::MouseEvent&) override
    {
        isDown = true;
        repaint();
        if (callback) callback(true);
    }

    void mouseUp(const juce::MouseEvent&) override
    {
        isDown = false;
        repaint();
        if (callback) callback(false);
    }

    void mouseEnter(const juce::MouseEvent&) override { isHovered = true; repaint(); }
    void mouseExit(const juce::MouseEvent&) override  { isHovered = false; isDown = false; repaint(); }

private:
    juce::String text;
    juce::Colour accentColour;
    std::function<void(bool)> callback;
    bool isDown = false;
    bool isHovered = false;
};

// Drag-and-Drop MIDI Export Handle Component (Zeddify, Chords, Hook, Vault)
class DragMidiButton : public juce::Component
{
public:
    enum ExportType { Zeddify = 0, Chords = 1, Hook = 2, Vault = 3 };

    DragMidiButton(KeshaZeddSynthAudioProcessor& p, ExportType type = Zeddify) 
        : processor(p), exportType(type) {}
    ~DragMidiButton() override = default;

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(1.0f);
        
        juce::Colour bgColour = isHovered ? juce::Colour(0xff2d3142) : juce::Colour(0xff1a1c24);
        g.setColour(bgColour);
        g.fillRoundedRectangle(bounds, 4.0f);

        juce::Colour borderCol = (exportType == Chords) ? (isHovered ? juce::Colour(0xff00d4ff) : juce::Colour(0xff2b4555))
                               : (exportType == Hook)   ? (isHovered ? juce::Colour(0xffd400ff) : juce::Colour(0xff452b55))
                               : (exportType == Vault)  ? (isHovered ? juce::Colour(0xff00ffaa) : juce::Colour(0xff2b5545))
                                                        : (isHovered ? juce::Colour(0xffff9900) : juce::Colour(0xff3a3e52));
        g.setColour(borderCol);
        g.drawRoundedRectangle(bounds, 4.0f, 1.2f);

        juce::String labelStr = (exportType == Chords) ? "CHORDS"
                              : (exportType == Hook)   ? "HOOK"
                              : (exportType == Vault)  ? "VAULT"
                                                       : "ARP";

        juce::Colour textCol = (exportType == Chords) ? (isHovered ? juce::Colour(0xff88eaff) : juce::Colour(0xff9ec7d8))
                             : (exportType == Hook)   ? (isHovered ? juce::Colour(0xfff0b8ff) : juce::Colour(0xffc59ed8))
                             : (exportType == Vault)  ? (isHovered ? juce::Colour(0xff88ffcc) : juce::Colour(0xff9ed8bf))
                                                      : (isHovered ? juce::Colour(0xffffaa33) : juce::Colour(0xffc5cad8));
        g.setColour(textCol);
        g.setFont(juce::FontOptions(9.0f, juce::Font::bold));
        g.drawText(labelStr, bounds, juce::Justification::centred, false);
    }

    void mouseEnter(const juce::MouseEvent&) override { isHovered = true; repaint(); }
    void mouseExit(const juce::MouseEvent&) override { isHovered = false; repaint(); }

    void mouseDrag(const juce::MouseEvent&) override
    {
        int root = static_cast<int>(processor.getAPVTS().getRawParameterValue("scale_root")->load());
        int scale = static_cast<int>(processor.getAPVTS().getRawParameterValue("scale_type")->load());

        if (exportType == Chords)
        {
            juce::File tempFile = juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("chord_progression.mid");
            int prog = static_cast<int>(processor.getAPVTS().getRawParameterValue("chord_prog_preset")->load());
            if (processor.getChordProgEngine().exportProgressionMidi(prog, root, tempFile))
                juce::DragAndDropContainer::performExternalDragDropOfFiles({ tempFile.getFullPathName() }, false, this);
        }
        else if (exportType == Hook)
        {
            juce::File tempFile = juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("topline_hook.mid");
            if (processor.getHookEngine().exportHookMidi(root, scale, tempFile))
                juce::DragAndDropContainer::performExternalDragDropOfFiles({ tempFile.getFullPathName() }, false, this);
        }
        else if (exportType == Vault)
        {
            juce::File tempFile = juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("vault_melody.mid");
            if (processor.getMidiVault().exportItemToMidi(vaultIndex, root, scale, tempFile))
                juce::DragAndDropContainer::performExternalDragDropOfFiles({ tempFile.getFullPathName() }, false, this);
        }
        else
        {
            juce::File tempFile = juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("zeddify_riff.mid");
            if (processor.getZeddifyEngine().exportToMidiFile(60, tempFile))
                juce::DragAndDropContainer::performExternalDragDropOfFiles({ tempFile.getFullPathName() }, false, this);
        }
    }

    void setVaultIndex(int idx) { vaultIndex = idx; }

private:
    KeshaZeddSynthAudioProcessor& processor;
    ExportType exportType;
    int vaultIndex = 0;
    bool isHovered = false;
};

// Stereo VU Meter Component
class VUMeterComponent : public juce::Component
{
public:
    VUMeterComponent(KeshaZeddSynthAudioProcessor& p) : processor(p) {}
    ~VUMeterComponent() override = default;

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        
        // Background
        g.setColour(juce::Colour(0xff0e131d));
        g.fillRoundedRectangle(bounds, 3.0f);
        
        g.setColour(juce::Colour(0xff222b3d));
        g.drawRoundedRectangle(bounds, 3.0f, 1.0f);

        float lvlL = processor.getOutputLevelL();
        float lvlR = processor.getOutputLevelR();

        float barH = (bounds.getHeight() - 6.0f) * 0.5f;
        float maxW = bounds.getWidth() - 8.0f;

        auto drawChannelMeter = [&](float y, float level, const juce::String& label) {
            // Label
            g.setFont(juce::FontOptions(8.0f, juce::Font::bold));
            g.setColour(juce::Colour(0xff8896b0));
            g.drawText(label, (int)(bounds.getX() + 4.0f), (int)y, 10, (int)barH, juce::Justification::centredLeft);

            float fillW = juce::jlimit(0.0f, maxW - 14.0f, level * (maxW - 14.0f));
            juce::Rectangle<float> meterRect(bounds.getX() + 16.0f, y + 2.0f, fillW, barH - 4.0f);

            juce::ColourGradient grad(juce::Colour(0xff00f0ff), bounds.getX() + 16.0f, 0.0f,
                                      juce::Colour(0xffff2a6d), bounds.getX() + 16.0f + (maxW - 14.0f), 0.0f, false);
            grad.addColour(0.7, juce::Colour(0xffffd166));

            g.setGradientFill(grad);
            g.fillRoundedRectangle(meterRect, 1.5f);
        };

        drawChannelMeter(bounds.getY() + 2.0f, lvlL, "L");
        drawChannelMeter(bounds.getY() + 2.0f + barH + 2.0f, lvlR, "R");
    }

private:
    KeshaZeddSynthAudioProcessor& processor;
};

// Custom LookAndFeel for Modern Beach Festival House Vibe
class ModernSynthLookAndFeel : public juce::LookAndFeel_V4
{
public:
    enum Theme { BeachFestival = 0, MiamiVice = 1, IbizaGold = 2, StealthBlackout = 3 };

    ModernSynthLookAndFeel();
    ~ModernSynthLookAndFeel() override = default;

    void setTheme(int themeIndex);
    int getTheme() const { return currentTheme; }

    juce::Colour getBgColour() const;
    juce::Colour getCardBgColour() const;
    juce::Colour getCardBorderColour() const;
    juce::Colour getAccentColour(int index) const;

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override;

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox& box) override;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

    void drawSidePanels(juce::Graphics& g, int width, int height) const;

private:
    int currentTheme = BeachFestival;
};

// Categorized Preset Dropdown Component
class CustomPresetComboBox : public juce::ComboBox
{
public:
    CustomPresetComboBox() = default;
    ~CustomPresetComboBox() override = default;

    std::function<void(int)> onPresetSelected;

    void showPopup() override
    {
        juce::PopupMenu menu;

        // 1. Zedd Iconic Anthems
        juce::PopupMenu zeddMenu;
        zeddMenu.addItem(1, "Clarity Supersaw Stack");
        zeddMenu.addItem(2, "Beautiful Now Anthem Lead");
        zeddMenu.addItem(3, "Zedd Punch Pluck");
        zeddMenu.addItem(4, "Complextro Growl Bass");
        zeddMenu.addItem(5, "Stay The Night Laser");
        zeddMenu.addItem(6, "Spectrum Stadium Lead");
        menu.addSubMenu("01_Zedd Signature", zeddMenu);

        // 2. Kesha Glitter Party
        juce::PopupMenu keshaMenu;
        keshaMenu.addItem(7, "Tik Tok Glitter Pulse");
        keshaMenu.addItem(8, "Blow Squelch Screamer");
        keshaMenu.addItem(9, "Take It Off Dirty Hook");
        keshaMenu.addItem(10, "Die Young Acoustic Drop");
        keshaMenu.addItem(11, "Your Love Is My Drug Brass");
        keshaMenu.addItem(12, "Crazy Kids Power Riff");
        menu.addSubMenu("02_Kesha Glitter Party", keshaMenu);

        // 3. Beach House Basslines
        juce::PopupMenu bassMenu;
        bassMenu.addItem(13, "Dirty Electro Saw Bass");
        bassMenu.addItem(14, "Rubber Band Pluck Bass");
        bassMenu.addItem(15, "Acid Squelch Bass");
        bassMenu.addItem(16, "Sub Smasher (4-on-Floor)");
        bassMenu.addItem(17, "Metallic FM Growl Bass");
        bassMenu.addItem(18, "Talking Formant Bass");
        menu.addSubMenu("03_Festival Basslines", bassMenu);

        // 4. Festival Anthem Leads
        juce::PopupMenu leadMenu;
        leadMenu.addItem(19, "Stadium Anthem Lead");
        leadMenu.addItem(20, "Laser Beam Screamer");
        leadMenu.addItem(21, "Eurodance Rave Saw");
        leadMenu.addItem(22, "Vocal Formant Lead");
        leadMenu.addItem(23, "Dirty Sync Lead");
        leadMenu.addItem(24, "8-Bit Glitch Lead");
        menu.addSubMenu("04_Festival Leads", leadMenu);

        // 5. Beach Plucks & Bells
        juce::PopupMenu pluckMenu;
        pluckMenu.addItem(25, "Glass Bell Pluck");
        pluckMenu.addItem(26, "Trashy Pop Pluck");
        pluckMenu.addItem(27, "Marimba Synth Strike");
        pluckMenu.addItem(28, "Staccato Arp Bite");
        pluckMenu.addItem(29, "Hollow Square Pluck");
        pluckMenu.addItem(30, "Club Drop Pluck");
        menu.addSubMenu("05_Plucks & Bells", pluckMenu);

        // 6. Stadium Chords & Keys
        juce::PopupMenu chordMenu;
        chordMenu.addItem(31, "Euphoric Pop Chords");
        chordMenu.addItem(32, "Radio Piano-Synth Hybrid");
        chordMenu.addItem(33, "Pumping Synth Brass");
        chordMenu.addItem(34, "Bright EDM Piano Stab");
        chordMenu.addItem(35, "Anthem Organ Stab");
        chordMenu.addItem(36, "Velvet Neo-Pop Chords");
        menu.addSubMenu("06_Stadium Chords", chordMenu);

        // 7. Sunset Atmospheric Pads
        juce::PopupMenu padMenu;
        padMenu.addItem(37, "Lush Sidechain Pad");
        padMenu.addItem(38, "Shimmer Dream Pad");
        padMenu.addItem(39, "Dark Cinema Drone");
        padMenu.addItem(40, "Retro Vapor Sweep");
        padMenu.addItem(41, "Glacier Atmosphere");
        menu.addSubMenu("07_Atmospheric Pads", padMenu);

        // 8. Riser & Tension FX
        juce::PopupMenu fxMenu;
        fxMenu.addItem(42, "Hyperpop Bubble FX");
        fxMenu.addItem(43, "Tension Noise Riser");
        fxMenu.addItem(44, "Sub Drop Boom");
        fxMenu.addItem(45, "Downlifter Laser Fall");
        fxMenu.addItem(46, "Pre-Drop Stadium Impact");
        menu.addSubMenu("08_FX & Risers", fxMenu);

        // 9. Full Track Suites
        juce::PopupMenu suitesMenu;
        suitesMenu.addItem(47, "Blow - Staccato Party Pluck");
        suitesMenu.addItem(48, "Blow - Dirty Squelch Bass");
        suitesMenu.addItem(49, "Blow - Pumping Anthem Chords");
        suitesMenu.addItem(50, "Blow - Screaming Laser Lead");
        suitesMenu.addItem(51, "Beautiful Now - Euphoric Drop Chords");
        suitesMenu.addItem(52, "Beautiful Now - Soaring Festival Lead");
        suitesMenu.addItem(53, "Beautiful Now - Clockwork Bell Pluck");
        suitesMenu.addItem(54, "Beautiful Now - Rolling Progressive Bass");
        suitesMenu.addItem(55, "Right Round - Club Pulse Synth Stab");
        suitesMenu.addItem(56, "Right Round - Dirty Electro Slap Bass");
        suitesMenu.addItem(57, "Let It Rock - Overdriven Power-Saw Riff");
        suitesMenu.addItem(58, "Let It Rock - Screaming Laser Lead");
        suitesMenu.addItem(59, "Take It Off - Sharp Confetti Hook Pluck");
        suitesMenu.addItem(60, "Take It Off - Festival Pumping Supersaws");
        suitesMenu.addItem(61, "Clarity - Pure Complextro Saw Stack");
        suitesMenu.addItem(62, "Die Young - Acoustic Pop Drop Lead");
        suitesMenu.addItem(63, "Stay The Night - Stadium Laser Lead");
        suitesMenu.addItem(64, "Album Master - Final Club Limiter FX");
        menu.addSubMenu("09_Complete Track Suites", suitesMenu);

        // User Presets
        juce::File userDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                                .getChildFile("LeesHouse")
                                .getChildFile("UserPresets");
        if (userDir.exists() && userDir.isDirectory())
        {
            auto subDirs = userDir.findChildFiles(juce::File::findDirectories, false);
            int userMenuId = 1000;
            for (const auto& subDir : subDirs)
            {
                juce::PopupMenu subMenu;
                auto files = subDir.findChildFiles(juce::File::findFiles, false, "*.kzpreset");
                for (const auto& f : files)
                {
                    subMenu.addItem(userMenuId++, f.getFileNameWithoutExtension());
                }
                if (subMenu.getNumItems() > 0)
                    menu.addSubMenu("User: " + subDir.getFileName(), subMenu);
            }
        }

        menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(this),
            [this](int result)
            {
                if (result == 0) return;
                if (result >= 1 && result <= 84)
                {
                    if (onPresetSelected)
                        onPresetSelected(result - 1);
                }
            });
    }
};

// ==============================================================================
// Main Plugin Audio Processor Editor
// ==============================================================================
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
    void updateModeVisibility();

    KeshaZeddSynthAudioProcessor& audioProcessor;
    ModernSynthLookAndFeel lookAndFeel;

    // Dual-Mode State: false = PRODUCER MODE (Simple), true = ADVANCED SOUND DESIGNER
    bool isAdvancedMode = false;

    // ----------------------------------------------------
    // HEADER BAR CONTROLS
    // ----------------------------------------------------
    juce::TextButton producerModeButton;
    juce::TextButton advancedModeButton;

    CustomPresetComboBox presetBox;
    juce::Label presetLabel;
    juce::TextButton prevPresetButton;
    juce::TextButton nextPresetButton;

    juce::TextButton savePresetButton;
    juce::TextButton loadPresetButton;
    juce::TextButton diceButton;
    juce::TextButton mutateButton;

    juce::TextButton zeddifyButton;
    juce::ComboBox zeddifyStyleBox;
    juce::TextButton autoMasterButton;
    juce::ComboBox themeBox;

    juce::Slider masterVolSlider;
    juce::Label masterVolLabel;
    VUMeterComponent vuMeter;
    juce::Label voiceCountLabel;

    // Drag-and-drop MIDI Buttons
    DragMidiButton dragMidiButton;
    DragMidiButton dragChordButton;
    DragMidiButton dragHookButton;
    DragMidiButton dragVaultButton;

    // ----------------------------------------------------
    // PRODUCER MODE (SIMPLE) COMPONENTS
    // ----------------------------------------------------
    juce::Slider beachPunchSlider;
    juce::Label beachPunchLabel;

    juce::Slider festivalFilterSlider;
    juce::Label festivalFilterLabel;

    juce::Slider sidechainPumpSlider;
    juce::Label sidechainPumpLabel;

    juce::Slider spaceReverbSlider;
    juce::Label spaceReverbLabel;

    juce::Slider echoDelaySlider;
    juce::Label echoDelayLabel;

    juce::Slider stereoWideSlider;
    juce::Label stereoWideLabel;

    juce::Slider glossAirSlider;
    juce::Label glossAirLabel;

    juce::Slider warmthDriveSlider;
    juce::Label warmthDriveLabel;

    // Instant Festival Quick-FX Toggles
    juce::ToggleButton phaserQuickToggle;
    juce::ToggleButton glitterQuickToggle;
    juce::ToggleButton trashQuickToggle;
    juce::ToggleButton monoMakerQuickToggle;

    // 4 Momentary Performance Trigger Pads
    std::unique_ptr<MomentaryPadButton> tapeStopPad;
    std::unique_ptr<MomentaryPadButton> stutterPad;
    std::unique_ptr<MomentaryPadButton> divePad;
    std::unique_ptr<MomentaryPadButton> reversePad;

    // ----------------------------------------------------
    // ADVANCED SOUND DESIGNER MODE COMPONENTS
    // ----------------------------------------------------
    // Bay 1: Oscillators & Voice
    juce::Slider osc1ShapeSlider;
    juce::Label osc1ShapeLabel;
    juce::ComboBox osc1OctaveBox;
    juce::Label osc1OctaveLabel;
    juce::Slider unisonDetuneSlider;
    juce::Label unisonDetuneLabel;
    juce::Slider subLevelSlider;
    juce::Label subLevelLabel;
    juce::Slider filterCutoffSlider;
    juce::Label filterCutoffLabel;
    juce::Slider filterResSlider;
    juce::Label filterResLabel;
    juce::ComboBox filterModeBox;
    juce::Label filterModeLabel;
    juce::ComboBox layerBTypeBox;
    juce::Label layerBTypeLabel;
    juce::Slider layerBMixSlider;
    juce::Label layerBMixLabel;

    // Interactive FL 6-Stage Envelope
    FLEnvelopeDisplayComponent envDisplay;
    juce::Slider envDelaySlider;
    juce::Label envDelayLabel;
    juce::Slider ampAttackSlider;
    juce::Label ampAttackLabel;
    juce::Slider envHoldSlider;
    juce::Label envHoldLabel;
    juce::Slider ampDecaySlider;
    juce::Label ampDecayLabel;
    juce::Slider ampSustainSlider;
    juce::Label ampSustainLabel;
    juce::Slider ampReleaseSlider;
    juce::Label ampReleaseLabel;
    juce::Slider envDecTensionSlider;
    juce::Label envDecTensionLabel;
    juce::Slider envRelTensionSlider;
    juce::Label envRelTensionLabel;

    // Bay 2: Performance, Pitch, Echo Suite
    juce::Slider echoFeedSlider;
    juce::Label echoFeedLabel;
    juce::Slider echoTimeSlider;
    juce::Label echoTimeLabel;
    juce::Slider echoPanSlider;
    juce::Label echoPanLabel;
    juce::Slider echoPitchSlider;
    juce::Label echoPitchLabel;
    juce::Slider echoCountSlider;
    juce::Label echoCountLabel;
    juce::ToggleButton echoPingPongToggle;
    juce::ToggleButton echoFatToggle;

    juce::Slider timeShiftSlider;
    juce::Label timeShiftLabel;
    juce::ToggleButton cutSelfToggle;
    juce::ToggleButton slideToggle;
    juce::Slider glideTimeSlider;
    juce::Label glideTimeLabel;

    juce::ComboBox chordProgBox;
    juce::Label chordProgLabel;
    juce::ComboBox harmonizerBox;
    juce::Label harmonizerLabel;
    juce::ComboBox autoBassBox;
    juce::Label autoBassLabel;

    juce::ToggleButton hookGenToggle;
    juce::ComboBox hookMoodBox;
    juce::TextButton generateHookButton;
    juce::ComboBox midiVaultBox;
    juce::Label midiVaultLabel;
    juce::ToggleButton easyKeyToggle;
    juce::ToggleButton counterMelodyToggle;

    juce::Slider macroDropSlider;
    juce::Label macroDropLabel;
    juce::Slider punchSlider;
    juce::Label punchLabel;
    juce::Slider humanizeSlider;
    juce::Label humanizeLabel;
    juce::ComboBox scaleRootBox;
    juce::Label scaleRootLabel;
    juce::ComboBox scaleTypeBox;
    juce::Label scaleTypeLabel;

    // Bay 3: Detailed Effects & Full Phaser Controls
    juce::Slider phaserRateSlider;
    juce::Label phaserRateLabel;
    juce::Slider phaserDepthSlider;
    juce::Label phaserDepthLabel;
    juce::Slider phaserFeedbackSlider;
    juce::Label phaserFeedbackLabel;
    juce::Slider phaserMixSlider;
    juce::Label phaserMixLabel;

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
    juce::Slider glitterMixSlider;
    juce::Label glitterMixLabel;
    juce::Slider glitterGrainSlider;
    juce::Label glitterGrainLabel;

    juce::ComboBox producerFlavorBox;
    juce::Label producerFlavorLabel;
    juce::Slider producerFlavorIntensitySlider;
    juce::Label producerFlavorIntensityLabel;

    juce::ToggleButton pumpToggle;
    juce::ToggleButton monoMakerToggle;

    // ----------------------------------------------------
    // PARAMETER ATTACHMENTS
    // ----------------------------------------------------
    std::unique_ptr<ComboBoxAttachment> presetAttachment;
    std::unique_ptr<SliderAttachment> masterVolAttachment;
    std::unique_ptr<ButtonAttachment> zeddifyAttachment;
    std::unique_ptr<ComboBoxAttachment> zeddifyStyleAttachment;
    std::unique_ptr<ButtonAttachment> autoMasterAttachment;
    std::unique_ptr<ComboBoxAttachment> themeAttachment;
    std::unique_ptr<ComboBoxAttachment> producerFlavorAttachment;
    std::unique_ptr<SliderAttachment> producerFlavorIntensityAttachment;

    // Simple Mode Attachments
    std::unique_ptr<SliderAttachment> beachPunchAttachment;
    std::unique_ptr<SliderAttachment> festivalFilterAttachment;
    std::unique_ptr<SliderAttachment> sidechainPumpAttachment;
    std::unique_ptr<SliderAttachment> spaceReverbAttachment;
    std::unique_ptr<SliderAttachment> echoDelayAttachment;
    std::unique_ptr<SliderAttachment> stereoWideAttachment;
    std::unique_ptr<SliderAttachment> glossAirAttachment;
    std::unique_ptr<SliderAttachment> warmthDriveAttachment;

    std::unique_ptr<ButtonAttachment> phaserQuickAttachment;
    std::unique_ptr<ButtonAttachment> monoMakerQuickAttachment;

    // Advanced Mode Attachments
    std::unique_ptr<SliderAttachment> osc1ShapeAttachment;
    std::unique_ptr<ComboBoxAttachment> osc1OctaveAttachment;
    std::unique_ptr<SliderAttachment> unisonDetuneAttachment;
    std::unique_ptr<SliderAttachment> subLevelAttachment;
    std::unique_ptr<SliderAttachment> filterCutoffAttachment;
    std::unique_ptr<SliderAttachment> filterResAttachment;
    std::unique_ptr<SliderAttachment> layerBMixAttachment;
    std::unique_ptr<ComboBoxAttachment> filterModeAttachment;
    std::unique_ptr<ComboBoxAttachment> layerBTypeAttachment;

    std::unique_ptr<SliderAttachment> envDelayAttachment;
    std::unique_ptr<SliderAttachment> ampAttackAttachment;
    std::unique_ptr<SliderAttachment> envHoldAttachment;
    std::unique_ptr<SliderAttachment> ampDecayAttachment;
    std::unique_ptr<SliderAttachment> ampSustainAttachment;
    std::unique_ptr<SliderAttachment> ampReleaseAttachment;
    std::unique_ptr<SliderAttachment> envDecTensionAttachment;
    std::unique_ptr<SliderAttachment> envRelTensionAttachment;

    std::unique_ptr<SliderAttachment> echoFeedAttachment;
    std::unique_ptr<SliderAttachment> echoTimeAttachment;
    std::unique_ptr<SliderAttachment> echoPanAttachment;
    std::unique_ptr<SliderAttachment> echoPitchAttachment;
    std::unique_ptr<SliderAttachment> echoCountAttachment;
    std::unique_ptr<ButtonAttachment> echoPingPongAttachment;
    std::unique_ptr<ButtonAttachment> echoFatAttachment;

    std::unique_ptr<SliderAttachment> timeShiftAttachment;
    std::unique_ptr<ButtonAttachment> cutSelfAttachment;
    std::unique_ptr<SliderAttachment> glideTimeAttachment;

    std::unique_ptr<ComboBoxAttachment> chordProgAttachment;
    std::unique_ptr<ComboBoxAttachment> harmonizerAttachment;
    std::unique_ptr<ComboBoxAttachment> autoBassAttachment;
    std::unique_ptr<ButtonAttachment> hookGenAttachment;
    std::unique_ptr<ComboBoxAttachment> hookMoodAttachment;
    std::unique_ptr<ButtonAttachment> easyKeyAttachment;
    std::unique_ptr<ButtonAttachment> counterMelodyAttachment;
    std::unique_ptr<SliderAttachment> humanizeAttachment;

    std::unique_ptr<SliderAttachment> macroDropAttachment;
    std::unique_ptr<SliderAttachment> punchAttachment;
    std::unique_ptr<ComboBoxAttachment> scaleRootAttachment;
    std::unique_ptr<ComboBoxAttachment> scaleTypeAttachment;

    std::unique_ptr<SliderAttachment> phaserRateAttachment;
    std::unique_ptr<SliderAttachment> phaserDepthAttachment;
    std::unique_ptr<SliderAttachment> phaserFeedbackAttachment;
    std::unique_ptr<SliderAttachment> phaserMixAttachment;

    std::unique_ptr<SliderAttachment> fxDriveAttachment;
    std::unique_ptr<SliderAttachment> fxChorusMixAttachment;
    std::unique_ptr<SliderAttachment> fxDelayTimeAttachment;
    std::unique_ptr<SliderAttachment> fxDelayMixAttachment;
    std::unique_ptr<SliderAttachment> fxReverbDecayAttachment;
    std::unique_ptr<SliderAttachment> fxReverbMixAttachment;
    std::unique_ptr<SliderAttachment> analogDriftAttachment;
    std::unique_ptr<SliderAttachment> glitterMixAttachment;
    std::unique_ptr<SliderAttachment> glitterGrainAttachment;
    std::unique_ptr<ButtonAttachment> pumpAttachment;
    std::unique_ptr<ButtonAttachment> monoMakerAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeshaZeddSynthAudioProcessorEditor)
};
