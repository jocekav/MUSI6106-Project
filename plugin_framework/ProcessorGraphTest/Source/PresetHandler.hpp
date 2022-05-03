//
//  PresetHandler.hpp
//  GUITARAMPPLUGIN
//
//  Created by Jocelyn Kavanagh on 5/1/22.
//
#pragma once

#ifndef PresetHandler_hpp
#define PresetHandler_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "rapidxml.hpp"
#include <stdlib.h>

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


class PresetHandler
{
public:
    PresetHandler();
    PresetHandler(juce::AudioProcessorValueTreeState *apvts);
    ~PresetHandler(void);
    
    void parseXML(float *paramValues, std::string fileName);
    void setParamsFromXML(float *paramValues, juce::AudioProcessorValueTreeState* apvts, int num_params);
    void averageMultiArray(float **paramValues, float *avgValues, int numParams, int numChoices);
    void setParamsFromPopUp(std::string* presetNameArray, int numPresetsToSet);
    void convertToFileNames(std::string* presetNameArray, int numPresetsToSet);
    
    juce::AudioProcessorValueTreeState* m_pAPVTS;
    

    
protected:
    int NUM_PARAMS;
    float** ppf_paramValues;
    std::string* fileNameArray;
};

#endif /* PresetHandler_hpp */
