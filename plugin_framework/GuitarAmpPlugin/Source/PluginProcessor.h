/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class GuitarAmpPluginAudioProcessor  : public juce::AudioProcessor,
                                        public juce::ValueTree::Listener
{
public:
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
    ~GuitarAmpPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    
    void init(); // Call once -- set initial values for DSP
    void prepare(double sampleRate, int samplesPerBlock); // Pass sample rate and buffer size to DSP
    void update(); //update params when changed
    void reset() override; // reset dsp params

    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    
private:
    juce::AudioBuffer<float> mixBuffer;
    bool mustUpdateParams{false};
    bool isActive{false};
    
    // Member variables for params
//    float m_inputVolume {0.f};
//    float m_drive {0.f};
//    float m_blend {0.f};
//    float m_outputVolume {0.f};
//    float m_preLPF {0};
//    float m_preHPF {0};
//    float m_postLPF {0};
//    float m_postHPF {0};
    int m_distortionType {0};
    
    juce::LinearSmoothedValue<float> m_inputVolume, m_outputVolume, m_drive, m_blend,m_preLPF,m_preHPF,m_postLPF,m_postHPF {0.f};
    
    juce::IIRFilter preLPF[2], preHPF[2], postLPF[2], postHPF[2];
    
    void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override
    {
        mustUpdateParams = true;
        
    }
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarAmpPluginAudioProcessor)
};
