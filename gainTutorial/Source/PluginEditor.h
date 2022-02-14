/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class GainTutorialAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                          public juce::Slider::Listener
{
public:
    GainTutorialAudioProcessorEditor (GainTutorialAudioProcessor&);
    ~GainTutorialAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void sliderValueChanged(juce::Slider* pSlider) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GainTutorialAudioProcessor& audioProcessor;
    
    juce::Slider mGainSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainTutorialAudioProcessorEditor)
};
