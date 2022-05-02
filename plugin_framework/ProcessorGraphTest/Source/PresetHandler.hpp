//
//  PresetHandler.hpp
//  GUITARAMPPLUGIN
//
//  Created by Jocelyn Kavanagh on 5/1/22.
//

#ifndef PresetHandler_hpp
#define PresetHandler_hpp

#include <stdio.h>
#include "PresetHandler.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include "rapidxml.hpp"
#include <stdlib.h>
#include "PluginProcessor.h"


class PresetHandler
{
public:
    PresetHandler(juce::AudioProcessorValueTreeState *apvts);
    ~PresetHandler();
    
    void parseXML(float *paramValues, std::string fileName);
    void setParamsFromXML(float *paramValues, juce::AudioProcessorValueTreeState* apvts, int num_params);
    void averageMultiArray(float **paramValues, float *avgValues, int numParams, int numChoices);
    void setParamsFromComboBox(int choiceNames);
    
    
    juce::AudioProcessorValueTreeState* m_pAPVTS;
    
protected:
    int NUM_PARAMS;
    float** ppf_paramValues;
    std::string* fileNameArray;
};

#endif /* PresetHandler_hpp */
