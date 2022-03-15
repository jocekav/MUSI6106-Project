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
#include "BaseProcessor.h"
#include "Amplifiers.h"


//==============================================================================
/*
class WaveshaperProcessor : public ProcessorBase
{
public:
    WaveshaperProcessor() {}

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        auto channels = static_cast<juce::uint32> (fmin(getMainBusNumInputChannels(), getMainBusNumOutputChannels()));
        *tubeLowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(getSampleRate(), 15000);
        *tubeHighPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(getSampleRate(), 45);

        juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32> (samplesPerBlock), channels };

        tubeLowPassFilter.prepare(spec);
        tubeHighPassFilter.prepare(spec);

    }

    void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {
        const auto totalNumInputChannels = getTotalNumInputChannels();
        const auto totalNumOutputChannels = getTotalNumOutputChannels();

        const auto numChannels = fmin(totalNumInputChannels, totalNumOutputChannels);
        const auto numSamples = buffer.getNumSamples();

        for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            buffer.clear(i, 0, buffer.getNumSamples());


        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);

        for (auto channel = 0; channel < numChannels; channel++)
        {
            auto* channelData = buffer.getWritePointer(channel);

            for (auto i = 0; i < numSamples; i++)
                channelData[i] = TriodeWaveshaper(channelData[i]);
        }
        tubeLowPassFilter.process(context);
        tubeHighPassFilter.process(context);
    }

    void reset() override
    {
        tubeLowPassFilter.reset();
        tubeHighPassFilter.reset();
    }

    const juce::String getName() const override { return "Waveshaper"; }

private:
    using Filter = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
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
    float gain = 4;
    float V_gk_cutoff = 5.5f;
    float sqrt_K = static_cast<float> (sqrt(K_vb + (V_ak * V_ak)));

    float TriodeWaveshaper(float V_gk)
    {
        float E_1 = 0;
        if (V_gk < V_gk_cutoff)
        {
            E_1 = (log(1 + exp(K_p * (mu_inverse + ((-1 * gain * V_gk) + V_ct) / (sqrt_K)))));
        }
        else
            E_1 = (log(1 + exp(K_p * (mu_inverse + ((-1 * gain * V_gk_cutoff) + V_ct) / (sqrt_K)))));

        return 2.5 * 22.8f * static_cast<float> (pow(E_1, E_x) / K_g);
    }
};
*/

//================================================================================================================
//  Amplifier Processor Node
//================================================================================================================
// TODO: Move functionality from Plugin processor to this
//  1. Pre-Post LPF, HPF and Emphasis filters + params
//  2. Mix buffer + params
//  Instantiate distortion type algorithm based on distType
//  processBlock - copy to mix buff, prefiltering, distortion, post filtering, wet-dry mix
class CAmplifierIf : public ProcessorBase
{
public:
    enum class DistortionAlgorithm
    {
        AlgorithmNone = 0,
        AlgorithmTanh,
        AlgorithmAtan,
        AlgorithmHardClipper,
        AlgorithmRectifier,
        AlgorithmSine,
        AlgorithmTubeModel
    };
    CAmplifierIf();
    void update();
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void reset() override;
    void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override;
    // TODO: Add apvts and parameters for dist type, pre lp, post lp, pre hp, post hp, drive
private:
    bool isActive = false;

    double m_dSampleRate;
    int m_iNumChannels, m_iNumSamples;
    CDistortionBase *m_pCDistortion;
//    juce::AudioProcessorValueTreeState apvts;

    //PARAMS
    DistortionAlgorithm distType;
    juce::AudioParameterFloat *prmDrive, *prmMix;
    juce::AudioParameterChoice *m_distortionType;

    juce::AudioParameterFloat *prmPreLP, *prmPreHP;
    juce::AudioParameterFloat *prmPostLP, *prmPostHP;

    juce::AudioParameterFloat *prmEPfreq;
    juce::AudioParameterFloat *prmEPgain;

    juce::AudioBuffer<float> mixBuffer;
    juce::LinearSmoothedValue<float> driveVolume, dryVolume, wetVolume;

    using Filter = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    Filter preLowPassFilter, preHighPassFilter, postLowPassFilter, postHighPassFilter;
    Filter preEmphasisFilter, postEmphasisFilter;

};

//================================================================================================================
//  Gain Processor Node
//================================================================================================================
class CGainProcessor  : public ProcessorBase
{
public:
    const juce::String getName() const override { return "Gain"; }
    CGainProcessor();
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void reset() override;
    void update();
    void updateParam(float fParamValue);
private:
//    juce::dsp::Gain<float> gain;
    juce::AudioParameterFloat *m_pGain;
    juce::LinearSmoothedValue<float> volume;
    bool isActive;

};

//================================================================================================================
//  Compressor Processor Node
//================================================================================================================
class CCompressorProcessor  : public ProcessorBase
{
public:
    const juce::String getName() const override { return "Compressor"; }
    CCompressorProcessor();
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void reset() override;
    void update();
private:
    juce::dsp::Compressor<float> compressor;
    juce::AudioParameterFloat *m_pThreshold, *m_pRatio, *m_pAttack, *m_pRelease,*m_pMakeupGain;
    juce::LinearSmoothedValue<float> threshold, ratio, attack, release, makeupgain;
    bool isActive;
};

//================================================================================================================
//  Reverb Processor Node
//================================================================================================================
class CReverbProcessor  : public ProcessorBase
{
public:
    const juce::String getName() const override { return "Reverb"; }
    CReverbProcessor();
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void reset() override;
    void update();
    void updateParams(float blend, float roomSize, float damping);
private:
    juce::Reverb reverb;
    juce::Reverb::Parameters reverbParams;
//    juce::AudioParameterFloat *m_pBlend, *m_pRoomSize, *m_pDamping;
    juce::LinearSmoothedValue<float> dry, wet, roomsize, damping;
    bool isActive;
};
