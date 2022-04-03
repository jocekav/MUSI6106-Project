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
    
//    foleys::MagicProcessorState magicState { *this, apTreeState};

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

        (*dynamic_cast<CGainProcessor*>(inputGainNode -> getProcessor())).updateParams(apTreeState.getRawParameterValue("inputGain")-> load());
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
        (*dynamic_cast<CAmplifierIf*>(amplifierNode->getProcessor())).updateParams(
                apTreeState.getRawParameterValue("ampDrive")->load(),
                apTreeState.getRawParameterValue("ampBlend")->load(),
                apTreeState.getRawParameterValue("ampType")->load(),
                apTreeState.getRawParameterValue("ampPreLPF")->load(),
                apTreeState.getRawParameterValue("ampPreHPF")->load(),
                apTreeState.getRawParameterValue("ampEmphasis")->load(),
                apTreeState.getRawParameterValue("ampEmphasisGain")->load(),
                apTreeState.getRawParameterValue("ampPostLPF")->load(),
                apTreeState.getRawParameterValue("ampPostHPF")->load()
                );



        (*dynamic_cast<CGainProcessor*>(outputGainNode -> getProcessor())).updateParams(apTreeState.getRawParameterValue("outputGain")-> load());
    }

    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        mainProcessor->setPlayConfigDetails (getMainBusNumInputChannels(),
                                             getMainBusNumOutputChannels(),
                                             sampleRate, samplesPerBlock);
        mainProcessor->prepareToPlay (sampleRate, samplesPerBlock);
        mainProcessor->enableAllBuses();
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
//    juce::AudioProcessorEditor* createEditor() override          { return new juce::GenericAudioProcessorEditor (*this); }
//    bool hasEditor() const override                              { return true; }

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
        inputGainNode = mainProcessor->addNode (std::make_unique<CGainProcessor>());
        reverbNode = mainProcessor->addNode(std::make_unique<CReverbProcessor>());
        compressorNode = mainProcessor->addNode(std::make_unique<CCompressorProcessor>());
        outputGainNode = mainProcessor->addNode(std::make_unique<CGainProcessor>());
        amplifierNode = mainProcessor->addNode(std::make_unique<CAmplifierIf>());
        
        for (int channel = 0; channel < 2; ++channel)
        {
            addConnectionToNode(audioInputNode->nodeID, inputGainNode->nodeID, channel);
            addConnectionToNode(inputGainNode->nodeID, amplifierNode->nodeID, channel);
            addConnectionToNode(amplifierNode->nodeID, eqNode->nodeID, channel);
            addConnectionToNode(eqNode->nodeID, compressorNode->nodeID, channel);
            addConnectionToNode(compressorNode->nodeID, reverbNode->nodeID, channel);
            addConnectionToNode(reverbNode->nodeID, outputGainNode->nodeID, channel);
            addConnectionToNode(outputGainNode->nodeID, audioOutputNode->nodeID, channel);
        }
        inputGainNode->getProcessor()->setPlayConfigDetails(getMainBusNumInputChannels(), getMainBusNumOutputChannels(), getSampleRate(), getBlockSize());
        amplifierNode->getProcessor()->setPlayConfigDetails(getMainBusNumInputChannels(), getMainBusNumOutputChannels(), getSampleRate(), getBlockSize());
        eqNode->getProcessor()->setPlayConfigDetails(getMainBusNumInputChannels(), getMainBusNumOutputChannels(), getSampleRate(), getBlockSize());
        compressorNode->getProcessor()->setPlayConfigDetails(getMainBusNumInputChannels(), getMainBusNumOutputChannels(), getSampleRate(), getBlockSize());
        reverbNode->getProcessor()->setPlayConfigDetails(getMainBusNumInputChannels(), getMainBusNumOutputChannels(), getSampleRate(), getBlockSize());
        outputGainNode->getProcessor()->setPlayConfigDetails(getMainBusNumInputChannels(), getMainBusNumOutputChannels(), getSampleRate(), getBlockSize());

        connectAudioNodes();
        connectMidiNodes();
        
        for (auto node : mainProcessor->getNodes())                 // [10]
            node->getProcessor()->enableAllBuses();
    }
    void addConnectionToNode(juce::AudioProcessorGraph::NodeID currentNodeId, juce::AudioProcessorGraph::NodeID nextNodeId, int channel)
    {
        mainProcessor->addConnection ({{currentNodeId, channel},{nextNodeId, channel}});
        mainProcessor->addConnection ({{currentNodeId, channel},{nextNodeId, channel}});
//        mainProcessor->addConnection ({ { midiInputNode->nodeID,         channel },
//                                        { nodeId, channel } });
//        mainProcessor->addConnection ({ { nodeId,  channel },
//                                        { midiOutputNode->nodeID,        channel } });
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
    Node::Ptr inputGainNode;
    Node::Ptr reverbNode;
    Node::Ptr compressorNode;
    Node::Ptr outputGainNode;
    Node::Ptr amplifierNode;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphProcessor)
};


#endif /* GraphProcessor_h */
