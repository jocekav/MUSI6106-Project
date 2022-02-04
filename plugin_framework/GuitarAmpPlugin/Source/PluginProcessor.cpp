/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuitarAmpPluginAudioProcessor::GuitarAmpPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "parameters", createParameters())
#endif
{
    apvts.state.addListener(this);
    init();
}

GuitarAmpPluginAudioProcessor::~GuitarAmpPluginAudioProcessor()
{
}

//==============================================================================
const juce::String GuitarAmpPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GuitarAmpPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GuitarAmpPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GuitarAmpPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GuitarAmpPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GuitarAmpPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GuitarAmpPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GuitarAmpPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GuitarAmpPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void GuitarAmpPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GuitarAmpPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    prepare(sampleRate, samplesPerBlock);
    update();
    reset();
    isActive = true;
}

void GuitarAmpPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GuitarAmpPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void GuitarAmpPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if(!isActive)  // Make sure plugin is properly instantiated before running
    {
        return;
    }
    
    if(mustUpdateParams) // Check if params need to e updated, and do so if required
    {
        update();
    }
    
    juce::ScopedNoDenormals noDenormals; // Zeros out very small values to save processing power
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
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        
        juce::ignoreUnused(channelData);
        
        
        
        // Hard clip values to (-1.0,1.0)
        for (int sample = 0; sample<buffer.getNumSamples();++sample)
        {    
            channelData[sample] = juce::jlimit(-1.f, 1.f, channelData[sample]);
        }
        // ..do something to the data...
    }
}

//==============================================================================
bool GuitarAmpPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GuitarAmpPluginAudioProcessor::createEditor()
{
//    return new GuitarAmpPluginAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void GuitarAmpPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    juce::ValueTree copyState = apvts.copyState(); // Thread safe way to copy the params
    std::unique_ptr<juce::XmlElement> xml = copyState.createXml();
    copyXmlToBinary(*xml.get(), destData);
}

void GuitarAmpPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xml = getXmlFromBinary(data, sizeInBytes);
    juce::ValueTree copyState = juce::ValueTree::fromXml(*xml.get());
    apvts.replaceState(copyState);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarAmpPluginAudioProcessor();
}

//==============================================================================
// -----------------------------------------------------------------------------
//==============================================================================
void GuitarAmpPluginAudioProcessor::init() // Call once -- set initial values for DSP
{
    
}

void  GuitarAmpPluginAudioProcessor::prepare(double sampleRate, int samplesPerBlock) // Pass sample rate and buffer size to DSP
{
    
}
void GuitarAmpPluginAudioProcessor::update() //update params when changed
{
    
    
    mustUpdateParams = false;
}
void GuitarAmpPluginAudioProcessor::reset()  // reset dsp params
{
    
}

juce::AudioProcessorValueTreeState::ParameterLayout GuitarAmpPluginAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;
    
    std::function<juce::String(float, int)> valueToTextFunction = [](float x, int l) {return juce::String(x,4); };
    std::function<float (const juce::String&)> textToValueFunction = [](const juce::String &str) {return str.getFloatValue(); };
    
//    //Create Parameters
//    auto gainParam = std::make_unique<juce::AudioParameterFloat>("VOL", "Volume", juce::NormalisableRange< float > (-40.0f,40.0f), 0.0f, "dB", juce::AudioProcessorParameter::genericParameter, valueToTextFunction, textToValueFunction);
//
//    //Add parameters to vector
//    parameters.push_back(std::move(gainParam));

    // Add new parameters to vector
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("VOL", "Volume", juce::NormalisableRange< float > (-40.0f,40.0f), 0.0f, "dB", juce::AudioProcessorParameter::genericParameter, valueToTextFunction, textToValueFunction));
    return
    {
        parameters.begin(), parameters.end()
    };
}
