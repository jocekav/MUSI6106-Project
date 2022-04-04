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

//==============================================================================
/**
*/
class GuitarAmpGUIAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    GuitarAmpGUIAudioProcessorEditor (GuitarAmpGUIAudioProcessor&);
    ~GuitarAmpGUIAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void drawCompressor();
    void drawAmp();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GuitarAmpGUIAudioProcessor& audioProcessor;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    
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

    
    juce::Label effectTitleLabel;
    
    std::vector<juce::Component*> getNoiseGateComps();
    
    std::vector<juce::Component*> getAmpComps();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarAmpGUIAudioProcessorEditor)
};
