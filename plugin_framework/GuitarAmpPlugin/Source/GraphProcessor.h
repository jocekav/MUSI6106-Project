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
          mainProcessor  (new juce::AudioProcessorGraph()),
//          eqProcessor    (new EQ_v1AudioProcessor()),
          muteInput      (new juce::AudioParameterBool   ("mute",    "Mute Input", true)),
          processorSlot1 (new juce::AudioParameterChoice ("slot1",   "Slot 1",     processorChoices, 0)),
          bypassSlot1    (new juce::AudioParameterBool   ("bypass1", "Bypass 1",   false))
    {
        addParameter (muteInput);

        addParameter (processorSlot1);

        addParameter (bypassSlot1);
    
    }
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // set lowest range to 20hz and highest to 20000hz and default to 20hz for low cut frequency
        layout.add(std::make_unique<juce::AudioParameterFloat>("LowCutFreq", "LowCutFreq", juce::NormalisableRange<float>(20, 20000, 1, 0.25), 20));

    // set lowest range to 20hz and highest to 20000hz and default to 20000hz for high cut frequency
        layout.add(std::make_unique<juce::AudioParameterFloat>("HighCutFreq", "HighCutFreq", juce::NormalisableRange<float>(20, 20000, 1, 0.25), 20000));

    // set lowest range to 20hz and highest to 20000hz and default to 20hz for peak frequency, adjust skew to sound better to user
        layout.add(std::make_unique<juce::AudioParameterFloat>("PeakFreq", "PeakFreq", juce::NormalisableRange<float>(20, 20000, 1, 0.25), 750));

    // set lowest range to -24 and highest to 24 and default to 0 for peak gain with 0.5 step size
        layout.add(std::make_unique<juce::AudioParameterFloat>("PeakGain", "PeakGain", juce::NormalisableRange<float>(-24, 24, 0.5, 1), 0));

    // set lowest range to 0.1 and highest to 10 and default to 2 for peak Q value with 0.05 step size
        layout.add(std::make_unique<juce::AudioParameterFloat>("PeakQ", "PeakQ", juce::NormalisableRange<float>(0.1, 10, 0.05, 1), 1));

    // filter choice uses string array and filter slopes will be multiples of 12 per db/octave
        juce::StringArray stringArray;
        for (int i = 0; i < 4; i++)
        {
            juce::String buildStr;
            buildStr << (12 + (i * 12));
            buildStr << " db/Oct";
            stringArray.add(buildStr);
        }

    // make lowcut slope params
        layout.add(std::make_unique<juce::AudioParameterChoice>("LowCutSlope", "LowCutSlope", stringArray, 0));

    // make highcut slope params
        layout.add(std::make_unique<juce::AudioParameterChoice>("HighCutSlope", "HighCutSlope", stringArray, 0));

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
    }

    void releaseResources() override
    {
        mainProcessor->releaseResources();
    }

    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages) override
    {
        for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
            buffer.clear (i, 0, buffer.getNumSamples());
        
        (*dynamic_cast<EQ_v1AudioProcessor*>(eqNode -> getProcessor())). updateEqParams(
                                                                  apTreeState.getRawParameterValue("LowCutFreq") -> load(),
                                                                  apTreeState.getRawParameterValue("HighCutFreq") -> load(),
                                                                  apTreeState.getRawParameterValue("PeakFreq") -> load(),
                                                                  apTreeState.getRawParameterValue("PeakGain") -> load(),
                                                                  apTreeState.getRawParameterValue("PeakQ") -> load(),
                                                                  apTreeState.getRawParameterValue("LowCutSlope") -> load(),
                                                                  apTreeState.getRawParameterValue("HighCutSlope") -> load());
        updateGraph();
        mainProcessor->processBlock (buffer, midiMessages);
//        eqProcessor -> processBlock(buffer, midiMessages);
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

        connectAudioNodes();
        connectMidiNodes();
        
        for (int channel = 0; channel < 2; ++channel)           // [9]
        {
            mainProcessor->addConnection ({ { audioInputNode->nodeID,         channel },
                                            { eqNode->nodeID, channel } });
            mainProcessor->addConnection ({ { eqNode->nodeID,  channel },
                                            { audioOutputNode->nodeID,        channel } });
        }
    }

    void updateGraph()
    {
        bool hasChanged = false;

        juce::Array<juce::AudioParameterChoice*> choices { processorSlot1};

        juce::Array<juce::AudioParameterBool*> bypasses { bypassSlot1};

        juce::ReferenceCountedArray<Node> slots;
        slots.add (slot1Node);
        

        for (int i = 0; i < 1; ++i)
        {
            auto& choice = choices.getReference (i);
            auto  slot   = slots  .getUnchecked (i);

            if (choice->getIndex() == 0)            // [1]
            {
                if (slot != nullptr)
                {
                    mainProcessor->removeNode (slot.get());
                    slots.set (i, nullptr);
                    hasChanged = true;
                }
            }
            else if (choice->getIndex() == 1)       // [2]
            {
                if (slot != nullptr)
                {
                    if (slot->getProcessor()->getName() == "EQ")
                        continue;

                    mainProcessor->removeNode (slot.get());
                }

//                slots.set (i, mainProcessor->addNode (eqProcessor));
                slots.set (i, eqNode);
//                mainProcessor -> addConnection(eqNode->nodeID, );
                
                hasChanged = true;
            }
        }

        if (hasChanged)
        {
            for (auto connection : mainProcessor->getConnections())     // [5]
                mainProcessor->removeConnection (connection);

            juce::ReferenceCountedArray<Node> activeSlots;

            for (auto slot : slots)
            {
                if (slot != nullptr)
                {
                    activeSlots.add (slot);                             // [6]

                    slot->getProcessor()->setPlayConfigDetails (getMainBusNumInputChannels(),
                                                                getMainBusNumOutputChannels(),
                                                                getSampleRate(), getBlockSize());
                }
            }

            if (activeSlots.isEmpty())                                  // [7]
            {
                connectAudioNodes();
            }
            else
            {
                for (int i = 0; i < activeSlots.size() - 1; ++i)        // [8]
                {
                    for (int channel = 0; channel < 2; ++channel)
                        mainProcessor->addConnection ({ { activeSlots.getUnchecked (i)->nodeID,      channel },
                                                        { activeSlots.getUnchecked (i + 1)->nodeID,  channel } });
                }

                for (int channel = 0; channel < 2; ++channel)           // [9]
                {
                    mainProcessor->addConnection ({ { audioInputNode->nodeID,         channel },
                                                    { activeSlots.getFirst()->nodeID, channel } });
                    mainProcessor->addConnection ({ { activeSlots.getLast()->nodeID,  channel },
                                                    { audioOutputNode->nodeID,        channel } });
                }
            }
            connectMidiNodes();

            for (auto node : mainProcessor->getNodes())                 // [10]
                node->getProcessor()->enableAllBuses();
        }

        for (int i = 0; i < 1; ++i)
        {
            auto  slot   = slots   .getUnchecked (i);
            auto& bypass = bypasses.getReference (i);

            if (slot != nullptr)
                slot->setBypassed (bypass->get());
        }

        audioInputNode->setBypassed (muteInput->get());

        slot1Node = slots.getUnchecked (0);
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
    juce::StringArray processorChoices { "Empty", "EQ"};

    std::unique_ptr<juce::AudioProcessorGraph> mainProcessor;
    
//    EQ_v1AudioProcessor* eqProcessor;

    juce::AudioParameterBool* muteInput;

    juce::AudioParameterChoice* processorSlot1;

    juce::AudioParameterBool* bypassSlot1;

    Node::Ptr audioInputNode;
    Node::Ptr audioOutputNode;
    Node::Ptr midiInputNode;
    Node::Ptr midiOutputNode;
    
    Node::Ptr eqNode;

    Node::Ptr slot1Node;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphProcessor)
};


#endif /* GraphProcessor_h */
