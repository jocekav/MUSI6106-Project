/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Compressor.h"
#include "DeEsser.h"
#include "Equalizer.h"
#include "Gate.h"
#include "Reverb.h"
#include "Saturator.h"


//==============================================================================
OneKnobVocalAudioProcessor::OneKnobVocalAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), mainProcessor(new juce::AudioProcessorGraph()),
    apvts{ *this, nullptr, "Parameters", createParameterLayout() }
#endif
{
}

OneKnobVocalAudioProcessor::~OneKnobVocalAudioProcessor()
{
}

//==============================================================================
const juce::String OneKnobVocalAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OneKnobVocalAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OneKnobVocalAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OneKnobVocalAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OneKnobVocalAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OneKnobVocalAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OneKnobVocalAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OneKnobVocalAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String OneKnobVocalAudioProcessor::getProgramName (int index)
{
    return {};
}

void OneKnobVocalAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void OneKnobVocalAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mainProcessor->setPlayConfigDetails(getMainBusNumInputChannels(),
        getMainBusNumOutputChannels(),
        sampleRate, samplesPerBlock);

    mainProcessor->prepareToPlay(sampleRate, samplesPerBlock);

    initialiseGraph();

    for (auto node : mainProcessor->getNodes())
    {
        node->getProcessor()->setPlayConfigDetails(getMainBusNumInputChannels(),
            getMainBusNumOutputChannels(),
            sampleRate, samplesPerBlock);
        node->getProcessor()->prepareToPlay(sampleRate, samplesPerBlock);
        node->getProcessor()->enableAllBuses();
    }
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void OneKnobVocalAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OneKnobVocalAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void OneKnobVocalAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
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
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool OneKnobVocalAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* OneKnobVocalAudioProcessor::createEditor()
{
    return new OneKnobVocalAudioProcessorEditor (*this);
}

//==============================================================================
void OneKnobVocalAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void OneKnobVocalAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
    return new OneKnobVocalAudioProcessor();
}

void OneKnobVocalAudioProcessor::initialiseAudioNodes(juce::ReferenceCountedArray<juce::AudioProcessorGraph::Node>& audioNodeList) //TODO: Could someone make this neat?
{ 

    gateNode = mainProcessor->addNode(std::make_unique<Gate>(&apvts));
    audioNodeList.add(gateNode);

    deEsserNode = mainProcessor->addNode(std::make_unique<Deesser>(&apvts));
    audioNodeList.add(deEsserNode);

    equalizerNode = mainProcessor->addNode(std::make_unique<Equalizer>(&apvts));
    audioNodeList.add(equalizerNode);

    compressorNode = mainProcessor->addNode(std::make_unique<Compressor>(&apvts));
    audioNodeList.add(compressorNode);

    saturatorNode = mainProcessor->addNode(std::make_unique<Saturator>(&apvts));
    audioNodeList.add(saturatorNode);

    reverbNode = mainProcessor->addNode(std::make_unique<Reverb>(&apvts));
    audioNodeList.add(reverbNode);

}

void OneKnobVocalAudioProcessor::connectAudioNodes(juce::ReferenceCountedArray<juce::AudioProcessorGraph::Node>& audioNodeList)

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

void OneKnobVocalAudioProcessor::connectMidiNodes()
{
    mainProcessor->addConnection({ { midiInputNode->nodeID,  juce::AudioProcessorGraph::midiChannelIndex },
                                    { midiOutputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex } });
}


void OneKnobVocalAudioProcessor::initialiseGraph()
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

juce::AudioProcessorValueTreeState::ParameterLayout OneKnobVocalAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    Gate::addToParameterLayout(params);
    Deesser::addToParameterLayout(params);
    Equalizer::addToParameterLayout(params);
    Compressor::addToParameterLayout(params);
    Saturator::addToParameterLayout(params);
    Reverb::addToParameterLayout(params);

    return { params.begin() , params.end() };
}
