//
// Created by Vedant Kalbag on 09/03/22.
//
#include <JuceHeader.h>
#include "ErrorDef.h"
#include "BaseProcessor.h"

#ifndef GUITARAMPPLUGIN_AMPLIFIERS_H
#define GUITARAMPPLUGIN_AMPLIFIERS_H

//==========================================================================
class CDistortionBase
{
public:
    virtual void process (juce::AudioSampleBuffer& buffer) = 0;
    void update(int numChannels, int numSamples); // NO UPDATE REQUIRED FOR ANY OF THE STATIC/TUBE WAVESHAPERS
protected:
    CDistortionBase(int numChannels, int numSamples);
    int m_iNumChannels;
    int m_iNumSamples;
};

CDistortionBase::CDistortionBase(int numChannels, int numSamples)
{
    update(numChannels, numSamples);
}

void CDistortionBase::update(int numChannels, int numSamples)
{
    m_iNumChannels = numChannels;
    m_iNumSamples = numSamples;
}

//==========================================================================
class CTanhDist : protected CDistortionBase
{
public:
    void process(juce::AudioSampleBuffer& buffer) override;
private:
    CTanhDist(int numChannels, int numSamples);
};

CTanhDist::CTanhDist(int numChannels, int numSamples) : CDistortionBase(numChannels, numSamples){}

void CTanhDist::process(juce::AudioSampleBuffer& buffer)
{
    for (auto channel = 0; channel < m_iNumChannels; channel++)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (auto i = 0; i < m_iNumSamples; i++)
            channelData[i] = std::tanh (channelData[i]); //Process the sample and place back in the buffer
    }
}

//==========================================================================
class CAtanDist : protected CDistortionBase
{
public:
    CAtanDist(int numChannels, int numSamples);
    void process(juce::AudioSampleBuffer& buffer) override;
private:
};

CAtanDist::CAtanDist(int numChannels, int numSamples) : CDistortionBase(numChannels, numSamples){}

void CAtanDist::process(juce::AudioSampleBuffer &buffer)
{
    const auto factor = 2.f / juce::MathConstants<float>::pi;

    for (auto channel = 0; channel < m_iNumChannels; channel++)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (auto i = 0; i < m_iNumSamples; i++)
            channelData[i] = std::atan (channelData[i]) * factor;
    }
}


//==========================================================================
class CHardClipper: protected CDistortionBase
{
public:
    void process(juce::AudioSampleBuffer& buffer) override;
private:
    CHardClipper(int numChannels, int numSamples);
};

CHardClipper::CHardClipper(int numChannels, int numSamples): CDistortionBase(numChannels, numSamples){}

void CHardClipper::process(juce::AudioSampleBuffer &buffer)
{
    for (auto channel = 0; channel < m_iNumChannels; channel++)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (auto i = 0; i < m_iNumSamples; i++)
            channelData[i] = juce::jlimit (-1.f, 1.f, channelData[i]);
    }
}


//==========================================================================
class CRectifier: protected CDistortionBase
{
public:
    void process(juce::AudioSampleBuffer& buffer) override;
private:
    CRectifier(int numChannels, int numSamples);
};

CRectifier::CRectifier(int numChannels, int numSamples): CDistortionBase(numChannels, numSamples){}

void CRectifier::process(juce::AudioSampleBuffer &buffer)
{
    for (auto channel = 0; channel < m_iNumChannels; channel++)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (auto i = 0; i < m_iNumSamples; i++)
            channelData[i] = std::abs (channelData[i]);
    }
}


//==========================================================================
class CSine: protected CDistortionBase
{
public:
    void process(juce::AudioSampleBuffer& buffer) override;
private:
    CSine(int numChannels, int numSamples);
};

CSine::CSine(int numChannels, int numSamples) : CDistortionBase(numChannels, numSamples){}

void CSine::process(juce::AudioSampleBuffer &buffer)
{
    for (auto channel = 0; channel < m_iNumChannels; channel++)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (auto i = 0; i < m_iNumSamples; i++)
            channelData[i] = std::sin (channelData[i]);
    }
}


//==========================================================================
class CTubeModel: protected CDistortionBase
{
public:
    float TriodeWaveshaper(float V_gk);
    void process(juce::AudioSampleBuffer& buffer) override;
private:
    CTubeModel(int numChannels, int numSamples);
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
};

CTubeModel::CTubeModel(int numChannels, int numSamples): CDistortionBase(numChannels, numSamples) {}

float CTubeModel::TriodeWaveshaper(float V_gk)
{
    float E_1 = 0;
    if (V_gk < V_gk_cutoff)
    {
        E_1 = (log(1 + exp(K_p * (mu_inverse + ((-1 * V_gk) + V_ct) / (sqrt_K)))));
    }
    else
        E_1 = (log(1 + exp(K_p * (mu_inverse + ((-1 * V_gk_cutoff) + V_ct) / (sqrt_K)))));

    return 22.8f * static_cast<float> (pow(E_1, E_x) / K_g);
}

void CTubeModel::process(juce::AudioSampleBuffer &buffer)
{
    for (auto channel = 0; channel < m_iNumChannels; channel++)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (auto i = 0; i < m_iNumSamples; i++)
            channelData[i] = TriodeWaveshaper(channelData[i]);
    }
    //TODO Thiago: Is the low pass specifically for the tube required or can it be done as a post lpf/hpf within the If?
}


#endif //GUITARAMPPLUGIN_AMPLIFIERS_H
