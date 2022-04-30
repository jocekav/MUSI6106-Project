/*
  ==============================================================================

    ConvolutionLayer.h
    Created: 10 Jan 2019 5:04:39pm
    Author:  Damskägg Eero-Pekka

  ==============================================================================
*/

#pragma once

#include <string>
//#include "../JuceLibraryCode/JuceHeader.h"
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
#include "Convolution.h"
#include "Activations.h"

class ConvolutionLayer
{
public:
    ConvolutionLayer(size_t inputChannels,
                     size_t outputChannels,
                     int filterWidth,
                     int dilation = 1,
                     bool residual = false,
                     std::string activationName = "linear");
    void process(float* data, int numSamples);
    void process(float* data, float* skipdata, int numSamples);
    void setParams(size_t newInputChannels, size_t newOutputChannels, int newFilterWidth,
                   int newDilation, bool newResidual, std::string newActivationName);
    void setWeight(std::vector<float> W, std::string name);
    
private:
    Convolution conv;
    Convolution out1x1;
    bool residual;
    bool usesGating;
    typedef void (* activationFunction)(float *x , size_t rows, size_t cols);
    activationFunction activation;
    void copySkipData(float *data, float *skipData, int numSamples);
};
