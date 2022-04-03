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
//class GuitarAmpGUIAudioProcessor  : public  foleys::MagicProcessor,
//private juce::AudioProcessorValueTreeState::Listener
//{
class GuitarAmpGUIAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    GuitarAmpGUIAudioProcessor();
    ~GuitarAmpGUIAudioProcessor() override;
    

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        // Input Gain Params
        layout.add(std::make_unique<juce::AudioParameterFloat>("inputGain", "Input", juce::NormalisableRange<float>(-20, 40, 0.1, 1), 0));

        // Noise Gate Params
        layout.add(std::make_unique<juce::AudioParameterFloat>("gateThreshold", "Threshold", juce::NormalisableRange<float>(-80.f, 0.f), -80.f,"dB"));
        layout.add(std::make_unique<juce::AudioParameterFloat>("gateRatio", "Ratio", juce::NormalisableRange<float>(1.0001f, 40.f), 2.f,""));
        layout.add(std::make_unique<juce::AudioParameterFloat>("gateAttack", "Attack", juce::NormalisableRange<float>(0.f, 1000.f), 25.f,"ms"));
        layout.add(std::make_unique<juce::AudioParameterFloat>("gateRelease", "Release", juce::NormalisableRange<float>(0.f, 1000.f), 25.f,"ms"));

        // EQ Params
        juce::StringArray stringArray;
        for (int i = 0; i < 4; i++)
        {
            juce::String buildStr;
            buildStr << (12 + (i * 12));
            buildStr << " dB/Oct";
            stringArray.add(buildStr);
        }
        layout.add(std::make_unique<juce::AudioParameterChoice>("LowCutSlope", "LowCutSlope", stringArray, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>("LowCutFreq", "LowCutFreq", juce::NormalisableRange<float>(20, 20000, 1, 0.25), 20));
        layout.add(std::make_unique<juce::AudioParameterChoice>("HighCutSlope", "HighCutSlope", stringArray, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>("HighCutFreq", "HighCutFreq", juce::NormalisableRange<float>(20, 20000, 1, 0.25), 20000));
        layout.add(std::make_unique<juce::AudioParameterFloat>("PeakFreq", "PeakFreq", juce::NormalisableRange<float>(20, 20000, 1, 0.25), 750));
        layout.add(std::make_unique<juce::AudioParameterFloat>("PeakGain", "PeakGain", juce::NormalisableRange<float>(-24, 24, 0.5, 1), 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>("PeakQ", "PeakQ", juce::NormalisableRange<float>(0.1, 10, 0.05, 1), 1));

        // Amplifier Params
        // drive, mix, distortiontype
        // preLP freq, pre HP freq, pre peak freq
        // post lp freq, post hp freq, post peak freq
        layout.add(std::make_unique<juce::AudioParameterFloat>("ampDrive", "Drive", juce::NormalisableRange<float>(0.f, 40.f), 20.f,""));
        layout.add(std::make_unique<juce::AudioParameterFloat>("ampBlend", "Blend", juce::NormalisableRange<float>(0.f, 100.f), 100.f,"%"));
        juce::StringArray strArray ( {"None", "Tanh", "ATan", "Hard Clipper", "Rectifier", "Sine", "Tube"});
        layout.add(std::make_unique<juce::AudioParameterChoice>("ampType", "Distortion Type", strArray, 1));
        layout.add(std::make_unique<juce::AudioParameterFloat>("ampPreLPF", "Pre-Amp LPF", juce::NormalisableRange<float>(20, 20000, 1, 0.5), 20000.f, "Hz"));
        layout.add(std::make_unique<juce::AudioParameterFloat>("ampPreHPF", "Pre-Amp HPF", juce::NormalisableRange<float>(20, 20000, 1, 0.5), 20.f, "Hz"));
        layout.add(std::make_unique<juce::AudioParameterFloat>("ampEmphasis", "Emphasis Frequency", juce::NormalisableRange<float>(20, 20000, 1, 0.5), 700.f, "Hz"));
        layout.add(std::make_unique<juce::AudioParameterFloat>("ampEmphasisGain", "Emphasis Filter Gain", juce::NormalisableRange<float>(-20.f, 20.f), 0.f,"dB"));
        layout.add(std::make_unique<juce::AudioParameterFloat>("ampPostLPF", "Post-Amp LPF", juce::NormalisableRange<float>(20, 20000, 1, 0.5), 20000.f, "Hz"));
        layout.add(std::make_unique<juce::AudioParameterFloat>("ampPostHPF", "Post-Amp HPF", juce::NormalisableRange<float>(20, 20000, 1, 0.5), 20.f, "Hz"));

        // Reverb Params
        layout.add(std::make_unique<juce::AudioParameterFloat>("rvbBlend", "Wet/Dry", juce::NormalisableRange<float>(0.f, 1.f), 0.25f,"%"));
        layout.add(std::make_unique<juce::AudioParameterFloat>("rvbRoomSize", "Room Size", juce::NormalisableRange<float>(0.f, 1.f), 0.2f,""));
        layout.add(std::make_unique<juce::AudioParameterFloat>("rvbDamping", "Damping", juce::NormalisableRange<float>(0.f, 1.f), 0,""));

        // Compressor Params
        layout.add(std::make_unique<juce::AudioParameterFloat>("compThreshold", "Threshold", juce::NormalisableRange<float>(-40.f, 0.f), 0.f,"dB"));
        layout.add(std::make_unique<juce::AudioParameterFloat>("compRatio", "Ratio", juce::NormalisableRange<float>(1.0001f, 40.f), 2.f,""));
        layout.add(std::make_unique<juce::AudioParameterFloat>("compAttack", "Attack", juce::NormalisableRange<float>(0.f, 1000.f), 25.f,"ms"));
        layout.add(std::make_unique<juce::AudioParameterFloat>("compRelease", "Release", juce::NormalisableRange<float>(0.f, 1000.f), 25.f,"ms"));
        layout.add(std::make_unique<juce::AudioParameterFloat>("compMakeupGain", "Makeup Gain", juce::NormalisableRange<float>(0.f, 40.f), 0.f,"dB"));

        // Phaser Params
        layout.add(std::make_unique<juce::AudioParameterFloat>("phaserRate", "Rate", juce::NormalisableRange<float>(0.1f, 20.f), 0.f,"Hz"));
        layout.add(std::make_unique<juce::AudioParameterFloat>("phaserDepth", "Depth", juce::NormalisableRange<float>(0.f, 1.f), 0.25f,""));
        layout.add(std::make_unique<juce::AudioParameterFloat>("phaserFc", "Centre Frequency", juce::NormalisableRange<float>(0.f, 1000.f, 1, 0.25), 25.f,"Hz"));
        layout.add(std::make_unique<juce::AudioParameterFloat>("phaserFeedback", "Feedback", juce::NormalisableRange<float>(-1.f, 1.f), 0.f,"%"));
        layout.add(std::make_unique<juce::AudioParameterFloat>("phaserBlend", "Wet/Dry", juce::NormalisableRange<float>(0.f, 1.f), 0.25f,"%"));

        // Output Gain Params
        layout.add(std::make_unique<juce::AudioParameterFloat>("outputGain", "Output", juce::NormalisableRange<float>(-20, 40, 0.1, 1), 0));
        return layout;
    }


    juce::AudioProcessorValueTreeState apTreeState {*this, nullptr, "Parameters", createParameterLayout()};
    
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

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarAmpGUIAudioProcessor)
};
