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
    auto channels = getTotalNumInputChannels();
    mixBuffer.setSize(channels, samplesPerBlock);
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
    const auto numSamples = buffer.getNumSamples();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    //Apply input gain
    m_inputVolume.applyGain(buffer, buffer.getNumSamples());
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        mixBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);
        auto* channelData = buffer.getWritePointer (channel);
//        juce:dsp::AudioBlock<float> block (buffer);
//        juce:dsp::ProcessContextReplacing<float> context (block);
        for (int sample = 0; sample<buffer.getNumSamples();++sample)
        {
            
            // Hard clip values to (-1.0,1.0)
            channelData[sample] = juce::jlimit(-1.f, 1.f, channelData[sample]);
        }
        // ..do something to the data...
    }
    // Apply output gain
    m_outputVolume.applyGain(buffer,buffer.getNumSamples());
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
    // Get values from APVTS
    auto prmVolIn = apvts.getRawParameterValue("VOL IN");
    auto prmDrive = apvts.getRawParameterValue("DRIVE");
    auto prmBlend = apvts.getRawParameterValue("BLEND");
    auto prmVolOut = apvts.getRawParameterValue("VOL OUT");
    
    auto prmPreLPF = apvts.getRawParameterValue("PRELPF");
    auto prmPreHPF = apvts.getRawParameterValue("PREHPF");
    auto prmPostLPF = apvts.getRawParameterValue("POSTLPF");
    auto prmPostHPF = apvts.getRawParameterValue("POSTHPF");

    auto prmDistortionType = apvts.getRawParameterValue("TYPE");
    
    // Get values from the atomic float pointer
    // Convert dB to gain
    m_inputVolume.setTargetValue(juce::Decibels::decibelsToGain(prmVolIn->load()));
    m_drive.setTargetValue(juce::Decibels::decibelsToGain(prmDrive->load()));
    m_blend.setTargetValue(prmBlend->load());
    m_outputVolume.setTargetValue(juce::Decibels::decibelsToGain(prmVolOut->load()));
    m_preLPF.setTargetValue(prmPreLPF->load());
    m_preHPF.setTargetValue(prmPreHPF->load());
    m_postLPF.setTargetValue(prmPostLPF->load());
    m_postHPF.setTargetValue(prmPostHPF->load());
    
    m_distortionType = prmDistortionType->load();
}
void GuitarAmpPluginAudioProcessor::reset()  // reset dsp params
{
    mixBuffer.applyGain(0.f);
    m_inputVolume.reset(getSampleRate(),0.050);
    m_drive.reset(getSampleRate(),0.050);
    m_blend.reset(getSampleRate(),0.050);
    m_outputVolume.reset(getSampleRate(),0.050);
    
    m_preLPF.reset(getSampleRate(),0.050);
    m_preHPF.reset(getSampleRate(),0.050);
    m_postLPF.reset(getSampleRate(),0.050);
    m_postHPF.reset(getSampleRate(),0.050);
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
    //================================================================================================
    // AMPLIFIER PARAMETERS
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("VOL IN", "Input Volume", juce::NormalisableRange< float > (-40.0f,40.0f), 0.0f, "dB", juce::AudioProcessorParameter::genericParameter, valueToTextFunction, textToValueFunction));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("DRIVE", "Drive", juce::NormalisableRange< float > (0.0f,40.0f), 20.0f, "dB", juce::AudioProcessorParameter::genericParameter, valueToTextFunction, textToValueFunction));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("BLEND", "Blend", juce::NormalisableRange< float > (0.0f,100.0f), 100.0f, "%", juce::AudioProcessorParameter::genericParameter, valueToTextFunction, textToValueFunction));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("VOL OUT", "Output Volume", juce::NormalisableRange< float > (-40.0f,40.0f), 0.0f, "dB", juce::AudioProcessorParameter::genericParameter, valueToTextFunction, textToValueFunction));
    
    //================================================================================================
    // FILTERING PARAMETERS
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("PRELPF", "Pre Low-Pass Filter", juce::NormalisableRange< float > (10.f, 20000.f, 0.f, 0.5f), 20000.0f, "Hz", juce::AudioProcessorParameter::genericParameter, valueToTextFunction, textToValueFunction));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("PREHPF", "Pre High-Pass Filter", juce::NormalisableRange< float > (10.f, 20000.f, 0.f, 0.5f), 10.0f, "Hz", juce::AudioProcessorParameter::genericParameter, valueToTextFunction, textToValueFunction));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("POSTLPF", "Post Low-Pass Filter", juce::NormalisableRange< float > (10.f, 20000.f, 0.f, 0.5f), 20000.0f, "Hz", juce::AudioProcessorParameter::genericParameter, valueToTextFunction, textToValueFunction));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("POSTHPF", "Post High-Pass Filter", juce::NormalisableRange< float > (10.f, 20000.f, 0.f, 0.5f), 10.0f, "Hz", juce::AudioProcessorParameter::genericParameter, valueToTextFunction, textToValueFunction));
    
    //================================================================================================
    // DISTORTION TYPE
    juce::StringArray strArray ( {"None", "Tanh", "ATan", "Hard Clipper", "Rectifier", "Sine"} );
    parameters.push_back(std::make_unique<juce::AudioParameterChoice> ("TYPE", "Type", strArray, 1));
    
    //=================================================================================================
    // PARAM RETURN
    return
    {
        parameters.begin(), parameters.end()
    };
}
