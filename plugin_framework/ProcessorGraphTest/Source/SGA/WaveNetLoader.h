/*
  ==============================================================================

    WaveNetLoader.h
    Created: 3 Feb 2019 8:55:31pm
    Author:  Eero-Pekka Damskägg

    Modified by keyth72

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

#include "WaveNet.h"

class WaveNetLoader
{
public:
    WaveNetLoader(juce::var jsonFile);
    WaveNetLoader(juce::File configFile);
    float levelAdjust;
    int numChannels;
    int inputChannels;
    int outputChannels;
    int filterWidth;
    std::vector<int> dilations;
    std::string activation;
    void loadVariables(WaveNet &model);
    juce::var current_jsonFile;
private:
    std::vector<int> readDilations();
    juce::var config;

};
