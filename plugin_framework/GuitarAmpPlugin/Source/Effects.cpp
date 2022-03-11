/*
  ==============================================================================

    Effects.cpp
    Created: 22 Feb 2022 11:04:09am
    Author:  Vedant Kalbag

  ==============================================================================
*/

#include "Effects.h"
CAmplifierIf::CAmplifierIf()
{
    addParameter (prmDrive = new juce::AudioParameterFloat ("DRIVE", "Drive", {0.f, 40.f}, 20.f, "dB"));
    addParameter (prmMix = new juce::AudioParameterFloat ("MIX", "Mix", {0.f, 100.f}, 100.f, "%"));
    juce::StringArray strArray ( {"None", "Tanh", "ATan", "Hard Clipper", "Rectifier", "Sine", "Tube"});
    addParameter (m_distortionType = new juce::AudioParameterChoice ("TYPE", "Type", strArray, 1));

    // -------------------------------------------------------------------------
    addParameter (prmPreLP = new juce::AudioParameterFloat ("PRELP", "Pre Low-Pass", {10.f, 20000.f, 0.f, 0.5f}, 20000.f, "Hz"));
    addParameter (prmPreHP = new juce::AudioParameterFloat ("PREHP", "Pre High-Pass", {10.f, 20000.f, 0.f, 0.5f}, 20.f, "Hz"));
    addParameter (prmPostLP = new juce::AudioParameterFloat ("POSTLP", "Post Low-Pass", {10.f, 20000.f, 0.f, 0.5f}, 20000.f, "Hz"));
    addParameter (prmPostHP = new juce::AudioParameterFloat ("POSTHP", "Post High-Pass", {10.f, 20000.f, 0.f, 0.5f}, 20.f, "Hz"));

    // -------------------------------------------------------------------------
    addParameter (prmEPfreq = new juce::AudioParameterFloat ("EPFREQ", "Emph freq", {10.f, 20000.f, 0.f, 0.5f}, 700.f, "Hz"));
    addParameter (prmEPgain = new juce::AudioParameterFloat ("EPGAIN", "Emph gain", {-20.f, 20.f}, 0.f, "dB"));
}
void CAmplifierIf::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    distType = static_cast<DistortionAlgorithm>(m_distortionType->getIndex());
    auto numChannels = static_cast<juce::uint32> (fmin (getMainBusNumInputChannels(), getMainBusNumOutputChannels()));
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), numChannels};

    m_dSampleRate = sampleRate;
    m_iNumSamples = samplesPerBlock;
    m_iNumChannels = static_cast<int>(numChannels);

    switch(distType)
    {
        case (DistortionAlgorithm::AlgorithmTanh):
        {
            m_pCDistortion = new CTanhDist(m_dSampleRate, m_iNumChannels, m_iNumSamples);
        }
        case (DistortionAlgorithm::AlgorithmAtan):
        {
            m_pCDistortion = new CAtanDist(m_dSampleRate, m_iNumChannels, m_iNumSamples);
        }
        case (DistortionAlgorithm::AlgorithmHardClipper):
        {
            m_pCDistortion = new CHardClipper(m_dSampleRate, m_iNumChannels, m_iNumSamples);
        }
        case (DistortionAlgorithm::AlgorithmRectifier):
        {
            m_pCDistortion = new CRectifier(m_dSampleRate, m_iNumChannels, m_iNumSamples);
        }
        case (DistortionAlgorithm::AlgorithmSine):
        {
            m_pCDistortion = new CSine(m_dSampleRate, m_iNumChannels, m_iNumSamples);
        }
        case (DistortionAlgorithm::AlgorithmTubeModel):
        {
            m_pCDistortion = new CTubeModel(m_dSampleRate, m_iNumChannels, m_iNumSamples);
        }
        case (DistortionAlgorithm::AlgorithmNone):
        {
            m_pCDistortion = new CNoDist();
        }
    }

    preLowPassFilter.prepare (spec);
    preHighPassFilter.prepare (spec);
    postLowPassFilter.prepare (spec);
    postHighPassFilter.prepare (spec);

    preEmphasisFilter.prepare (spec);
    postEmphasisFilter.prepare (spec);

//    tubeLowPassFilter.prepare(spec);
//    tubeHighPassFilter.prepare(spec);

    mixBuffer.setSize (static_cast<int>(numChannels), samplesPerBlock);

    update();
    reset();

    isActive = true;
}

void CAmplifierIf::reset()
{
    //TODO: Write CAmplifierIf reset
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

void CAmplifierIf::update()
{
    driveVolume.setTargetValue (juce::Decibels::decibelsToGain (prmDrive->get()));
    auto mix = prmMix->get() * 0.01f;
    dryVolume.setTargetValue (1.f - mix);
    wetVolume.setTargetValue (mix);
    // ------------------------------------------------------------------------------------------
    *preLowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass (getSampleRate(), prmPreLP->get());
    *preHighPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass (getSampleRate(), prmPreHP->get());
    *postLowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass (getSampleRate(), prmPostLP->get());
    *postHighPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass (getSampleRate(), prmPostHP->get());
    *preEmphasisFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf (getSampleRate(), prmEPfreq->get(),1.f / sqrt (2.f), juce::Decibels::decibelsToGain (prmEPgain->get()));
    *postEmphasisFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf (getSampleRate(), prmEPfreq->get(),1.f / sqrt (2.f), juce::Decibels::decibelsToGain (-prmEPgain->get()));
//    *tubeLowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(getSampleRate(), 15000);
//    *tubeHighPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(getSampleRate(), 45);
    m_pCDistortion->update(m_dSampleRate, m_iNumChannels, m_iNumSamples);
    //TODO: NO UPDATE REQUIRED FOR DistortionBase child classes as of now, may be required with SmartGuitarAmp
}

void CAmplifierIf::processBlock(juce::AudioSampleBuffer &buffer, juce::MidiBuffer &)
{
    //TODO: Write CAmplifierIf processBlock
    if (isActive == false)
        return;
    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    const auto numChannels = fmin (totalNumInputChannels, totalNumOutputChannels);
    const auto numSamples = buffer.getNumSamples();

    update();

    // COPY DATA FROM INPUT BUFER TO MIX BUFFER FOR WET/DRY MIX
    for (auto channel = 0; channel < numChannels; channel++)
        mixBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);

    juce::ScopedNoDenormals noDenormals;
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

    // ADD DISTORTION
    // ------------------------------------------------------------------------------------------
    m_pCDistortion->process(buffer);

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