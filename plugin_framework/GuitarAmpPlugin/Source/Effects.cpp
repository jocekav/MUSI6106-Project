/*
  ==============================================================================

    Effects.cpp
    Created: 22 Feb 2022 11:04:09am
    Author:  Vedant Kalbag

  ==============================================================================
*/

#include "Effects.h"

CAmplifierDistortion::CAmplifierDistortion() {

}

CAmplifierDistortion::~CAmplifierDistortion() {

}
//Error_t CAmplifierDistortion::create(CAmplifierDistortion*& pCInstance)
//{
//    pCInstance = new CAmplifierDistortion();
//}

Error_t CAmplifierDistortion::resetDistortion() {
    mixBuffer.applyGain (0.f);
    driveVolume.reset (getSampleRate(), 0.05);
    dryVolume.reset (getSampleRate(), 0.05);
    wetVolume.reset (getSampleRate(), 0.05);
    preLowPassFilter.reset();
    preHighPassFilter.reset();
    postLowPassFilter.reset();
    postHighPassFilter.reset();

    preEmphasisFilter.reset();
    postEmphasisFilter.reset();
}



Error_t CAmplifierDistortion::prepare(juce::dsp::ProcessSpec spec, int channels, int samplesPerBlock) {
    preLowPassFilter.prepare (spec);
    preHighPassFilter.prepare (spec);
    postLowPassFilter.prepare (spec);
    postHighPassFilter.prepare (spec);

    preEmphasisFilter.prepare (spec);
    postEmphasisFilter.prepare (spec);

    tubeLowPassFilter.prepare(spec);
    tubeHighPassFilter.prepare(spec);

    mixBuffer.setSize (channels, samplesPerBlock);
}

Error_t CAmplifierDistortion::process(juce::AudioSampleBuffer& buffer, int numChannels, int numInputChannels, int numOutputChannels) {
    int numSamples = buffer.getNumSamples();
    // Mix Buffer feeding
    // ------------------------------------------------------------------------------------------
    for (auto channel = 0; channel < numChannels; channel++)
        mixBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);

    // Pre-Filtering
    // ------------------------------------------------------------------------------------------
    preLowPassFilter.process (context);
    preHighPassFilter.process (context);
    preEmphasisFilter.process (context);


    // Drive Volume
    // ------------------------------------------------------------------------------------------
    driveVolume.applyGain (buffer, numSamples);

    // Static Waveshaper
    // ------------------------------------------------------------------------------------------
    const auto type = m_distortionType->getIndex();

    if (type == DistortionType::AlgorithmTanh)
    {
        for (auto channel = 0; channel < numChannels; channel++)
        {
            auto* channelData = buffer.getWritePointer (channel);

            for (auto i = 0; i < numSamples; i++)
                channelData[i] = std::tanh (channelData[i]);
        }
    }
    else if (type == DistortionType::AlgorithmAtan)
    {
        const auto factor = 2.f / juce::MathConstants<float>::pi;

        for (auto channel = 0; channel < numChannels; channel++)
        {
            auto* channelData = buffer.getWritePointer (channel);

            for (auto i = 0; i < numSamples; i++)
                channelData[i] = std::atan (channelData[i]) * factor;
        }
    }
    else if (type == DistortionType::AlgorithmHardClipper)
    {
        for (auto channel = 0; channel < numChannels; channel++)
        {
            auto* channelData = buffer.getWritePointer (channel);

            for (auto i = 0; i < numSamples; i++)
                channelData[i] = juce::jlimit (-1.f, 1.f, channelData[i]);
        }
    }
    else if (type == DistortionType::AlgorithmRectifier)
    {
        for (auto channel = 0; channel < numChannels; channel++)
        {
            auto* channelData = buffer.getWritePointer (channel);

            for (auto i = 0; i < numSamples; i++)
                channelData[i] = std::abs (channelData[i]);
        }
    }
    else if (type == DistortionType::AlgorithmSine)
    {
        for (auto channel = 0; channel < numChannels; channel++)
        {
            auto* channelData = buffer.getWritePointer (channel);

            for (auto i = 0; i < numSamples; i++)
                channelData[i] = std::sin (channelData[i]);
        }
    }
    else if (type == DistortionType::AlgorithmTubeModel)
    {

        for (auto channel = 0; channel < numChannels; channel++)
        {
            auto* channelData = buffer.getWritePointer (channel);

            for (auto i = 0; i < numSamples; i++)
                channelData[i] = TriodeWaveshaper(channelData[i]);
        }
        tubeLowPassFilter.process(context);
        tubeHighPassFilter.process(context);
    }
    // Post-Filtering
    // ------------------------------------------------------------------------------------------
    postLowPassFilter.process (context);
    postHighPassFilter.process (context);
    postEmphasisFilter.process (context);
    // Mix processing
    // ------------------------------------------------------------------------------------------
    dryVolume.applyGain (mixBuffer, numSamples);
    wetVolume.applyGain (buffer, numSamples);
    for (auto channel = 0; channel < numChannels; channel++)
        buffer.addFrom(channel, 0, mixBuffer, channel, 0, numSamples);
}

Error_t CAmplifierDistortion::updateProcessing() {
    driveVolume.setTargetValue (juce::Decibels::decibelsToGain (prmDrive->get()));

    auto mix = prmMix->get() * 0.01f;
    dryVolume.setTargetValue (1.f - mix);
    wetVolume.setTargetValue (mix);

    // ------------------------------------------------------------------------------------------
    *preLowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass (getSampleRate(), prmPreLP->get());
    *preHighPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass (getSampleRate(), prmPreHP->get());
    *postLowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass (getSampleRate(), prmPostLP->get());
    *postHighPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass (getSampleRate(), prmPostHP->get());

    *preEmphasisFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf (getSampleRate(), prmEPfreq->get(),
                                                                                   1.f / sqrt (2.f), juce::Decibels::decibelsToGain (prmEPgain->get()));
    *postEmphasisFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf (getSampleRate(), prmEPfreq->get(),
                                                                                    1.f / sqrt (2.f), juce::Decibels::decibelsToGain (-prmEPgain->get()));


    *tubeLowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(getSampleRate(), 15000);
    *tubeHighPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(getSampleRate(), 45);

}
float CAmplifierDistortion::TriodeWaveshaper(float V_gk)
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
