/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class WaveNetVaAudioProcessorEditor  : public AudioProcessorEditor,
                                       private Button::Listener
                                
{
public:
    WaveNetVaAudioProcessorEditor (WaveNetVaAudioProcessor&);
    ~WaveNetVaAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    WaveNetVaAudioProcessor& processor;

    TextButton loadButton;
    juce::String fname;
    virtual void buttonClicked(Button* button) override;
    void loadButtonClicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveNetVaAudioProcessorEditor)
};
