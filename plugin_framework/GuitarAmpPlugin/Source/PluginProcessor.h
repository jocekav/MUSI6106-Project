/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"



//==============================================================================
/**
*/
class GuitarAmpPluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    enum
    {
        AlgorithmNone = 0,
        AlgorithmTanh,
        AlgorithmAtan,
        AlgorithmHardClipper,
        AlgorithmRectifier,
        AlgorithmSine
    };

    //==============================================================================
    GuitarAmpPluginAudioProcessor();
    ~GuitarAmpPluginAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void reset() override;
    void releaseResources() override;

    void processBlock (juce::AudioSampleBuffer&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override { return JucePlugin_Name; }
    
    bool acceptsMidi() const override
    {
        #if JucePlugin_WantsMidiInput
            return true;
        #else
            return false;
        #endif
    }
    
    bool producesMidi() const override
    {
        #if JucePlugin_ProducesMidiOutput
            return true;
        #else
            return false;
        #endif
    }
    
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int /*index*/) override {}
    const juce::String getProgramName (int /*index*/) override { return juce::String();}
    void changeProgramName (int /*index*/, const juce::String& /*newName*/) override {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& /*destData*/) override {}
    void setStateInformation (const void* /*data*/, int /*sizeInBytes*/) override {}

    //==============================================================================
    juce::AudioParameterFloat *prmInput, *prmDrive, *prmMix, *prmOutput;
    juce::AudioParameterChoice *m_distortionType;

    juce::AudioParameterFloat *prmPreLP, *prmPreHP;
    juce::AudioParameterFloat *prmPostLP, *prmPostHP;

    juce::AudioParameterFloat *prmEPfreq;
    juce::AudioParameterFloat *prmEPgain;

private:
    //==============================================================================
    void updateProcessing();

    //==============================================================================
    bool isActive = false;

    juce::LinearSmoothedValue<float> inputVolume, outputVolume;
    juce::LinearSmoothedValue<float> driveVolume, dryVolume, wetVolume;

    using Filter = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    Filter preLowPassFilter, preHighPassFilter, postLowPassFilter, postHighPassFilter;
    Filter preEmphasisFilter, postEmphasisFilter;

    juce::AudioBuffer<float> mixBuffer;

    //==============================================================================
};

