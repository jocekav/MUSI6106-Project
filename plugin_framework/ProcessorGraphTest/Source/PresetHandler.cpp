//
//  PresetHandler.cpp
//  GUITARAMPPLUGIN
//
//  Created by Jocelyn Kavanagh on 5/1/22.
//

#include "PresetHandler.hpp"

using namespace std;
using namespace rapidxml;

PresetHandler::PresetHandler(juce::AudioProcessorValueTreeState *apvts)
{
    m_pAPVTS = apvts;
    NUM_PARAMS = 41;
}


void PresetHandler::parseXML(float *paramValues, string fileName)
{
//    float new paramValues[41];
    // Read the sample.xml file
    xml_document<> doc;
    xml_node<> * root_node;
    
    ifstream theFile (fileName);
    vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');
   
    // Parse the buffer
    doc.parse<0>(&buffer[0]);
   
    // Find out the root node
    root_node = doc.first_node("Parameters");
    int count = 0;
    for (xml_node<> * param_node = root_node->first_node("PARAM"); param_node; param_node = param_node->next_sibling())
    {
        paramValues[count] = atof(param_node->first_attribute("value")->value());
        count++;
    }
}

void PresetHandler::setParamsFromXML(float *paramValues, juce::AudioProcessorValueTreeState* apvts, int num_params)
{
    const char* paramNames[] = { "Amp_0", "DelayBlend_0", "DelayBypass_0", "DelayTime_0", "CompressorAttack_0","CompressorBypass_0","CompressorInputGain_0","CompressorMakeupGain_0","CompressorRatio_0","CompressorRelease_0","CompressorThreshold_0", "EqualizerBypass_0","EqualizerHMF_0","EqualizerHMGain_0","EqualizerHMQ_0","EqualizerHPFQ_0","EqualizerHPF_0","EqualizerLMF_0","EqualizerLMGain_0","EqualizerLMQ_0","EqualizerLPFQ_0","EqualizerLPF_0","EqualizerMF_0","EqualizerMGain_0","EqualizerMQ_0","GainValue_0","GainValue_1","NoiseGateAttack_0","NoiseGateBypass_0","NoiseGateRatio_0","NoiseGateRelease_0","NoiseGateThreshold_0","PhaserBlend_0","PhaserBypass_0","PhaserDepth_0","PhaserFc_0","PhaserFeedback_0","PhaserRate_0","ReverbBlend_0","ReverbBypass_0","ReverbDamping_0","ReverbRoomSize_0" };
    
    for (int i = 0; i < num_params; i++) {
    
        juce::Value curVal = apvts->getParameterAsValue(paramNames[i]);
        curVal = paramValues[i];
    }

}

void PresetHandler::averageMultiArray(float **paramValues, float *avgValues, int numParams, int numChoices)
{
    float currValue;
    float prevValue;
    int count = 0;
    for (int i = 0; i < numParams; i++)
    {
        prevValue = paramValues[count][i];
        avgValues[i] = paramValues[count][i];
        for (int j = 0; j < numChoices - 1; j++){
            // check bypassed
            currValue = paramValues[j][i];
            if ((currValue == 0.0 && prevValue == 1.0) || (prevValue == 0.0 && currValue == 1.0))
            {
                currValue = 0.0;
            }
            else
            {
            // add params
                avgValues[i] += currValue;
            }
            prevValue = currValue;
        }
        count++;
        // get average through division
        avgValues[i] /= numChoices;
    }
}

void PresetHandler::setParamsFromComboBox(int choiceNames)
{
    // how many choices selected
    // create a multi-dimensional array with 41xlength(choice_Names)
    ppf_paramValues = new float* [choiceNames];
    
    for (int i = 0; i < NUM_PARAMS; i++)
    {
        ppf_paramValues[i] = new float[NUM_PARAMS];
    }
//    float** paramValues[choiceNames][NUM_PARAMS];
    string* fileNameArray = new string [choiceNames];
    string fileName;
    
    // in iteratition, instantiate file names and parseXML
    for (int i = 0; i < choiceNames; i++)
    {
        fileName = fileNameArray[i];
        parseXML(ppf_paramValues[i], fileName);
    }
    
    float* avgValues = new float [NUM_PARAMS];
    // send multi array to averaging function with length of choices
    averageMultiArray(ppf_paramValues, avgValues, NUM_PARAMS, choiceNames);
    
    // set values to averages in a loop
    setParamsFromXML(avgValues, m_pAPVTS, NUM_PARAMS);
    
    // delete everything
    for (int i = 0; i < NUM_PARAMS; i++)
    {
        delete ppf_paramValues[i];
    }
    delete[] ppf_paramValues;
    delete[] fileNameArray;
}



