/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Effects_new.h"

//==============================================================================
/**
*/
class ProcessorGraphTestAudioProcessor  : public juce::AudioProcessor
{
public:
    using AudioGraphIOProcessor = juce::AudioProcessorGraph::AudioGraphIOProcessor;
    using Node = juce::AudioProcessorGraph::Node;
    //==============================================================================
    ProcessorGraphTestAudioProcessor();
    ~ProcessorGraphTestAudioProcessor() override;

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

    Node::Ptr inputGainNode;
    Node::Ptr noisegateNode;
//    Node::Ptr eqNode;
//    Node::Ptr gainNode;
    Node::Ptr compressorNode;
//    Node::Ptr amplifierNode;
    Node::Ptr reverbNode;
    Node::Ptr phaserNode;
    Node::Ptr outputGainNode;


    // FOR FUTURE CODE TO CHANGE THE ORDER OF FX
    juce::StringArray processorChoices { "Empty", "Noise Gate", "EQ", "Compressor", "Reverb", "Phaser", "Gain", "Amplifier" };
    juce::AudioParameterChoice* processorSlot1;
    juce::AudioParameterChoice* processorSlot2;
    juce::AudioParameterChoice* processorSlot3;
    juce::AudioParameterChoice* processorSlot4;
    juce::AudioParameterChoice* processorSlot5;
    juce::AudioParameterChoice* processorSlot6;
    juce::AudioParameterChoice* processorSlot7;

    Node::Ptr node1;
    Node::Ptr node2;
    Node::Ptr node3;
    Node::Ptr node4;
    Node::Ptr node5;
    Node::Ptr node6;
    Node::Ptr node7;

    juce::AudioProcessorValueTreeState apvts;
private:
    //==============================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    std::unique_ptr<juce::AudioProcessorGraph> mainProcessor;

    void initialiseAudioNodes(juce::ReferenceCountedArray<juce::AudioProcessorGraph::Node>& audioNodeList);
    void connectAudioNodes(juce::ReferenceCountedArray<juce::AudioProcessorGraph::Node>& audioNodeList);
    void connectMidiNodes();
    void initialiseGraph();


    juce::AudioProcessorGraph::Node::Ptr midiInputNode;
    juce::AudioProcessorGraph::Node::Ptr midiOutputNode;

    juce::AudioProcessorGraph::Node::Ptr audioInputNode;
    juce::AudioProcessorGraph::Node::Ptr audioOutputNode;

    ProcessorBase effect1;
    ProcessorBase effect2;
    ProcessorBase effect3;
    ProcessorBase effect4;
    ProcessorBase effect5;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProcessorGraphTestAudioProcessor)
};
