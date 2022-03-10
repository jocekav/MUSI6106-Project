//
// Created by Vedant Kalbag on 09/03/22.
//
#include <JuceHeader.h>
#include "ErrorDef.h"
#include "BaseProcessor.h"

#ifndef GUITARAMPPLUGIN_AMPLIFIERS_H
#define GUITARAMPPLUGIN_AMPLIFIERS_H
class CWaveShaper : public ProcessorBase
{
public:
    enum
    {
        AlgorithmNone = 0,
        AlgorithmTanh,
        AlgorithmAtan,
        AlgorithmHardClipper,
        AlgorithmRectifier,
        AlgorithmSine,
        AlgorithmTubeModel
    };
    CWaveShaper();
    ~CWaveShaper();
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void reset() override;
    void releaseResources() override;
    void processBlock (juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
private:

};

void CWaveShaper::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer &)
{
    //const auto type = m_distortionType->getIndex(); // TODO: figure out value tree state implementation to get this value
    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();
    if (type == AlgorithmTanh)
    {
        for (auto channel = 0; channel < numChannels; channel++)
        {
            auto* channelData = buffer.getWritePointer (channel);

            for (auto i = 0; i < numSamples; i++)
                channelData[i] = std::tanh (channelData[i]);
        }
    }
    else if (type == AlgorithmAtan)
    {
        const auto factor = 2.f / juce::MathConstants<float>::pi;

        for (auto channel = 0; channel < numChannels; channel++)
        {
            auto* channelData = buffer.getWritePointer (channel);

            for (auto i = 0; i < numSamples; i++)
                channelData[i] = std::atan (channelData[i]) * factor;
        }
    }
    else if (type == AlgorithmHardClipper)
    {
        for (auto channel = 0; channel < numChannels; channel++)
        {
            auto* channelData = buffer.getWritePointer (channel);

            for (auto i = 0; i < numSamples; i++)
                channelData[i] = juce::jlimit (-1.f, 1.f, channelData[i]);
        }
    }
    else if (type == AlgorithmRectifier)
    {
        for (auto channel = 0; channel < numChannels; channel++)
        {
            auto* channelData = buffer.getWritePointer (channel);

            for (auto i = 0; i < numSamples; i++)
                channelData[i] = std::abs (channelData[i]);
        }
    }
    else if (type == AlgorithmSine)
    {
        for (auto channel = 0; channel < numChannels; channel++)
        {
            auto* channelData = buffer.getWritePointer (channel);

            for (auto i = 0; i < numSamples; i++)
                channelData[i] = std::sin (channelData[i]);
        }
    }
}

CWaveShaper::CWaveShaper() {}
CWaveShaper::~CWaveShaper() {}
void CWaveShaper::prepareToPlay(double sampleRate, int samplesPerBlock) {}
void CWaveShaper::reset() {}
void CWaveShaper::releaseResources() {}
void CWaveShaper::getStateInformation(juce::MemoryBlock &) {}
void CWaveShaper::setStateInformation(const void *, int) {}
juce::AudioProcessorEditor *CWaveShaper::createEditor() {}
bool CWaveShaper::hasEditor() const {}


class CTubeModel : public ProcessorBase
{
public:
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void reset() override;
    void releaseResources() override;
    void processBlock (juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

private:
    float TriodeWaveshaper(float V_gk);
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




#endif //GUITARAMPPLUGIN_AMPLIFIERS_H
