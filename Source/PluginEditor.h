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
        g.setColour(juce::Colour(0xff12161b));
        g.fillRoundedRectangle(bounds, 3.0f);

        g.setColour(juce::Colour(0xff242b36));
        g.drawRoundedRectangle(bounds, 3.0f, 1.0f);

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

        // Draw Envelope Fill
        juce::Path fillPath;
        fillPath.startNewSubPath(x0, y0);
        fillPath.lineTo(x1, y1);
        fillPath.lineTo(x2, y2);
        fillPath.lineTo(x3, y3);

        // Decay curve with tension
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

        g.setColour(juce::Colour(0xff55ee77).withAlpha(0.18f));
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

        g.setColour(juce::Colour(0xff66ff88));
        g.strokePath(strokePath, juce::PathStrokeType(1.6f));

        // Draw vertex nodes
        auto drawVertex = [&](float vx, float vy) {
            g.setColour(juce::Colour(0xff12161b));
            g.fillEllipse(vx - 3.0f, vy - 3.0f, 6.0f, 6.0f);
            g.setColour(juce::Colour(0xff88ffaa));
            g.drawEllipse(vx - 3.0f, vy - 3.0f, 6.0f, 6.0f, 1.2f);
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
        
        juce::Colour bg = isDown ? accentColour.withAlpha(0.35f) : (isHovered ? juce::Colour(0xff222533) : juce::Colour(0xff161822));
        g.setColour(bg);
        g.fillRoundedRectangle(bounds, 3.5f);

        juce::Colour border = isDown ? accentColour : (isHovered ? accentColour.withAlpha(0.6f) : juce::Colour(0xff2e3244));
        g.setColour(border);
        g.drawRoundedRectangle(bounds, 3.5f, isDown ? 1.5f : 1.0f);

        g.setColour(isDown ? juce::Colours::white : (isHovered ? juce::Colour(0xfff0f2f8) : juce::Colour(0xffa2a8be)));
        g.setFont(juce::FontOptions(8.5f, juce::Font::bold));
        g.drawText(text, bounds, juce::Justification::centred, false);
    }

    void mouseEnter(const juce::MouseEvent&) override { isHovered = true; repaint(); }
    void mouseExit(const juce::MouseEvent&) override { isHovered = false; isDown = false; if (callback) callback(false); repaint(); }
    void mouseDown(const juce::MouseEvent&) override { isDown = true; if (callback) callback(true); repaint(); }
    void mouseUp(const juce::MouseEvent&) override { isDown = false; if (callback) callback(false); repaint(); }

private:
    juce::String text;
    juce::Colour accentColour;
    std::function<void(bool)> callback;
    bool isHovered = false;
    bool isDown = false;
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
                                                       : "EXPORT";

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
    ExportType exportType = Zeddify;
    int vaultIndex = 0;
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
    PresetComboBox() { setEditableText(false); }
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

        // 8. Kesha "Blow" Studio Suite
        juce::PopupMenu blowMenu;
        blowMenu.addItem(47, "Blow - Dirty Squelch Bass");
        blowMenu.addItem(48, "Blow - Pumping Anthem Chords");
        blowMenu.addItem(49, "Blow - Screaming Laser Lead");
        blowMenu.addItem(50, "Blow - Staccato Party Pluck");
        blowMenu.addItem(51, "Blow - Glitter Shimmer FX");
        blowMenu.addItem(52, "Blow - Heavy Sub Smasher");
        menu.addSubMenu("08_Kesha 'Blow' Studio Suite", blowMenu);

        // 9. Zedd "Beautiful Now" Anthem Suite
        juce::PopupMenu zeddAnthemMenu;
        zeddAnthemMenu.addItem(53, "Beautiful Now - Euphoric Drop Chords");
        zeddAnthemMenu.addItem(54, "Beautiful Now - Soaring Festival Lead");
        zeddAnthemMenu.addItem(55, "Beautiful Now - Clockwork Bell Pluck");
        zeddAnthemMenu.addItem(56, "Beautiful Now - Rolling Progressive Bass");
        zeddAnthemMenu.addItem(57, "Beautiful Now - Cinematic Emotional Pad");
        zeddAnthemMenu.addItem(58, "Beautiful Now - Emotional Breakdown Piano");
        zeddAnthemMenu.addItem(59, "Beautiful Now - Stadium Punch Kick");
        zeddAnthemMenu.addItem(60, "Beautiful Now - White Noise Tension Sweep");
        menu.addSubMenu("09_Zedd 'Beautiful Now' Suite", zeddAnthemMenu);
        
        // 10. Flo Rida "Right Round" Club Suite
        juce::PopupMenu floRidaMenu;
        floRidaMenu.addItem(61, "Right Round - Club Pulse Synth Stab");
        floRidaMenu.addItem(62, "Right Round - Dirty Electro Slap Bass");
        floRidaMenu.addItem(63, "Right Round - Anthemic Octave Club Lead");
        floRidaMenu.addItem(64, "Right Round - Pumping Stadium Chords");
        floRidaMenu.addItem(65, "Right Round - Glitch Squelch Arp");
        floRidaMenu.addItem(66, "Right Round - 80s Retro Analog Brass");
        menu.addSubMenu("10_Flo Rida 'Right Round' Suite", floRidaMenu);

        // 11. Kevin Rudolf "Let It Rock" Pop-Rock Suite
        juce::PopupMenu kevinRudolfMenu;
        kevinRudolfMenu.addItem(67, "Let It Rock - Overdriven Power-Saw Riff");
        kevinRudolfMenu.addItem(68, "Let It Rock - Screaming Laser Lead");
        kevinRudolfMenu.addItem(69, "Let It Rock - High-Energy Synth Riser FX");
        kevinRudolfMenu.addItem(70, "Let It Rock - Pumping Electro-Rock Chords");
        kevinRudolfMenu.addItem(71, "Let It Rock - Distorted Gritty 808 Sub");
        kevinRudolfMenu.addItem(72, "Let It Rock - Stadium Impact Downlifter FX");
        menu.addSubMenu("11_Kevin Rudolf 'Let It Rock' Suite", kevinRudolfMenu);
        
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
                if (result >= 1 && result <= 72)
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

// LookAndFeel with 4 Theme Skins
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
    juce::TextButton mutateButton;
    
    DragMidiButton dragMidiButton;
    DragMidiButton dragChordButton;
    DragMidiButton dragHookButton;
    DragMidiButton dragVaultButton;

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
    // SECTION 2: FL STUDIO AHDSR ENVELOPE & ECHO/FAT MODE (Center Bay)
    // ----------------------------------------------------
    FLEnvelopeDisplayComponent envDisplay;

    // AHDSR + Tension Controls
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

    // FL Studio Echo Delay & Fat Mode
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

    // FL Studio Time Shift, Gate, Cut Self & Slide
    juce::Slider timeShiftSlider;
    juce::Label timeShiftLabel;
    juce::ToggleButton cutSelfToggle;
    juce::ToggleButton slideToggle;
    juce::Slider glideTimeSlider;
    juce::Label glideTimeLabel;

    // Songwriting Dropdowns
    juce::ComboBox chordProgBox;
    juce::Label chordProgLabel;
    juce::ComboBox harmonizerBox;
    juce::Label harmonizerLabel;
    juce::ComboBox autoBassBox;
    juce::Label autoBassLabel;

    // Hook Generator & MIDI Vault Controls
    juce::ToggleButton hookGenToggle;
    juce::ComboBox hookMoodBox;
    juce::TextButton generateHookButton;
    juce::ComboBox midiVaultBox;
    juce::Label midiVaultLabel;

    juce::ToggleButton easyKeyToggle;
    juce::ToggleButton counterMelodyToggle;

    // 4 Momentary Glitch Trigger Pads
    std::unique_ptr<MomentaryPadButton> tapeStopPad;
    std::unique_ptr<MomentaryPadButton> stutterPad;
    std::unique_ptr<MomentaryPadButton> divePad;
    std::unique_ptr<MomentaryPadButton> reversePad;

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

    // ----------------------------------------------------
    // SECTION 3: EFFECTS, SPACE & GLITTER CLOUD (Right Bay)
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

    // Parameter Attachments
    std::unique_ptr<ComboBoxAttachment> presetAttachment;
    std::unique_ptr<SliderAttachment> masterVolAttachment;
    std::unique_ptr<ButtonAttachment> zeddifyAttachment;
    std::unique_ptr<ComboBoxAttachment> zeddifyStyleAttachment;
    std::unique_ptr<ButtonAttachment> autoMasterAttachment;
    std::unique_ptr<ComboBoxAttachment> themeAttachment;
    std::unique_ptr<ComboBoxAttachment> producerFlavorAttachment;
    std::unique_ptr<SliderAttachment> producerFlavorIntensityAttachment;

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
