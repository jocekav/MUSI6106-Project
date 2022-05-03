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
    NUM_PARAMS = 13;
}

PresetHandler::PresetHandler()
{
    NUM_PARAMS = 13;
}

PresetHandler::~PresetHandler( void )
{
}


void PresetHandler::parseXML(float *paramValues, string fileName)
{
    float aggresive[13] = {0.0, 1.0, 0.0, 1000.0, 21.31999969482422, 0.0, 19.16999816894531, 0.4200000762939453, 21.03999900817871, 14.38999938964844, -11.25, 10.59000015258789, 30.0};
    for (int i = 0; i < 13; i++)
    {
        paramValues[i] = aggresive[i];
    }
//    float new paramValues[41];
    // Read the sample.xml file
//    xml_document<> doc;
//    xml_node<> * root_node;
//
//    fileName = "/Users/jocekav/Documents/GitHub/MUSI6106-Project/plugin_framework/ProcessorGraphTest/Resources/XMLPresets/" + fileName;
//
//    juce::File file = juce::File(fileName);
//
//    juce::XmlDocument doc(file);
//    std::unique_ptr<juce::XmlElement> mainElement = doc.getDocumentElement();
//
//    if (mainElement == 0)
//        {
//            std::cout <<(doc.getLastParseError());
//        }
//    else
//    {
//        if( mainElement->hasTagName( "PARAM" ) )
//        {
//            int count = 0;
//            forEachXmlChildElement(*mainElement, child)
//            {
//                if( child->hasTagName( "value" ) )
//                {
//                    float value = (child->getAllSubText()).getFloatValue();
//                    paramValues[count] = value;
//                }
//            }
//        }
//    }
    
//    ifstream xmlFile;
//    xmlFile.open(fileName);
//    static vector<char> buffer((istreambuf_iterator<char>(xmlFile)), istreambuf_iterator<char>());
//    buffer.push_back('\0');
//
//    // Parse the buffer
//    doc.parse<0>(&buffer[0]);
//
//    // Find out the root node
//    root_node = doc.first_node("Parameters");
//    int count = 0;
//    for (xml_node<> * param_node = root_node->first_node("PARAM"); param_node; param_node = param_node->next_sibling())
//    {
//        paramValues[count] = atof(param_node->first_attribute("value")->value());
//        count++;
//    }
}

void PresetHandler::setParamsFromXML(float *paramValues, juce::AudioProcessorValueTreeState* apvts, int num_params)
{
    float displayVal;
//    const char* paramNames[] = { "Amp_0", "DelayBlend_0", "DelayBypass_0", "DelayTime_0", "CompressorAttack_0","CompressorBypass_0","CompressorInputGain_0","CompressorMakeupGain_0","CompressorRatio_0","CompressorRelease_0","CompressorThreshold_0", "GainValue_0","GainValue_1","NoiseGateAttack_0","NoiseGateBypass_0","NoiseGateRatio_0","NoiseGateRelease_0","NoiseGateThreshold_0","PhaserBlend_0","PhaserBypass_0","PhaserDepth_0","PhaserFc_0","PhaserFeedback_0","PhaserRate_0","ReverbBlend_0","ReverbBypass_0","ReverbDamping_0","ReverbRoomSize_0" "EqualizerBypass_0","EqualizerHMF_0","EqualizerHMGain_0","EqualizerHMQ_0","EqualizerHPFQ_0","EqualizerHPF_0","EqualizerLMF_0","EqualizerLMGain_0","EqualizerLMQ_0","EqualizerLPFQ_0","EqualizerLPF_0","EqualizerMF_0","EqualizerMGain_0","EqualizerMQ_0"};
    const char* paramNames[] = { "Amp_0", "DelayBlend_0", "DelayBypass_0", "DelayTime_0", "CompressorAttack_0","CompressorBypass_0","CompressorInputGain_0","CompressorMakeupGain_0","CompressorRatio_0","CompressorRelease_0","CompressorThreshold_0", "GainValue_0","GainValue_1"};
    
    for (int i = 0; i < num_params; i++) {
    
        juce::Value curVal = apvts->getParameterAsValue(paramNames[i]);
        displayVal = paramValues[i];
        curVal = paramValues[i];
    }

}

void PresetHandler::averageMultiArray(float **paramValues, float *avgValues, int numParams, int numChoices)
{
    float currValue;
    float prevValue;
    for (int i = 0; i < numParams; i++)
    {
        prevValue = paramValues[0][i];
        avgValues[i] = paramValues[0][i];
        for (int j = 1; j < numChoices - 1; j++){
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
        // get average through division
        avgValues[i] /= numChoices;
    }
}

std::string PresetHandler::convertToFileNames(int presetInd)
{
//    for (int i = 0; i < numPresetsToSet; i++)
//    {
//        string currName = presetNameArray[i];
//        transform(currName.begin(), currName.end(), currName.begin(), ::tolower);
//        currName.append(".xml");
//        presetNameArray[i] = currName;
//    }
    const char* fileNames[] = { "aggresive.xml", "airy.xml", "attack.xml", "bloom.xml", "boom.xml","bright.xml","chunky.xml","compressed.xml","creamy.xml","djent.xml","fat.xml", "fizz.xml","floppy.xml", "flutey.xml","growl.xml","hot.xml","icepick.xml","juicy.xml","muddy.xml","mushy.xml", "quack.xml","sizzle.xml", "twang.xml", "vintage.xml", "warm.xml", "woof.xml"};
    return fileNames[presetInd];
}

void PresetHandler::setParamsFromPopUp(int* presetIndArray, int numPresetsToSet)
{
    // how many choices selected
    // create a multi-dimensional array with 41xlength(choice_Names)
    ppf_paramValues = new float* [unsigned(numPresetsToSet)]();
    
    for (int i = 0; i < numPresetsToSet; i++)
    {
        ppf_paramValues[i] = new float[unsigned(NUM_PARAMS)]();
    }
    
//    convertToFileNames(presetNameArray, numPresetsToSet);
    string fileName;
    
    // in iteratition, instantiate file names and parseXML
    for (int i = 0; i < numPresetsToSet; i++)
    {
        fileName = convertToFileNames(presetIndArray[i]);
        parseXML(ppf_paramValues[i], fileName);
    }
    
    float* avgValues = new float [unsigned(NUM_PARAMS)]();
    // send multi array to averaging function with length of choices
    averageMultiArray(ppf_paramValues, avgValues, NUM_PARAMS, numPresetsToSet);
    
    // set values to averages in a loop
//    setParamsFromXML(avgValues, m_pAPVTS, NUM_PARAMS);
    
    // delete everything
    for (int i = 0; i < numPresetsToSet; i++)
    {
        delete ppf_paramValues[i];
    }
    delete[] ppf_paramValues;
    delete[] avgValues;
}



