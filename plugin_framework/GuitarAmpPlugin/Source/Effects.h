///*
//  ==============================================================================
//
//    Effects.h
//    Created: 22 Feb 2022 11:04:09am
//    Author:  Vedant Kalbag
//
//  ==============================================================================
//*/
//
#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ErrorDef.h"

class CAmplifierDistortion //: public juce::AudioProcessor
{
public:
    enum class DistortionType
    {
        AlgorithmNone = 0,
        AlgorithmTanh,
        AlgorithmAtan,
        AlgorithmHardClipper,
        AlgorithmRectifier,
        AlgorithmSine,
        AlgorithmTubeModel
    };
//    Error_t create(CAmplifierDistortion*& pCInstance);
    Error_t resetDistortion();
    Error_t prepare(juce::dsp::ProcessSpec spec, int channels, int samplesPerBlock);
    Error_t updateProcessing();
    Error_t process(juce::AudioSampleBuffer& buffer, int numChannels, int numInputChannels, int numOutputChannels);

private:
    CAmplifierDistortion(juce::AudioParameterFloat *prmPreLP, juce::AudioParameterFloat *prmPreHP, juce::AudioParameterFloat *prmPreLP);
    ~CAmplifierDistortion();
    bool isActive = true;




    juce::AudioBuffer<float> mixBuffer;

    using Filter = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    Filter preLowPassFilter, preHighPassFilter, postLowPassFilter, postHighPassFilter;
    Filter preEmphasisFilter, postEmphasisFilter;
    Filter tubeLowPassFilter, tubeHighPassFilter;

    //Triode Params;
    float mu = 100.0f;
    float mu_inverse = 1 / mu;
    float E_x = 1.4f;
    int K_g = 1060;
    int K_p = 600;
    int K_vb = 300;
    float V_ct = 0.5f;
    int V_ak = 280;

    float V_gk_cutoff = 5.5f;
    float sqrt_K = static_cast<float> (sqrt(K_vb + (V_ak * V_ak)));
    float TriodeWaveshaper(float V_gk);
};

