#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

// Interactive 2D X/Y Pad Component
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
        g.fillAll(juce::Colour(0xff12131a));
        
        g.setColour(juce::Colour(0xff232533));
        g.drawRoundedRectangle(getLocalBounds().toFloat(), 6.0f, 1.5f);
        
        // Draw grid lines
        g.setColour(juce::Colour(0xff232533).withAlpha(0.5f));
        float dashes[2] = { 4.0f, 4.0f };
        g.drawDashedLine(juce::Line<float>(getWidth() * 0.5f, 0.0f, getWidth() * 0.5f, getHeight()), dashes, 2);
        g.drawDashedLine(juce::Line<float>(0.0f, getHeight() * 0.5f, getWidth(), getHeight() * 0.5f), dashes, 2);
        
        // Grid Labels
        g.setColour(juce::Colour(0xff8c90b0).withAlpha(0.6f));
        g.setFont(10.0f);
        g.drawText("GRIT", 5, getHeight() / 2 - 12, 40, 10, juce::Justification::left);
        g.drawText("TRASH", getWidth() - 45, getHeight() / 2 - 12, 40, 10, juce::Justification::right);
        g.drawText("GLOSS", getWidth() / 2 - 20, 5, 40, 10, juce::Justification::centred);
        g.drawText("CLEAN", getWidth() / 2 - 20, getHeight() - 15, 40, 10, juce::Justification::centred);

        // Draw thumb position
        float thumbX = xPos * getWidth();
        float thumbY = (1.0f - yPos) * getHeight();
        
        // Neon glow gradient thumb
        juce::ColourGradient grad(juce::Colour(0xff00f2fe), thumbX, thumbY,
                                  juce::Colour(0xffff2a85), thumbX + 15.0f, thumbY + 15.0f, true);
        g.setGradientFill(grad);
        g.fillEllipse(thumbX - 9.0f, thumbY - 9.0f, 18.0f, 18.0f);
        g.setColour(juce::Colours::white);
        g.drawEllipse(thumbX - 9.0f, thumbY - 9.0f, 18.0f, 18.0f, 2.0f);
    }
    
    void mouseDown(const juce::MouseEvent& e) override
    {
        updatePosition(e.position);
    }
    
    void mouseDrag(const juce::MouseEvent& e) override
    {
        updatePosition(e.position);
    }
    
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

// Modern Dark-Mode Custom LookAndFeel
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
    void showLoadPresetDialog();

    KeshaZeddSynthAudioProcessor& audioProcessor;
    ModernSynthLookAndFeel lookAndFeel;

    // Header Controls
    juce::ComboBox presetBox;
    juce::Label presetLabel;
    juce::Slider masterVolSlider;
    juce::Label masterVolLabel;
    juce::Label voiceCountLabel;

    // Save, Load, and Dice buttons
    juce::TextButton diceButton;
    juce::TextButton savePresetButton;
    juce::TextButton loadPresetButton;

    // Oscillator 1 & Unison Controls
    juce::ComboBox osc1WaveBox;
    juce::Label osc1WaveLabel;
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

    // Oscillator 2 Controls
    juce::ComboBox osc2WaveBox;
    juce::Label osc2WaveLabel;
    juce::ComboBox osc2OctaveBox;
    juce::Label osc2OctaveLabel;
    juce::Slider osc2DetuneSlider;
    juce::Label osc2DetuneLabel;
    juce::Slider osc2LevelSlider;
    juce::Label osc2LevelLabel;

    // Filter Controls
    juce::ComboBox filterModeBox;
    juce::Label filterModeLabel;
    juce::Slider filterCutoffSlider;
    juce::Label filterCutoffLabel;
    juce::Slider filterResSlider;
    juce::Label filterResLabel;
    juce::Slider filterEnvAmtSlider;
    juce::Label filterEnvAmtLabel;

    // Filter ADSR Controls
    juce::Slider filterAttackSlider, filterDecaySlider, filterSustainSlider, filterReleaseSlider;
    juce::Label filterAttackLabel, filterDecayLabel, filterSustainLabel, filterReleaseLabel;

    // Amp ADSR Controls
    juce::Slider ampAttackSlider, ampDecaySlider, ampSustainSlider, ampReleaseSlider;
    juce::Label ampAttackLabel, ampDecayLabel, ampSustainLabel, ampReleaseLabel;

    // Post FX Controls
    juce::Slider fxDriveSlider;
    juce::Label fxDriveLabel;
    juce::Slider fxChorusRateSlider, fxChorusDepthSlider, fxChorusMixSlider;
    juce::Label fxChorusRateLabel, fxChorusDepthLabel, fxChorusMixLabel;
    juce::Slider fxDelayTimeSlider, fxDelayFeedbackSlider, fxDelayMixSlider;
    juce::Label fxDelayTimeLabel, fxDelayFeedbackLabel, fxDelayMixLabel;
    juce::Slider fxReverbMixSlider;
    juce::Label fxReverbMixLabel;

    // New Sound Generation / Setting Controls
    juce::Slider subLevelSlider;
    juce::Label subLevelLabel;
    juce::Slider formantMorphSlider;
    juce::Label formantMorphLabel;
    juce::Slider clickLevelSlider;
    juce::Label clickLevelLabel;
    juce::Slider glideTimeSlider;
    juce::Label glideTimeLabel;

    juce::ComboBox playModeBox;
    juce::Label playModeLabel;
    juce::ComboBox chordModeBox;
    juce::Label chordModeLabel;
    juce::ComboBox pumpActiveBox;
    juce::Label pumpActiveLabel;

    // Interactive 2D Pad (Trash vs Gloss)
    XYPad trashGlossPad;
    juce::Label trashGlossLabel;

    // Drop Master Wheel
    juce::Slider macroDropSlider;
    juce::Label macroDropLabel;

    // Macro Knobs
    juce::Slider punchSlider, gritSlider, spaceSlider, widthSlider;
    juce::Label punchLabel, gritLabel, spaceLabel, widthLabel;

    // APVTS Attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<ComboBoxAttachment> presetAttachment;
    std::unique_ptr<SliderAttachment> masterVolAttachment;

    std::unique_ptr<ComboBoxAttachment> osc1WaveAttachment, osc1OctaveAttachment, unisonCountAttachment;
    std::unique_ptr<SliderAttachment> osc1DetuneAttachment, osc1LevelAttachment, unisonDetuneAttachment;

    std::unique_ptr<ComboBoxAttachment> osc2WaveAttachment, osc2OctaveAttachment;
    std::unique_ptr<SliderAttachment> osc2DetuneAttachment, osc2LevelAttachment;

    std::unique_ptr<ComboBoxAttachment> filterModeAttachment;
    std::unique_ptr<SliderAttachment> filterCutoffAttachment, filterResAttachment, filterEnvAmtAttachment;
    std::unique_ptr<SliderAttachment> filterAttackAttachment, filterDecayAttachment, filterSustainAttachment, filterReleaseAttachment;

    std::unique_ptr<SliderAttachment> ampAttackAttachment, ampDecayAttachment, ampSustainAttachment, ampReleaseAttachment;

    std::unique_ptr<SliderAttachment> fxDriveAttachment;
    std::unique_ptr<SliderAttachment> fxChorusRateAttachment, fxChorusDepthAttachment, fxChorusMixAttachment;
    std::unique_ptr<SliderAttachment> fxDelayTimeAttachment, fxDelayFeedbackAttachment, fxDelayMixAttachment;
    std::unique_ptr<SliderAttachment> fxReverbMixAttachment;

    // New features attachments
    std::unique_ptr<SliderAttachment> subLevelAttachment;
    std::unique_ptr<SliderAttachment> formantMorphAttachment;
    std::unique_ptr<SliderAttachment> clickLevelAttachment;
    std::unique_ptr<SliderAttachment> glideTimeAttachment;
    std::unique_ptr<ComboBoxAttachment> playModeAttachment;
    std::unique_ptr<ComboBoxAttachment> chordModeAttachment;
    std::unique_ptr<ComboBoxAttachment> pumpActiveAttachment;

    std::unique_ptr<SliderAttachment> macroDropAttachment;
    std::unique_ptr<SliderAttachment> punchAttachment, gritAttachment, spaceAttachment, widthAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeshaZeddSynthAudioProcessorEditor)
};
