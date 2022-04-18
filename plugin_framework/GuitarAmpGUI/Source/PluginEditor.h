/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider (juce::Graphics&,
                           int x, int y, int width, int height,
                           float sliderPosProportional,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           juce::Slider&) override;
    
    void drawToggleButton (juce::Graphics &g,
                           juce::ToggleButton &toggle,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override;
    
    void drawLinearSlider(juce::Graphics &,
                          int x, int y, int width, int height,
                          float     sliderPos,
                          float     minSliderPos,
                          float     maxSliderPos,
                          const juce::Slider::SliderStyle,
                          juce::Slider &) override;
};


struct CustomRotarySlider : juce::Slider
{
    CustomRotarySlider(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) : juce::Slider(SliderStyle::RotaryHorizontalVerticalDrag,
                                        juce::Slider::TextEntryBoxPosition::NoTextBox),
    param(&rap),
    suffix(unitSuffix)
    {
        setLookAndFeel(&lnf);
    }
    
    ~CustomRotarySlider()
    {
        setLookAndFeel(nullptr);
    }
    
    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    juce::String getDisplayString() const;
private:
    LookAndFeel lnf;
    
    juce::RangedAudioParameter* param;
    juce::String suffix;
};

struct CustomToggle : juce::ToggleButton
{
    CustomToggle(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) : juce::ToggleButton(),
    param(&rap),
    suffix(unitSuffix)
    {
        setLookAndFeel(&lnf);
    }
    
    ~CustomToggle()
    {
        setLookAndFeel(nullptr);
    }
    
    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    juce::String getDisplayString() const;
private:
    LookAndFeel lnf;
    
    juce::RangedAudioParameter* param;
    juce::String suffix;
};


//==============================================================================
/**
*/
class GuitarAmpGUIAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                            public juce::Button::Listener
{
public:
    GuitarAmpGUIAudioProcessorEditor (GuitarAmpGUIAudioProcessor&);
    ~GuitarAmpGUIAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked (juce::Button* button);
    void updateToggleState(juce::Button* button);
    void drawNoiseGate();
    void drawAmp();
    void drawReverb();
    void drawCompressor();
    void drawEQ();
    void drawPhaser();
    
    enum RadioButtonIDs
    {
        EffectShown = 1001
    };

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GuitarAmpGUIAudioProcessor& audioProcessor;
    
    // Effect chain buttons
    juce::TextButton switchEffectButton;
    
    juce::ToggleButton gateButton,
    ampButton,
    verbButton,
    compressorButton,
    eqButton,
    phaserButton;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    
    // Chain in and out gain
    CustomRotarySlider inputGainSlider,
    outputGainSlider;
//    juce::Slider inputGainSlider,
//    outputGainSlider;
    
    APVTS::SliderAttachment inputGainSliderAttachment,
    outputGainSliderAttachment;

    
    // Noise Gate Components
    CustomRotarySlider gateThresholdSlider,
    gateRatioSlider,
    gateAttackSlider,
    gateReleaseSlider;
    
    APVTS::SliderAttachment gateThresholdSliderAttachment,
    gateRatioSliderAttachment,
    gateAttackSliderAttachment,
    gateReleaseSliderAttachment;
    
    juce::Label gateThresholdSliderLabel,
    gateRatioSliderLabel,
    gateAttackSliderLabel,
    gateReleaseSliderLabel;
    
    CustomToggle gateBypassToggle;
    
    // Amp Components
    CustomRotarySlider ampDriveSlider,
    ampBlendSlider,
    ampPreLpfSlider,
    ampPreHpfSlider,
    ampEmphasisFreqSlider,
    ampEmphasisGainSlider,
    ampPostLpfSlider,
    ampPostHpfSlider;
    
    APVTS::SliderAttachment ampDriveSliderAttachment,
    ampBlendSliderAttachment,
    ampPreLpfSliderAttachment,
    ampPreHpfSliderAttachment,
    ampEmphasisFreqSliderAttachment,
    ampEmphasisGainSliderAttachment,
    ampPostLpfSliderAttachment,
    ampPostHpfSliderAttachment;
    
    juce::Label ampDriveSliderLabel,
    ampBlendSliderLabel,
    ampPreLpfSliderLabel,
    ampPreHpfSliderLabel,
    ampEmphasisFreqSliderLabel,
    ampEmphasisGainSliderLabel,
    ampPostLpfSliderLabel,
    ampPostHpfSliderLabel;
    
    juce::ComboBox ampTypeComboBox;
    APVTS::ComboBoxAttachment ampTypeComboBoxAttachment;
    
    CustomToggle ampBypassToggle;
    
    // Reverb Components
    CustomRotarySlider rvbBlendSlider,
    rvbRoomSizeSlider,
    rvbDampingSlider;

    APVTS::SliderAttachment rvbBlendSliderAttachment,
    rvbRoomSizeSliderAttachment,
    rvbDampingSliderAttachment;

    juce::Label rvbBlendSliderLabel,
    rvbRoomSizeSliderLabel,
    rvbDampingSliderLabel;
    
    CustomToggle verbBypassToggle;
    
    // Compessor Components
    CustomRotarySlider compThresholdSlider,
    compRatioSlider,
    compAttackSlider,
    compReleaseSlider,
    compMakeUpGainSlider;

    APVTS::SliderAttachment compThresholdSliderAttachment,
    compRatioSliderAttachment,
    compAttackSliderAttachment,
    compReleaseSliderAttachment,
    compMakeUpGainSliderAttachment;

    juce::Label compThresholdSliderLabel,
    compRatioSliderLabel,
    compAttackSliderLabel,
    compReleaseSliderLabel,
    compMakeUpGainSliderLabel;
    
    CustomToggle compBypassToggle;
    
    // EQ Components
    CustomRotarySlider eqLowCutFreqSlider,
    eqLowCutSlopeSlider,
    eqHighCutFreqSlider,
    eqHighCutSlopeSlider,
    eqPeakFreqSlider,
    eqPeakGainSlider,
    eqPeakQSlider;
    
    APVTS::SliderAttachment eqLowCutFreqSliderAttachment,
    eqLowCutSlopeSliderAttachment,
    eqHighCutFreqSliderAttachment,
    eqHighCutSlopeSliderAttachment,
    eqPeakFreqSliderAttachment,
    eqPeakGainSliderAttachment,
    eqPeakQSliderAttachment;
    
    juce::Label eqLowCutFreqSliderLabel,
    eqLowCutSlopeSliderLabel,
    eqHighCutFreqSliderLabel,
    eqHighCutSlopeSliderLabel,
    eqPeakFreqSliderLabel,
    eqPeakGainSliderLabel,
    eqPeakQSliderLabel;
    
    CustomToggle eqBypassToggle;
    
    // Phaser Components
    CustomRotarySlider phaserRateSlider,
    phaserDepthSlider,
    phaserFcSlider,
    phaserFeedbackSlider,
    phaserBlendSlider;
    
    APVTS::SliderAttachment phaserRateSliderAttachment,
    phaserDepthSliderAttachment,
    phaserFcSliderAttachment,
    phaserFeedbackSliderAttachment,
    phaserBlendSliderAttachment;
    
    juce::Label phaserRateSliderLabel,
    phaserDepthSliderLabel,
    phaserFcSliderLabel,
    phaserFeedbackSliderLabel,
    phaserBlendSliderLabel;
    
    CustomToggle phaserBypassToggle;
    
    
    juce::Label effectTitleLabel;
    
    std::vector<juce::Component*> getNoiseGateComps();
    
    std::vector<juce::Component*> getAmpComps();
    
    std::vector<juce::Component*> getReverbComps();
    
    std::vector<juce::Component*> getCompressorComps();
    
    std::vector<juce::Component*> getEqComps();
    
    std::vector<juce::Component*> getPhaserComps();
    
    std::vector<juce::Component*> getChainComps();
    
    LookAndFeel lnf;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarAmpGUIAudioProcessorEditor)
};


 
