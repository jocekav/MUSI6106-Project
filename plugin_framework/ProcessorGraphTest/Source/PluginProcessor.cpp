/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
//#include "Effects_new.h"


//==============================================================================
ProcessorGraphTestAudioProcessor::ProcessorGraphTestAudioProcessor():
                    AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                               .withOutput ("Output", juce::AudioChannelSet::mono(), true)),
                    processorSlot1 (new juce::AudioParameterChoice ("slot1",   "Slot 1",     processorChoices, 0)),
                    processorSlot2 (new juce::AudioParameterChoice ("slot2",   "Slot 2",     processorChoices, 0)),
                    processorSlot3 (new juce::AudioParameterChoice ("slot3",   "Slot 3",     processorChoices, 0)),
                    processorSlot4 (new juce::AudioParameterChoice ("slot4",   "Slot 4",     processorChoices, 0)),
                    processorSlot5 (new juce::AudioParameterChoice ("slot5",   "Slot 5",     processorChoices, 0)),
                    processorSlot6 (new juce::AudioParameterChoice ("slot6",   "Slot 6",     processorChoices, 0)),
                    processorSlot7 (new juce::AudioParameterChoice ("slot7",   "Slot 7",     processorChoices, 0)),
                    apvts{*this, nullptr, "Parameters",createParameterLayout()},
                    mainProcessor(new juce::AudioProcessorGraph())
{
    addParameter(processorSlot1);
    addParameter(processorSlot2);
    addParameter(processorSlot3);
    addParameter(processorSlot4);
    addParameter(processorSlot5);
    addParameter(processorSlot6);
    addParameter(processorSlot7);
}

ProcessorGraphTestAudioProcessor::~ProcessorGraphTestAudioProcessor()
{
}

//==============================================================================
const juce::String ProcessorGraphTestAudioProcessor::getName() const
{
    return ProjectInfo::projectName;
}

bool ProcessorGraphTestAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ProcessorGraphTestAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ProcessorGraphTestAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ProcessorGraphTestAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ProcessorGraphTestAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ProcessorGraphTestAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ProcessorGraphTestAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ProcessorGraphTestAudioProcessor::getProgramName (int index)
{
    return {};
}

void ProcessorGraphTestAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ProcessorGraphTestAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    mainProcessor->setPlayConfigDetails(getMainBusNumInputChannels(),
                                        getMainBusNumOutputChannels(),
                                        sampleRate,
                                        samplesPerBlock);
    mainProcessor->prepareToPlay(sampleRate,samplesPerBlock);
    initialiseGraph();

    for (auto node : mainProcessor->getNodes())
    {
        node->getProcessor()->setPlayConfigDetails(getMainBusNumInputChannels(),
                                                   getMainBusNumOutputChannels(),
                                                   sampleRate, samplesPerBlock);
        node->getProcessor()->prepareToPlay(sampleRate, samplesPerBlock);
        node->getProcessor()->enableAllBuses();
    }
}

void ProcessorGraphTestAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ProcessorGraphTestAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ProcessorGraphTestAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    updateGraph();
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    mainProcessor->processBlock(buffer, midiMessages);
//    for (int channel = 0; channel < totalNumInputChannels; ++channel)
//    {
//        auto* channelData = buffer.getWritePointer (channel);
//
//        // ..do something to the data...
//    }
    for (auto channel = totalNumInputChannels; channel < totalNumOutputChannels; channel++)
    {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            channelData[sample] = juce::jlimit(-1.f,1.f, channelData[sample]);
        }

    }
}

//==============================================================================
bool ProcessorGraphTestAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ProcessorGraphTestAudioProcessor::createEditor()
{
//    return new ProcessorGraphTestAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void ProcessorGraphTestAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ProcessorGraphTestAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ProcessorGraphTestAudioProcessor();
}

//void ProcessorGraphTestAudioProcessor::connectAudioNodes(juce::ReferenceCountedArray<juce::AudioProcessorGraph::Node>& audioNodeList)
//{
//    for (int i = 0; i < audioNodeList.size() - 1; ++i)
//    {
//        for (int channel = 0; channel < 2; ++channel)
//            mainProcessor->addConnection({ { audioNodeList.getUnchecked(i)->nodeID,      channel },
//                                           { audioNodeList.getUnchecked(i + 1)->nodeID,  channel } });
//    }
//    for (int channel = 0; channel < 2; ++channel)
//    {
//        mainProcessor->addConnection({ { audioInputNode->nodeID,  channel },
//                                       { audioNodeList.getFirst()->nodeID, channel } });
//        mainProcessor->addConnection({ { audioNodeList.getLast()->nodeID,  channel },
//                                       { audioOutputNode->nodeID, channel } });
//    }
//}

void ProcessorGraphTestAudioProcessor::connectMidiNodes()
{
    mainProcessor->addConnection({ { midiInputNode->nodeID,  juce::AudioProcessorGraph::midiChannelIndex },
                                   { midiOutputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex } });
}
void ProcessorGraphTestAudioProcessor::initialiseGraph()
{
    juce::ReferenceCountedArray<juce::AudioProcessorGraph::Node> audioNodeList;
    mainProcessor->clear();
    audioInputNode = mainProcessor->addNode(std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
    audioOutputNode = mainProcessor->addNode(std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));
    midiInputNode = mainProcessor->addNode(std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(juce::AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode));
    midiOutputNode = mainProcessor->addNode(std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(juce::AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode));
    initialiseAudioNodes(audioNodeList);
//    connectAudioNodes(audioNodeList);
    connectMidiNodes();
}

void ProcessorGraphTestAudioProcessor::updateGraph()
{
    bool hasChanged = false;

    juce::Array<juce::AudioParameterChoice*> choices { processorSlot1,
                                                       processorSlot2,
                                                       processorSlot3,
                                                       processorSlot4,
                                                       processorSlot5,
                                                       processorSlot6,
                                                       processorSlot7};

    juce::ReferenceCountedArray<Node> slots;
    slots.add (node1);
    slots.add (node2);
    slots.add (node3);
    slots.add (node4);
    slots.add (node5);
    slots.add (node6);
    slots.add (node7);

    //TODO: create a way to store number of instances of each plugin (enum of arrays?) - the indices 0 are hardcoded for now
    // { "Empty", "Noise Gate", "EQ", "Compressor", "Reverb", "Phaser", "Gain", "Amplifier" }
    for (int i = 0; i < 7; ++i)
    {
        auto& choice = choices.getReference (i);
        auto  slot   = slots  .getUnchecked (i);

        if (choice->getIndex() == 0)            // Empty
        {
            if (slot != nullptr)
            {
                mainProcessor->removeNode (slot.get());
                slots.set (i, nullptr);
                hasChanged = true;
            }
        }
        else if (choice->getIndex() == 1)       // Noise Gate
        {
            if (slot != nullptr)
            {
                if (slot->getProcessor()->getName() == "Noise Gate")
                    continue;

                mainProcessor->removeNode (slot.get());
            }

            slots.set (i, mainProcessor->addNode (std::make_unique<CNoiseGateProcessor>(&apvts, 0)));
            hasChanged = true;
        }
        // TODO: Add EQ processor below
//        else if (choice->getIndex() == 2)       // EQ
//        {
//            if (slot != nullptr)
//            {
//                if (slot->getProcessor()->getName() == "EQ")
//                    continue;
//
//                mainProcessor->removeNode (slot.get());
//            }
//
//            slots.set (i, mainProcessor->addNode (std::make_unique<C>()));
//            hasChanged = true;
//        }
        else if (choice->getIndex() == 3)       // Compressor
        {
            if (slot != nullptr)
            {
                if (slot->getProcessor()->getName() == "Compressor")
                    continue;

                mainProcessor->removeNode (slot.get());
            }

            slots.set (i, mainProcessor->addNode (std::make_unique<CCompressorProcessor>(&apvts, 0)));
            hasChanged = true;
        }

        else if (choice->getIndex() == 4)       // Reverb
        {
            if (slot != nullptr)
            {
                if (slot->getProcessor()->getName() == "Reverb")
                    continue;

                mainProcessor->removeNode (slot.get());
            }

            slots.set (i, mainProcessor->addNode (std::make_unique<CReverbProcessor>(&apvts, 0)));
            hasChanged = true;
        }

        else if (choice->getIndex() == 5)       // Phaser
        {
            if (slot != nullptr)
            {
                if (slot->getProcessor()->getName() == "Phaser")
                    continue;

                mainProcessor->removeNode (slot.get());
            }

            slots.set (i, mainProcessor->addNode (std::make_unique<CPhaserProcessor>(&apvts, 0)));
            hasChanged = true;
        }

        else if (choice->getIndex() == 6)       // Gain
        {
            if (slot != nullptr)
            {
                if (slot->getProcessor()->getName() == "Gain")
                    continue;

                mainProcessor->removeNode (slot.get());
            }

            slots.set (i, mainProcessor->addNode (std::make_unique<CGainProcessor>(&apvts, 0)));
            hasChanged = true;
        }
        //TODO: Add amplifier below
//        else if (choice->getIndex() == 7)       // Amplifier
//        {
//            if (slot != nullptr)
//            {
//                if (slot->getProcessor()->getName() == "Filter")
//                    continue;
//
//                mainProcessor->removeNode (slot.get());
//            }
//
//            slots.set (i, mainProcessor->addNode (std::make_unique<FilterProcessor>()));
//            hasChanged = true;
//        }
    }

    if (hasChanged)
    {
        for (auto connection : mainProcessor->getConnections())     //  remove all the connections in the graph to start from a blank state
            mainProcessor->removeConnection (connection);

        juce::ReferenceCountedArray<Node> activeSlots;

        for (auto slot : slots) // iterate over the slots and check whether they have an AudioProcessor node within the graph
        {
            if (slot != nullptr)
            {
                // add the node to our temporary array of active nodes and setPlayConfigDetails() function on the corresponding
                // processor instance with channel, sample rate and block size information to prepare the node for future processing
                activeSlots.add (slot);

                slot->getProcessor()->setPlayConfigDetails (getMainBusNumInputChannels(),
                                                            getMainBusNumOutputChannels(),
                                                            getSampleRate(), getBlockSize());
            }
        }

        if (activeSlots.isEmpty())
        {
            // if there are no active slots found this means that all the choices are in an "Empty" state and
            // the audio I/O processor nodes can be simply connected together.
            connectAudioNodes();
        }
        else
        {
            // start connecting the active slots together in an ascending order of slot number
            for (int i = 0; i < activeSlots.size() - 1; ++i)
            {
                for (int channel = 0; channel < 2; ++channel)
                    mainProcessor->addConnection ({ { activeSlots.getUnchecked (i)->nodeID,      channel },
                                                    { activeSlots.getUnchecked (i + 1)->nodeID,  channel } });
            }

            // finish connecting the graph by linking the audio input processor node to the first active slot
            // in the chain and the last active slot to the audio output processor node
            for (int channel = 0; channel < 2; ++channel)
            {
                mainProcessor->addConnection ({ { audioInputNode->nodeID,         channel },
                                                { activeSlots.getFirst()->nodeID, channel } });
                mainProcessor->addConnection ({ { activeSlots.getLast()->nodeID,  channel },
                                                { audioOutputNode->nodeID,        channel } });
            }
        }


        // connect the midi I/O nodes together and make sure that all the buses in the audio processors are enabled
        connectMidiNodes();
        for (auto node : mainProcessor->getNodes())
            node->getProcessor()->enableAllBuses();
    }

}

void ProcessorGraphTestAudioProcessor::connectAudioNodes()
{
    for (int channel = 0; channel < 2; ++channel)
        mainProcessor->addConnection ({ { audioInputNode->nodeID,  channel },
                                        { audioOutputNode->nodeID, channel } });
}

//==============================================================================
//     TODO: EDIT THE TWO FUNCTIONS BELOW TO ADD A NODE TO THE GRAPH
//==============================================================================

void ProcessorGraphTestAudioProcessor::initialiseAudioNodes(juce::ReferenceCountedArray<juce::AudioProcessorGraph::Node>& audioNodeList)
{
//    inputGainNode = mainProcessor->addNode(std::make_unique<CGainProcessor>(&apvts,0));
//    audioNodeList.add(inputGainNode);
//
//    noisegateNode = mainProcessor->addNode(std::make_unique<CNoiseGateProcessor>(&apvts,0));
//    audioNodeList.add(noisegateNode);
//
//    compressorNode = mainProcessor->addNode(std::make_unique<CCompressorProcessor>(&apvts,0));
//    audioNodeList.add(compressorNode);
//
//    reverbNode = mainProcessor->addNode(std::make_unique<CReverbProcessor>(&apvts,0));
//    audioNodeList.add(reverbNode);
//
//    phaserNode = mainProcessor->addNode(std::make_unique<CPhaserProcessor>(&apvts,0));
//    audioNodeList.add(phaserNode);
//
//    outputGainNode = mainProcessor->addNode(std::make_unique<CGainProcessor>(&apvts,1));
//    audioNodeList.add(outputGainNode);

}

juce::AudioProcessorValueTreeState::ParameterLayout ProcessorGraphTestAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    // FIXME: The current method of appending a number will break when accessing
    //  the parameter, since the update() has no way to know what the suffix
    //  is, or what the parameter names are

    // ADD PARAMS BELOW
    CGainProcessor::addToParameterLayout(params,0); // Input Gain
    CNoiseGateProcessor::addToParameterLayout(params,0);

    CCompressorProcessor::addToParameterLayout(params,0); // Compressor Params
    CReverbProcessor::addToParameterLayout(params,0); // Reverb Params
    CPhaserProcessor::addToParameterLayout(params,0); // Phaser Params


    CGainProcessor::addToParameterLayout(params, 1); // Output Gain



    return { params.begin() , params.end() };
}

