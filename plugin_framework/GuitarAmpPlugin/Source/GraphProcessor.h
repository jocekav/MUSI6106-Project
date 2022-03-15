//
//  GraphProcessor.h
//  GuitarAmpPlugin
//
//  Created by Jocelyn Kavanagh on 3/9/22.
//  Copyright © 2022 LL. All rights reserved.
//

#ifndef GraphProcessor_h
#define GraphProcessor_h

#include "EqProcessor.h"
#include "Effects.h"

class GraphProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    using AudioGraphIOProcessor = juce::AudioProcessorGraph::AudioGraphIOProcessor;
    using Node = juce::AudioProcessorGraph::Node;

    //==============================================================================
    GraphProcessor()
        : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                           .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
          mainProcessor  (new juce::AudioProcessorGraph())
    {
        
    }
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // EQ Params
        layout.add(std::make_unique<juce::AudioParameterFloat>("LowCutFreq", "LowCutFreq", juce::NormalisableRange<float>(20, 20000, 1, 0.25), 20));

        layout.add(std::make_unique<juce::AudioParameterFloat>("HighCutFreq", "HighCutFreq", juce::NormalisableRange<float>(20, 20000, 1, 0.25), 20000));
        
        layout.add(std::make_unique<juce::AudioParameterFloat>("PeakFreq", "PeakFreq", juce::NormalisableRange<float>(20, 20000, 1, 0.25), 750));

        layout.add(std::make_unique<juce::AudioParameterFloat>("PeakGain", "PeakGain", juce::NormalisableRange<float>(-24, 24, 0.5, 1), 0));

        layout.add(std::make_unique<juce::AudioParameterFloat>("PeakQ", "PeakQ", juce::NormalisableRange<float>(0.1, 10, 0.05, 1), 1));

        juce::StringArray stringArray;
        for (int i = 0; i < 4; i++)
        {
            juce::String buildStr;
            buildStr << (12 + (i * 12));
            buildStr << " db/Oct";
            stringArray.add(buildStr);
        }

        layout.add(std::make_unique<juce::AudioParameterChoice>("LowCutSlope", "LowCutSlope", stringArray, 0));

        layout.add(std::make_unique<juce::AudioParameterChoice>("HighCutSlope", "HighCutSlope", stringArray, 0));
        
        // Gain Params
        layout.add(std::make_unique<juce::AudioParameterFloat>("InputGain", "InputGain", juce::NormalisableRange<float>(-20, 40, 0.5, 1), 0));

        return layout;
    }
    
    
    juce::AudioProcessorValueTreeState apTreeState {*this, nullptr, "Parameters", createParameterLayout()};

    //==============================================================================
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override
    {
        if (layouts.getMainInputChannelSet()  == juce::AudioChannelSet::disabled()
         || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::disabled())
            return false;

        if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
         && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
            return false;

        return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet();
    }

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        mainProcessor->setPlayConfigDetails (getMainBusNumInputChannels(),
                                             getMainBusNumOutputChannels(),
                                             sampleRate, samplesPerBlock);

//        eqProcessor -> prepareToPlay(sampleRate, samplesPerBlock);
        
        mainProcessor->prepareToPlay (sampleRate, samplesPerBlock);

        initialiseGraph();
        (*dynamic_cast<EQ_v1AudioProcessor*>(eqNode -> getProcessor())). updateEqParams(
                                                                  apTreeState.getRawParameterValue("LowCutFreq") -> load(),
                                                                  apTreeState.getRawParameterValue("HighCutFreq") -> load(),
                                                                  apTreeState.getRawParameterValue("PeakFreq") -> load(),
                                                                  apTreeState.getRawParameterValue("PeakGain") -> load(),
                                                                  apTreeState.getRawParameterValue("PeakQ") -> load(),
                                                                  apTreeState.getRawParameterValue("LowCutSlope") -> load(),
                                                                  apTreeState.getRawParameterValue("HighCutSlope") -> load());
        
        (*dynamic_cast<CGainProcessor*>(gainNode -> getProcessor())). updateParam(apTreeState.getRawParameterValue("InputGain")-> load());
    }
    void releaseResources() override
    {
        mainProcessor->releaseResources();
    }

    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages) override
    {
        for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
            buffer.clear (i, 0, buffer.getNumSamples());
        mainProcessor->processBlock (buffer, midiMessages);
//        eqProcessor -> processBlock(buffer, midiMessages);
        (*dynamic_cast<EQ_v1AudioProcessor*>(eqNode -> getProcessor())). updateEqParams(
                                                                  apTreeState.getRawParameterValue("LowCutFreq") -> load(),
                                                                  apTreeState.getRawParameterValue("HighCutFreq") -> load(),
                                                                  apTreeState.getRawParameterValue("PeakFreq") -> load(),
                                                                  apTreeState.getRawParameterValue("PeakGain") -> load(),
                                                                  apTreeState.getRawParameterValue("PeakQ") -> load(),
                                                                  apTreeState.getRawParameterValue("LowCutSlope") -> load(),
                                                                  apTreeState.getRawParameterValue("HighCutSlope") -> load());
        
        (*dynamic_cast<CGainProcessor*>(gainNode -> getProcessor())).
        updateParam(apTreeState.getRawParameterValue("InputGain")-> load());
    }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override          { return new juce::GenericAudioProcessorEditor (*this); }
    bool hasEditor() const override                              { return true; }

    //==============================================================================
    const juce::String getName() const override                  { return "Graph Tutorial"; }
    bool acceptsMidi() const override                            { return true; }
    bool producesMidi() const override                           { return true; }
    double getTailLengthSeconds() const override                 { return 0; }

    //==============================================================================
    int getNumPrograms() override                                { return 1; }
    int getCurrentProgram() override                             { return 0; }
    void setCurrentProgram (int) override                        {}
    const juce::String getProgramName (int) override             { return {}; }
    void changeProgramName (int, const juce::String&) override   {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock&) override       {}
    void setStateInformation (const void*, int) override         {}

private:
    //==============================================================================
    void initialiseGraph()
    {
        mainProcessor->clear();

        audioInputNode  = mainProcessor->addNode (std::make_unique<AudioGraphIOProcessor> (AudioGraphIOProcessor::audioInputNode));
        audioOutputNode = mainProcessor->addNode (std::make_unique<AudioGraphIOProcessor> (AudioGraphIOProcessor::audioOutputNode));
        midiInputNode   = mainProcessor->addNode (std::make_unique<AudioGraphIOProcessor> (AudioGraphIOProcessor::midiInputNode));
        midiOutputNode  = mainProcessor->addNode (std::make_unique<AudioGraphIOProcessor> (AudioGraphIOProcessor::midiOutputNode));
        
        eqNode = mainProcessor->addNode (std::make_unique<EQ_v1AudioProcessor>());
        gainNode = mainProcessor->addNode (std::make_unique<CGainProcessor>());

        
        for (int channel = 0; channel < 2; ++channel)
        {
            mainProcessor->addConnection ({ { audioInputNode->nodeID,         channel },
                                            { eqNode->nodeID, channel } });
            mainProcessor->addConnection ({ { eqNode->nodeID,  channel },
                                            { audioOutputNode->nodeID,        channel } });
            mainProcessor->addConnection ({ { midiInputNode->nodeID,         channel },
                                            { eqNode->nodeID, channel } });
            mainProcessor->addConnection ({ { eqNode->nodeID,  channel },
                                            { midiOutputNode->nodeID,        channel } });
            
            mainProcessor->addConnection ({ { audioInputNode->nodeID,         channel },
                                            { gainNode->nodeID, channel } });
            mainProcessor->addConnection ({ { gainNode->nodeID,  channel },
                                            { audioOutputNode->nodeID,        channel } });
            mainProcessor->addConnection ({ { midiInputNode->nodeID,         channel },
                                            { gainNode->nodeID, channel } });
            mainProcessor->addConnection ({ { gainNode->nodeID,  channel },
                                            { midiOutputNode->nodeID,        channel } });
        }
        
        connectAudioNodes();
        connectMidiNodes();
        
        for (auto node : mainProcessor->getNodes())                 // [10]
            node->getProcessor()->enableAllBuses();
    }

    void connectAudioNodes()
    {
        for (int channel = 0; channel < 2; ++channel)
            mainProcessor->addConnection ({ { audioInputNode->nodeID,  channel },
                                            { audioOutputNode->nodeID, channel } });
    }

    void connectMidiNodes()
    {
        mainProcessor->addConnection ({ { midiInputNode->nodeID,  juce::AudioProcessorGraph::midiChannelIndex },
                                        { midiOutputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex } });
    }

    //==============================================================================

    std::unique_ptr<juce::AudioProcessorGraph> mainProcessor;

    Node::Ptr audioInputNode;
    Node::Ptr audioOutputNode;
    Node::Ptr midiInputNode;
    Node::Ptr midiOutputNode;
    
    Node::Ptr eqNode;
    Node::Ptr gainNode;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphProcessor)
};


#endif /* GraphProcessor_h */
