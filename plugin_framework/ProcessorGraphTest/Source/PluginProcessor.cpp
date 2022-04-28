/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
//#include "Effects_new.h"

//==============================================================================
ProcessorGraphTestAudioProcessor::ProcessorGraphTestAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),mainProcessor(new juce::AudioProcessorGraph()),
                       apvts{*this, nullptr, "Parameters",createParameterLayout()}
#endif
{
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
    this->updateGraph();

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

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

void ProcessorGraphTestAudioProcessor::connectAudioNodes(juce::ReferenceCountedArray<juce::AudioProcessorGraph::Node>& audioNodeList)
{
    for (int i = 0; i < audioNodeList.size() - 1; ++i)
    {
        for (int channel = 0; channel < 2; ++channel)
            mainProcessor->addConnection({ { audioNodeList.getUnchecked(i)->nodeID,      channel },
                                           { audioNodeList.getUnchecked(i + 1)->nodeID,  channel } });
    }
    for (int channel = 0; channel < 2; ++channel)
    {
        mainProcessor->addConnection({ { audioInputNode->nodeID,  channel },
                                       { audioNodeList.getFirst()->nodeID, channel } });
        mainProcessor->addConnection({ { audioNodeList.getLast()->nodeID,  channel },
                                       { audioOutputNode->nodeID, channel } });
    }
}

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
    connectAudioNodes(audioNodeList);
    connectMidiNodes();
}

void ProcessorGraphTestAudioProcessor::updateGraph()
{
    //bool hasChanged = false;

    //juce::Array<juce::AudioParameterChoice*> choices{ AmpSlot };
    //juce::ReferenceCountedArray<juce::AudioProcessorGraph::Node> AmpNode;
    //    mainProcessor->getNode(AmpNodeIndex);
    //

    //auto& choice = choices.getReference(0);
    //auto  ampSlot = audioNodeList.getUnchecked(3);

    //switch (choice->getIndex()) {
    //case EmptyIndex:
    //    if (ampSlot != nullptr)
    //    {
    //        if (ampSlot->getProcessor()->getName() == "Bypass")
    //            break;
    //        mainProcessor->removeNode(ampSlot.get());
    //    }
    //    audioNodeList.set(AmpNodeIndex, bypassAmpNode);
    //    hasChanged = true;
    //    break;

    //case WaveshaperIndex:
    //    if (ampSlot != nullptr)
    //    {
    //        if (ampSlot->getProcessor()->getName() == "TanhWaveshaping")
    //            break;
    //        mainProcessor->removeNode(ampSlot.get());
    //    }
    //    audioNodeList.set(AmpNodeIndex, waveshapingNode);
    //    hasChanged = true;
    //    break;

    //case AnalogAmpIndex:
    //    if (ampSlot != nullptr)
    //    {
    //        if (ampSlot->getProcessor()->getName() == "TanhWaveshaping")
    //            break;
    //        mainProcessor->removeNode(ampSlot.get());
    //    }
    //    audioNodeList.set(AmpNodeIndex, AnalogAmpNode);
    //    hasChanged = true;
    //    break;

    //    //case SGAIndex:
    //    //    if (ampSlot != nullptr)
    //    //    {
    //    //        if (ampSlot->getProcessor()->getName() == "SGAmp")
    //    //            break;

    //    //        mainProcessor->removeNode(ampSlot.get());
    //    //    }
    //    //    audioNodeList.set(AmpNodeIndex, SGANode);
    //    //    hasChanged = true;
    //    //    break;
    //}
    
}


//==============================================================================
//     TODO: EDIT THE TWO FUNCTIONS BELOW TO ADD A NODE TO THE GRAPH
//==============================================================================

void ProcessorGraphTestAudioProcessor::initialiseAudioNodes(juce::ReferenceCountedArray<juce::AudioProcessorGraph::Node>& audioNodeList)
{
    inputGainNode = mainProcessor->addNode(std::make_unique<CGainProcessor>(&apvts,0));
    audioNodeList.add(inputGainNode);

    noisegateNode = mainProcessor->addNode(std::make_unique<CNoiseGateProcessor>(&apvts,0));
    audioNodeList.add(noisegateNode);

    compressorNode = mainProcessor->addNode(std::make_unique<CCompressorProcessor>(&apvts,0));
    audioNodeList.add(compressorNode);

    reverbNode = mainProcessor->addNode(std::make_unique<CReverbProcessor>(&apvts,0));
    audioNodeList.add(reverbNode);

    AmpInterfaceNode = mainProcessor->addNode(std::make_unique<CAmpIf>(&apvts, 0));
    audioNodeList.add(AmpInterfaceNode);

    phaserNode = mainProcessor->addNode(std::make_unique<CPhaserProcessor>(&apvts,0));
    audioNodeList.add(phaserNode);

    outputGainNode = mainProcessor->addNode(std::make_unique<CGainProcessor>(&apvts,1));
    audioNodeList.add(outputGainNode);

    //waveshapingNode = mainProcessor->addNode(std::make_unique<CTanhWaveshaping>(&apvts, 0));
    //AnalogAmpNode = mainProcessor->addNode(std::make_unique<CPreampProcessorChain>(&apvts, 0));
    // SGANode = CSmartGuitarAmp->addNode(std::make_unique<CBypassAmp>(&apvts, 0));
}


juce::AudioProcessorValueTreeState::ParameterLayout ProcessorGraphTestAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    // FIXME: The current method of appending a number will break when accessing
    //  the parameter, since the update()has no way to know what the suffix
    //  is, or what the parameter names are

    // ADD PARAMS BELOW
    CGainProcessor::addToParameterLayout(params,0); // Input Gain
    CNoiseGateProcessor::addToParameterLayout(params,0);
//
    CCompressorProcessor::addToParameterLayout(params,0); // Compressor Params
    CReverbProcessor::addToParameterLayout(params,0); // Reverb Params
    CPhaserProcessor::addToParameterLayout(params,0);
    CAmpIf::addToParameterLayout(params, 0);

//    CGainProcessor::addToParameterLayout(params); // Input Gain
//    CNoiseGateProcessor::addToParameterLayout(params);
//
//    CCompressorProcessor::addToParameterLayout(params); // Compressor Params
//    CReverbProcessor::addToParameterLayout(params); // Reverb Params
//    CPhaserProcessor::addToParameterLayout(params);
//
////    TODO: Find a way to change the name of the parameter from gain to output gain
    CGainProcessor::addToParameterLayout(params, 1); // Output Gain



    return { params.begin() , params.end() };
}

