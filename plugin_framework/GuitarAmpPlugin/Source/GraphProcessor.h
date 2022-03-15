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

        // Noise Gate Params
        layout.add(std::make_unique<juce::AudioParameterFloat>("gateThreshold", "Threshold", juce::NormalisableRange<float>(-40.f, 0.f), 0.f,"dB"));
        layout.add(std::make_unique<juce::AudioParameterFloat>("gateRatio", "Ratio", juce::NormalisableRange<float>(1.0001f, 40.f), 2.f,""));
        layout.add(std::make_unique<juce::AudioParameterFloat>("gateAttack", "Attack", juce::NormalisableRange<float>(0.f, 1000.f), 25.f,"ms"));
        layout.add(std::make_unique<juce::AudioParameterFloat>("gateRelease", "Release", juce::NormalisableRange<float>(0.f, 1000.f), 25.f,"ms"));

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
        layout.add(std::make_unique<juce::AudioParameterFloat>("InputGain", "InputGain", juce::NormalisableRange<float>(-20, 40, 0.1, 1), 0));

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
        layout.add(std::make_unique<juce::AudioParameterFloat>("phaserRate", "Rate", juce::NormalisableRange<float>(0.f, 2000.f), 0.f,"Hz"));
        layout.add(std::make_unique<juce::AudioParameterFloat>("phaserDepth", "Depth", juce::NormalisableRange<float>(0.f, 1.f), 0.25f,""));
        layout.add(std::make_unique<juce::AudioParameterFloat>("phaserFc", "Centre Frequency", juce::NormalisableRange<float>(0.f, 1000.f, 1, 0.25), 25.f,"Hz"));
        layout.add(std::make_unique<juce::AudioParameterFloat>("phaserFeedback", "Feedback", juce::NormalisableRange<float>(-1.f, 1.f), 0.f,"%"));
        layout.add(std::make_unique<juce::AudioParameterFloat>("phaserBlend", "Wet/Dry", juce::NormalisableRange<float>(0.f, 1.f), 0.25f,"%"));

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

    void update()
    {
        (*dynamic_cast<EQ_v1AudioProcessor*>(eqNode -> getProcessor())). updateEqParams(
                apTreeState.getRawParameterValue("LowCutFreq") -> load(),
                apTreeState.getRawParameterValue("HighCutFreq") -> load(),
                apTreeState.getRawParameterValue("PeakFreq") -> load(),
                apTreeState.getRawParameterValue("PeakGain") -> load(),
                apTreeState.getRawParameterValue("PeakQ") -> load(),
                apTreeState.getRawParameterValue("LowCutSlope") -> load(),
                apTreeState.getRawParameterValue("HighCutSlope") -> load());

        (*dynamic_cast<CGainProcessor*>(gainNode -> getProcessor())).updateParams(apTreeState.getRawParameterValue("InputGain")-> load());
        (*dynamic_cast<CReverbProcessor*>(reverbNode->getProcessor())).updateParams(
                apTreeState.getRawParameterValue("rvbBlend")-> load(),
                apTreeState.getRawParameterValue("rvbRoomSize")-> load(),
                apTreeState.getRawParameterValue("rvbDamping")-> load()
        );
        (*dynamic_cast<CCompressorProcessor*>(compressorNode->getProcessor())).updateParams(
                apTreeState.getRawParameterValue("compThreshold")->load(),
                apTreeState.getRawParameterValue("compRatio")->load(),
                apTreeState.getRawParameterValue("compAttack")->load(),
                apTreeState.getRawParameterValue("compRelease")->load(),
                apTreeState.getRawParameterValue("compMakeupGain")->load()
                );
    }

    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        mainProcessor->setPlayConfigDetails (getMainBusNumInputChannels(),
                                             getMainBusNumOutputChannels(),
                                             sampleRate, samplesPerBlock);
        mainProcessor->prepareToPlay (sampleRate, samplesPerBlock);
        initialiseGraph();
        update();
    }
    void releaseResources() override
    {
        mainProcessor->releaseResources();
    }

    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages) override
    {
        for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
            buffer.clear (i, 0, buffer.getNumSamples());
        update();
        mainProcessor->processBlock (buffer, midiMessages);
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
        reverbNode = mainProcessor->addNode(std::make_unique<CReverbProcessor>());
        compressorNode = mainProcessor->addNode(std::make_unique<CCompressorProcessor>());

        
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

            mainProcessor->addConnection ({ { audioInputNode->nodeID,         channel },
                                            { reverbNode->nodeID, channel } });
            mainProcessor->addConnection ({ { reverbNode->nodeID,  channel },
                                            { audioOutputNode->nodeID,        channel } });
            mainProcessor->addConnection ({ { midiInputNode->nodeID,         channel },
                                            { reverbNode->nodeID, channel } });
            mainProcessor->addConnection ({ { reverbNode->nodeID,  channel },
                                            { midiOutputNode->nodeID,        channel } });

            mainProcessor->addConnection ({ { audioInputNode->nodeID,         channel },
                                            { compressorNode->nodeID, channel } });
            mainProcessor->addConnection ({ { compressorNode->nodeID,  channel },
                                            { audioOutputNode->nodeID,        channel } });
            mainProcessor->addConnection ({ { midiInputNode->nodeID,         channel },
                                            { compressorNode->nodeID, channel } });
            mainProcessor->addConnection ({ { compressorNode->nodeID,  channel },
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
    Node::Ptr reverbNode;
    Node::Ptr compressorNode;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphProcessor)
};


#endif /* GraphProcessor_h */
