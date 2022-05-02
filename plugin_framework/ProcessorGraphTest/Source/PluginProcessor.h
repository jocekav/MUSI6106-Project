/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
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

    //enum nodeIndex
    //{
    //    InputGainNodeIndex,
    //    NoiseGateNodeIndex,
    //    //EqNodeIndex,
    //    CompressorNodeIndex,
    //    AmpNodeIndex,
    //    ReverbNodeIndex,
    //    PhaserNodeIndex,
    //    OutputGainNodeIndex
    //};

    Node::Ptr inputGainNode;
    Node::Ptr noisegateNode;
    Node::Ptr eqNode;
    Node::Ptr compressorNode;
    Node::Ptr delayNode;
    Node::Ptr reverbNode;
    Node::Ptr phaserNode;
    Node::Ptr outputGainNode;

    Node::Ptr AmpInterfaceNode;
//    Node::Ptr CabSimNode;
        ;


    juce::AudioProcessorValueTreeState apvts;
private:
    //==============================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    std::unique_ptr<juce::AudioProcessorGraph> mainProcessor;

    enum ampNode
    {
        EmptyIndex,
        WaveshaperIndex,
        AnalogAmpIndex,
        SGAIndex
    };


    //juce::AudioParameterChoice* AmpSlot = (new juce::AudioParameterChoice("slot1", "Slot 1", processorChoices, 0));

    void initialiseAudioNodes(juce::ReferenceCountedArray<juce::AudioProcessorGraph::Node>& audioNodeList);
    void connectAudioNodes(juce::ReferenceCountedArray<juce::AudioProcessorGraph::Node>& audioNodeList);
    void connectMidiNodes();
    void initialiseGraph();
    void updateGraph();


    juce::AudioProcessorGraph::Node::Ptr midiInputNode;
    juce::AudioProcessorGraph::Node::Ptr midiOutputNode;

    Node::Ptr audioInputNode;
    Node::Ptr audioOutputNode;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProcessorGraphTestAudioProcessor)
};
