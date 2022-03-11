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
class EQ_v1AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    EQ_v1AudioProcessorEditor (EQ_v1AudioProcessor&);
    ~EQ_v1AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EQ_v1AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQ_v1AudioProcessorEditor)
};
