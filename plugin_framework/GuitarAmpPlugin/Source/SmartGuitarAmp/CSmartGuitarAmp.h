#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveNet.h"
#include "WaveNetLoader.h"
#include "Amplifiers.h"

//==============================================================================
/**
*/
class CSmartGuitarAmp : public CDistortionBase
{
public:
    CSmartGuitarAmp(double sampleRate, int numChannels, int numSamples);
    void prepare(double sampleRate, int numSamples);
    void process(juce::AudioSampleBuffer& buffer) override;
    void loadConfig(juce::File configFile);
    juce::File loaded_tone;
    juce::String loaded_tone_name;
    
private:
    WaveNet waveNet;
    juce::var dummyVar;
};

CSmartGuitarAmp::CSmartGuitarAmp(double sampleRate, int numChannels, int numSamples) : CDistortionBase(sampleRate, numChannels, numSamples), waveNet(1, 1, 1, 1, "linear", { 1 })
{
}

void CSmartGuitarAmp::prepare(double sampleRate, int numSamples)
{
    waveNet.prepareToPlay(numSamples);
}

void CSmartGuitarAmp::process(juce::AudioSampleBuffer &buffer)
{
    buffer.applyGain(10.0);
    waveNet.process(buffer.getArrayOfReadPointers(), buffer.getArrayOfWritePointers(), buffer.getNumSamples());
}

void CSmartGuitarAmp::loadConfig(juce::File configFile)
{
    // suspendProcessing belongs to the audioprocessor class
    // not sure if necessary when it's working
//    this->suspendProcessing(true);
    WaveNetLoader loader(dummyVar, configFile);
    float levelAdjust = loader.levelAdjust;
    int numChannels = loader.numChannels;
    int inputChannels = loader.inputChannels;
    int outputChannels = loader.outputChannels;
    int filterWidth = loader.filterWidth;
    std::vector<int> dilations = loader.dilations;
    std::string activation = loader.activation;
    waveNet.setParams(inputChannels, outputChannels, numChannels, filterWidth, activation, dilations, levelAdjust);
    loader.loadVariables(waveNet);
//    this->suspendProcessing(false);
}
