/*
  ==============================================================================

    ConvolutionStack.h
    Created: 8 Jan 2019 5:21:49pm
    Author:  Damskägg Eero-Pekka

  ==============================================================================
*/

#pragma once

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
#include "ConvolutionLayer.h"

class ConvolutionStack
{
public:
    ConvolutionStack(int numChannels, int filterWidth, std::vector<int> dilations,
                     std::string activation, bool residual = true);
    void process(float *data, float* skipData, int numSamples);
    void prepareToPlay(int newNumSamples);
    size_t getNumLayers() { return dilations.size(); }
    void setWeight(std::vector<float> W, int layerIdx, std::string name);
    void setParams(int newNumChannels, int newFilterWidth, std::vector<int> newDilations,
                   std::string newActivation, bool newResidual);
    
private:
    std::vector<ConvolutionLayer> layers;
    std::vector<int> dilations;
    bool residual;
    int numChannels;
    int filterWidth;
    std::string activation;
    int samplesPerBlock = 0;
    juce::AudioBuffer<float> residualData;
    int idx(int ch, int i, int numSamples);
    void copyResidual(float *data, int numSamples);
    void addResidual(float *data, int numSamples);
    float* getSkipPointer(float *skipData, int layerIdx, int numSamples);
    void initLayers();
};
