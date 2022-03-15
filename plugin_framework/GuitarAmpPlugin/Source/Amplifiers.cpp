//
// Created by Vedant Kalbag on 14/03/22.
//
#include "Amplifiers.h"

//==========================================================================
CDistortionBase::CDistortionBase(double sampleRate, int numChannels, int numSamples)
{
    update(sampleRate, numChannels, numSamples);
    m_bisInitialised = true;
}

void CDistortionBase::update(double sampleRate, int numChannels, int numSamples)
{
    m_fSampleRate = sampleRate;
    m_iNumChannels = numChannels;
    m_iNumSamples = numSamples;
}

void CDistortionBase::reset()
{
    m_iNumChannels = 0;
    m_iNumSamples  = 0;
    m_bisInitialised = false;
}
//==========================================================================
CNoDist::CNoDist(): CDistortionBase(m_fSampleRate,m_iNumChannels, m_iNumSamples){}

void CNoDist::process(juce::AudioSampleBuffer &buffer)
{
    return;
}

//==========================================================================
CTanhDist::CTanhDist(double sampleRate, int numChannels, int numSamples) : CDistortionBase(sampleRate, numChannels, numSamples){}

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
CAtanDist::CAtanDist(double sampleRate, int numChannels, int numSamples) : CDistortionBase(sampleRate, numChannels, numSamples) {}

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
CHardClipper::CHardClipper(double sampleRate, int numChannels, int numSamples) : CDistortionBase(sampleRate, numChannels, numSamples) {}

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
CRectifier::CRectifier(double sampleRate, int numChannels, int numSamples) : CDistortionBase(sampleRate, numChannels, numSamples) {}

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
CSine::CSine(double sampleRate, int numChannels, int numSamples) : CDistortionBase(sampleRate, numChannels, numSamples) {}

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
CTubeModel::CTubeModel(double sampleRate, int numChannels, int numSamples) : CDistortionBase(sampleRate, numChannels, numSamples) {}

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

//==========================================================================



