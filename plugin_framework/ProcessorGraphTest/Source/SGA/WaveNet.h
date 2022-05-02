/*
  ==============================================================================

    WaveNet.h
    Created: 14 Jan 2019 5:19:01pm
    Author:  Damskägg Eero-Pekka

  ==============================================================================
*/

#pragma once

#include <string>
//#include "../../JuceLibraryCode/JuceHeader.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "Activations.h"
#include "ConvolutionStack.h"

class WaveNet
{
public:
    WaveNet(int inputChannels, int outputChannels, int convolutionChannels,
            int filterWidth, std::string activation, std::vector<int> dilations);
    void prepareToPlay (int newSamplesPerBlock);
    void process(const float **inputData, float **outputData, int numSamples);
    void setWeight(std::vector<float> W, int layerIdx, std::string name);
    void setParams(int newInputChannels, int newOutputChannels, int newConvChannels,
                   int newFilterWidth, std::string newActivation,
                   std::vector<int> newDilations, float levelAdjust_in);
    float levelAdjust = 0.0;

private:
    ConvolutionStack convStack;
    ConvolutionLayer inputLayer;
    ConvolutionLayer outputLayer;
    int inputChannels;
    int outputChannels;
    int filterWidth;
    int skipChannels;
    int convolutionChannels;
    int memoryChannels;
    std::string activation;
    std::vector<int> dilations;
    int samplesPerBlock = 0;
    juce::AudioBuffer<float> convData;
    juce::AudioBuffer<float> skipData;
    
    int idx(int ch, int i, int numSamples);
    void readDilations(juce::var config);
    void copyInputData(const float **inputData, int numSamples);
    void copyOutputData(float **outputData, int numSamples);
};
