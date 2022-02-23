/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"



//==============================================================================
GuitarAmpPluginAudioProcessor::GuitarAmpPluginAudioProcessor()
{
    // -------------------------------------------------------------------------
    addParameter (prmInput = new juce::AudioParameterFloat ("INPUT", "Input Gain", {0.f, 40.f}, 0.f, "dB"));
    addParameter (prmDrive = new juce::AudioParameterFloat ("DRIVE", "Drive", {0.f, 40.f}, 20.f, "dB"));
    addParameter (prmMix = new juce::AudioParameterFloat ("MIX", "Mix", {0.f, 100.f}, 100.f, "%"));
    addParameter (prmOutput = new juce::AudioParameterFloat ("OUTPUT", "Output Gain", {-40.f, 40.f}, 0.f, "dB"));

    // -------------------------------------------------------------------------
    juce::StringArray strArray ( {"None", "Tanh", "ATan", "Hard Clipper", "Rectifier", "Sine"} );
    addParameter (m_distortionType = new juce::AudioParameterChoice ("TYPE", "Type", strArray, 1));

    // -------------------------------------------------------------------------
    addParameter (prmPreLP = new juce::AudioParameterFloat ("PRELP", "Pre Low-Pass", {10.f, 20000.f, 0.f, 0.5f}, 20000.f, "Hz"));
    addParameter (prmPreHP = new juce::AudioParameterFloat ("PREHP", "Pre High-Pass", {10.f, 20000.f, 0.f, 0.5f}, 20.f, "Hz"));
    addParameter (prmPostLP = new juce::AudioParameterFloat ("POSTLP", "Post Low-Pass", {10.f, 20000.f, 0.f, 0.5f}, 20000.f, "Hz"));
    addParameter (prmPostHP = new juce::AudioParameterFloat ("POSTHP", "Post High-Pass", {10.f, 20000.f, 0.f, 0.5f}, 20.f, "Hz"));

    // -------------------------------------------------------------------------
    addParameter (prmEPfreq = new juce::AudioParameterFloat ("EPFREQ", "Emph freq", {10.f, 20000.f, 0.f, 0.5f}, 700.f, "Hz"));
    addParameter (prmEPgain = new juce::AudioParameterFloat ("EPGAIN", "Emph gain", {-20.f, 20.f}, 0.f, "dB"));
}

GuitarAmpPluginAudioProcessor::~GuitarAmpPluginAudioProcessor()
{
}

//==============================================================================
double GuitarAmpPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

//==============================================================================
void GuitarAmpPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    auto channels = static_cast<juce::uint32> (fmin (getMainBusNumInputChannels(), getMainBusNumOutputChannels()));
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), channels };
    
    preLowPassFilter.prepare (spec);
    preHighPassFilter.prepare (spec);
    postLowPassFilter.prepare (spec);
    postHighPassFilter.prepare (spec);
    
    preEmphasisFilter.prepare (spec);
    postEmphasisFilter.prepare (spec);
    
    mixBuffer.setSize (channels, samplesPerBlock);
    
    updateProcessing();
    reset();

    isActive = true;
}

void GuitarAmpPluginAudioProcessor::reset()
{
    mixBuffer.applyGain (0.f);
    
    inputVolume.reset (getSampleRate(), 0.05);
    driveVolume.reset (getSampleRate(), 0.05);
    dryVolume.reset (getSampleRate(), 0.05);
    wetVolume.reset (getSampleRate(), 0.05);
    outputVolume.reset (getSampleRate(), 0.05);
    
    preLowPassFilter.reset();
    preHighPassFilter.reset();
    postLowPassFilter.reset();
    postHighPassFilter.reset();
    
    preEmphasisFilter.reset();
    postEmphasisFilter.reset();
}

void GuitarAmpPluginAudioProcessor::releaseResources()
{

}

void GuitarAmpPluginAudioProcessor::processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    if (isActive == false)
        return;

    updateProcessing();
    
    
    // ------------------------------------------------------------------------------------------
    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    const auto numChannels = fmin (totalNumInputChannels, totalNumOutputChannels);
    const auto numSamples = buffer.getNumSamples();
        
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    // Input Volume
    // ------------------------------------------------------------------------------------------
    inputVolume.applyGain (buffer, numSamples);
    
    
    // Mix Buffer feeding
    // ------------------------------------------------------------------------------------------
    for (auto channel = 0; channel < numChannels; channel++)
        mixBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);


    // Processing
    // ------------------------------------------------------------------------------------------
    juce::ScopedNoDenormals noDenormals;
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    
    // Pre-Filtering
    // ------------------------------------------------------------------------------------------
    preLowPassFilter.process (context);
    preHighPassFilter.process (context);
    preEmphasisFilter.process (context);


    // Drive Volume
    // ------------------------------------------------------------------------------------------
    driveVolume.applyGain (buffer, numSamples);
    
    
    // Static Waveshaper
    // ------------------------------------------------------------------------------------------
    const auto type = m_distortionType->getIndex();

    if (type == AlgorithmTanh)
    {
        for (auto channel = 0; channel < numChannels; channel++)
        {
            auto* channelData = buffer.getWritePointer (channel);
            
            for (auto i = 0; i < numSamples; i++)
                channelData[i] = std::tanh (channelData[i]);
        }
    }
    else if (type == AlgorithmAtan)
    {
        const auto factor = 2.f / juce::MathConstants<float>::pi;
        
        for (auto channel = 0; channel < numChannels; channel++)
        {
            auto* channelData = buffer.getWritePointer (channel);
            
            for (auto i = 0; i < numSamples; i++)
                channelData[i] = std::atan (channelData[i]) * factor;
        }
    }
    else if (type == AlgorithmHardClipper)
    {
        for (auto channel = 0; channel < numChannels; channel++)
        {
            auto* channelData = buffer.getWritePointer (channel);
            
            for (auto i = 0; i < numSamples; i++)
                channelData[i] = juce::jlimit (-1.f, 1.f, channelData[i]);
        }
    }
    else if (type == AlgorithmRectifier)
    {
        for (auto channel = 0; channel < numChannels; channel++)
        {
            auto* channelData = buffer.getWritePointer (channel);
            
            for (auto i = 0; i < numSamples; i++)
                channelData[i] = std::abs (channelData[i]);
        }
    }
    else if (type == AlgorithmSine)
    {
        for (auto channel = 0; channel < numChannels; channel++)
        {
            auto* channelData = buffer.getWritePointer (channel);
            
            for (auto i = 0; i < numSamples; i++)
                channelData[i] = std::sin (channelData[i]);
        }
    }


    // Post-Filtering
    // ------------------------------------------------------------------------------------------
    postLowPassFilter.process (context);
    postHighPassFilter.process (context);
    postEmphasisFilter.process (context);


    // Mix processing
    // ------------------------------------------------------------------------------------------
    dryVolume.applyGain (mixBuffer, numSamples);
    wetVolume.applyGain (buffer, numSamples);
    
    for (auto channel = 0; channel < numChannels; channel++)
        buffer.addFrom(channel, 0, mixBuffer, channel, 0, numSamples);
    

    // Output Volume
    // ------------------------------------------------------------------------------------------
    outputVolume.applyGain (buffer, numSamples);


    // Hard clipper limiter
    // ------------------------------------------------------------------------------------------
    for (auto channel = 0; channel < numChannels; channel++)
    {
        auto* channelData = buffer.getWritePointer (channel);
        
        for (auto i = 0; i < numSamples; i++)
            channelData[i] = juce::jlimit (-2.f, 2.f, channelData[i]);
    }
}

void GuitarAmpPluginAudioProcessor::updateProcessing()
{
    inputVolume.setTargetValue (juce::Decibels::decibelsToGain (prmInput->get()));
    outputVolume.setTargetValue (juce::Decibels::decibelsToGain (prmOutput->get()));
    driveVolume.setTargetValue (juce::Decibels::decibelsToGain (prmDrive->get()));
    
    auto mix = prmMix->get() * 0.01f;
    dryVolume.setTargetValue (1.f - mix);
    wetVolume.setTargetValue (mix);

    // ------------------------------------------------------------------------------------------
    *preLowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass (getSampleRate(), prmPreLP->get());
    *preHighPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass (getSampleRate(), prmPreHP->get());
    *postLowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass (getSampleRate(), prmPostLP->get());
    *postHighPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass (getSampleRate(), prmPostHP->get());
    
    *preEmphasisFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf (getSampleRate(), prmEPfreq->get(),
                                                                                   1.f / sqrt (2.f), juce::Decibels::decibelsToGain (prmEPgain->get()));
    *postEmphasisFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf (getSampleRate(), prmEPfreq->get(),
                                                                                    1.f / sqrt (2.f), juce::Decibels::decibelsToGain (-prmEPgain->get()));
}

//==============================================================================
bool GuitarAmpPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GuitarAmpPluginAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarAmpPluginAudioProcessor();
}
