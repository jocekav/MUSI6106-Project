//
// Created by Vedant Kalbag on 09/03/22.
//
#include <JuceHeader.h>
#include "ErrorDef.h"
//#include "BaseProcessor.h"

#ifndef GUITARAMPPLUGIN_AMPLIFIERS_H
#define GUITARAMPPLUGIN_AMPLIFIERS_H

//==========================================================================
class CDistortionBase
{
public:
    virtual void process (juce::AudioSampleBuffer& buffer) = 0;
    void update(double sampleRate, int numChannels, int numSamples); // NO UPDATE REQUIRED FOR ANY OF THE STATIC/TUBE WAVESHAPERS
    void reset();
//    CDistortionBase(double sampleRate, int numChannels, int numSamples);
    CDistortionBase();
    ~CDistortionBase(){reset();}
protected:
    double m_fSampleRate = 44100;
    int m_iNumChannels = 0;
    int m_iNumSamples = 0;
    bool m_bisInitialised = false;
};

//==========================================================================
class CNoDist : public CDistortionBase
{
public:
    void process(juce::AudioSampleBuffer& buffer) override;
    CNoDist();
};

//==========================================================================
class CTanhDist : public CDistortionBase
{
public:
    void process(juce::AudioSampleBuffer& buffer) override;
//    CTanhDist(double sampleRate, int numChannels, int numSamples);
    CTanhDist(){};
};

//==========================================================================
class CAtanDist : public CDistortionBase
{
public:
//    CAtanDist(double sampleRate, int numChannels, int numSamples);
    CAtanDist(){}
    void process(juce::AudioSampleBuffer& buffer) override;
};

//==========================================================================
class CHardClipper: public CDistortionBase
{
public:
    void process(juce::AudioSampleBuffer& buffer) override;
//    CHardClipper(double sampleRate, int numChannels, int numSamples);
    CHardClipper(){}
};

//==========================================================================
class CRectifier: public CDistortionBase
{
public:
    void process(juce::AudioSampleBuffer& buffer) override;
//    CRectifier(double sampleRate, int numChannels, int numSamples);
    CRectifier(){}
};

//==========================================================================
class CSine: public CDistortionBase
{
public:
    void process(juce::AudioSampleBuffer& buffer) override;
//    CSine(double sampleRate, int numChannels, int numSamples);
    CSine(){}
};

//==========================================================================
class CTubeModel: public CDistortionBase
{
public:
    float TriodeWaveshaper(float V_gk);
    void process(juce::AudioSampleBuffer& buffer) override;
//    CTubeModel(double sampleRate, int numChannels, int numSamples);
    CTubeModel(){}
private:
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
};
#endif //GUITARAMPPLUGIN_AMPLIFIERS_H
