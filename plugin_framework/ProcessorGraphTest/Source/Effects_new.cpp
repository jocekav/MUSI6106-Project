//
// Created by Vedant Kalbag on 29/03/22.
//

#include "Effects_new.h"

//================================================================================================================
//  Compressor Processor Node
//================================================================================================================
void CCompressorProcessor::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params)
{
    params.push_back(std::make_unique<juce::AudioParameterBool>("CompressorBypass", "Bypass", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("CompressorInputGain", "Input Gain", -30.f, 30.f, 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("CompressorThreshold", "Threshold", -60.f, 0.f, -6.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("CompressorRatio", "Ratio", 1.f, 40.f, 1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("CompressorAttack", "Attack", 0.5f, 1000.f, 10.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("CompressorRelease", "Release", 0.5f, 2000.f, 10.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("CompressorMakeupGain", "Makeup Gain", -30.f, 30.f, 0.f));
}

CCompressorProcessor::CCompressorProcessor(juce::AudioProcessorValueTreeState* apvts)
{
    m_pAPVTS = apvts;
    this->update();
}

void CCompressorProcessor::update()
{

    isBypassed = m_pAPVTS->getRawParameterValue("CompressorBypass")->load();
    inputgain.setTargetValue(juce::Decibels::decibelsToGain (m_pAPVTS->getRawParameterValue("CompressorInputGain")->load()));
    threshold.setTargetValue(m_pAPVTS->getRawParameterValue("CompressorThreshold")->load());
    ratio.setTargetValue(m_pAPVTS->getRawParameterValue("CompressorRatio")->load());
    attack.setTargetValue(m_pAPVTS->getRawParameterValue("CompressorAttack")->load());
    release.setTargetValue(m_pAPVTS->getRawParameterValue("CompressorRelease")->load());
    makeupgain.setTargetValue(juce::Decibels::decibelsToGain (m_pAPVTS->getRawParameterValue("CompressorMakeupGain")->load()));

    Compressor.setThreshold(threshold.getNextValue());
    Compressor.setRatio(ratio.getNextValue());
    Compressor.setAttack(attack.getNextValue());
    Compressor.setRelease(release.getNextValue());

    IGain.setGainDecibels(inputgain.getNextValue());
    MGain.setGainDecibels(makeupgain.getNextValue());

}

void CCompressorProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
    Compressor.prepare (spec);
    IGain.prepare(spec);
    MGain.prepare(spec);

    isActive=true;
}

void CCompressorProcessor::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer &)
{
    this->update();
    if(!isActive)
        return;
    if(isBypassed)
        return;
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    IGain.process(context);
    Compressor.process(context);
    MGain.process(context);
}

void CCompressorProcessor::reset()
{
    IGain.reset();
    MGain.reset();
    Compressor.reset();
}

//================================================================================================================
//  Gain Processor Node
//================================================================================================================
void CGainProcessor::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params)
{
    params.push_back(std::make_unique<juce::AudioParameterFloat>("GainValue", "Gain", -30.f, 30.f, 0.f));
}

CGainProcessor::CGainProcessor(juce::AudioProcessorValueTreeState* apvts)
{
    m_pAPVTS = apvts;
    this->update();
}

void CGainProcessor::update()
{

    gain.setTargetValue(juce::Decibels::decibelsToGain (m_pAPVTS->getRawParameterValue("GainValue")->load()));
    Gain.setGainDecibels(gain.getNextValue());

}

void CGainProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
    Gain.prepare(spec);

    isActive=true;
}

void CGainProcessor::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer &)
{
    this->update();
    if(!isActive)
        return;
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    Gain.process(context);
}

void CGainProcessor::reset()
{
    Gain.reset();
}

//================================================================================================================
//  Reverb Processor Node
//================================================================================================================
void CReverbProcessor::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params)
{
    params.push_back(std::make_unique<juce::AudioParameterBool>("ReverbBypass", "Bypass", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("ReverbDamping", "Damping", 0.f, 1.f, 0.25f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("ReverbRoomSize", "Room Size", 0.f, 1.f, 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("ReverbBlend", "Blend", 0.f, 1.f, 0.25f));
}

CReverbProcessor::CReverbProcessor(juce::AudioProcessorValueTreeState* apvts)
{
    m_pAPVTS = apvts;
    this->update();
}

void CReverbProcessor::update()
{
    isBypassed = m_pAPVTS->getRawParameterValue("ReverbBypass")->load();
    damping.setTargetValue(m_pAPVTS->getRawParameterValue("ReverbDamping")->load());
    roomsize.setTargetValue(m_pAPVTS->getRawParameterValue("ReverbRoomSize")->load());

    blend.setTargetValue(m_pAPVTS->getRawParameterValue("ReverbBlend")->load());
    wet.setTargetValue(blend.getCurrentValue());
    dry.setTargetValue(1-blend.getCurrentValue());

    reverbParams.wetLevel = wet.getNextValue();
    reverbParams.dryLevel = dry.getNextValue();
    reverbParams.damping = damping.getNextValue();
    reverbParams.roomSize = roomsize.getNextValue();

    Reverb.setParameters(reverbParams);

}

void CReverbProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    Reverb.reset();
    this->update();
    isActive=true;
}

void CReverbProcessor::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer &)
{
    this->update();
    if (!isActive)
        return;
    if (isBypassed)
        return;
    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();
    const auto numChannels = fmin (totalNumInputChannels, totalNumOutputChannels);
    const auto numSamples = buffer.getNumSamples();

    if (numChannels == 1)
        Reverb.processMono (buffer.getWritePointer (0), buffer.getNumSamples());
    else if (numChannels == 2)
        Reverb.processStereo (buffer.getWritePointer (0), buffer.getWritePointer (1), buffer.getNumSamples());
}

void CReverbProcessor::reset()
{
    Reverb.reset();
}

//================================================================================================================
//  Phaser Processor Node
//================================================================================================================
void CPhaserProcessor::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params)
{
    params.push_back(std::make_unique<juce::AudioParameterBool>("PhaserBypass", "Bypass", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PhaserRate", "Rate", juce::NormalisableRange<float>(0.1f, 20.f), 0.f,"Hz"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PhaserDepth", "Depth", juce::NormalisableRange<float>(0.f, 1.f), 0.25f,""));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PhaserFc", "Centre Frequency", juce::NormalisableRange<float>(0.f, 1000.f, 1, 0.25), 25.f,"Hz"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PhaserFeedback", "Feedback", juce::NormalisableRange<float>(-1.f, 1.f), 0.f,"%"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PhaserBlend", "Wet/Dry", juce::NormalisableRange<float>(0.f, 1.f), 0.25f,"%"));
}

CPhaserProcessor::CPhaserProcessor(juce::AudioProcessorValueTreeState* apvts)
{
    m_pAPVTS = apvts;
    this->update();
}

void CPhaserProcessor::update()
{
    isBypassed = m_pAPVTS->getRawParameterValue("PhaserBypass")->load();
    rate.setTargetValue(m_pAPVTS->getRawParameterValue("PhaserRate")->load());
    depth.setTargetValue(m_pAPVTS->getRawParameterValue("PhaserDepth")->load());
    fc.setTargetValue(m_pAPVTS->getRawParameterValue("PhaserFc")->load());
    feedback.setTargetValue(m_pAPVTS->getRawParameterValue("PhaserFeedback")->load());
    blend.setTargetValue(m_pAPVTS->getRawParameterValue("PhaserBlend")->load());

    Phaser.setRate(rate.getNextValue());
    Phaser.setDepth(depth.getNextValue());
    Phaser.setCentreFrequency(fc.getNextValue());
    Phaser.setFeedback(feedback.getNextValue());
    Phaser.setMix(blend.getNextValue());
}

void CPhaserProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
    Phaser.prepare(spec);

    isActive=true;
}

void CPhaserProcessor::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer &)
{
    this->update();
    if (!isActive)
        return;
    if (isBypassed)
        return;
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    Phaser.process(context);
}

void CPhaserProcessor::reset()
{
    Phaser.reset();
}

//================================================================================================================
//  NoiseGate Processor Node
//================================================================================================================
void CNoiseGateProcessor::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params)
{
    params.push_back(std::make_unique<juce::AudioParameterBool>("NoiseGateBypass", "Bypass", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("NoiseGateThreshold", "Threshold", -60.f, 0.f, -6.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("NoiseGateRatio", "Ratio", 1.f, 40.f, 1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("NoiseGateAttack", "Attack", 0.5f, 1000.f, 10.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("NoiseGateRelease", "Release", 0.5f, 2000.f, 10.f));
}

CNoiseGateProcessor::CNoiseGateProcessor(juce::AudioProcessorValueTreeState* apvts)
{
    m_pAPVTS = apvts;
    this->update();
}

void CNoiseGateProcessor::update()
{

    isBypassed = m_pAPVTS->getRawParameterValue("NoiseGateBypass")->load();
    threshold.setTargetValue(m_pAPVTS->getRawParameterValue("NoiseGateThreshold")->load());
    ratio.setTargetValue(m_pAPVTS->getRawParameterValue("NoiseGateRatio")->load());
    attack.setTargetValue(m_pAPVTS->getRawParameterValue("NoiseGateAttack")->load());
    release.setTargetValue(m_pAPVTS->getRawParameterValue("NoiseGateRelease")->load());

    NoiseGate.setThreshold(threshold.getNextValue());
    NoiseGate.setRatio(ratio.getNextValue());
    NoiseGate.setAttack(attack.getNextValue());
    NoiseGate.setRelease(release.getNextValue());


}

void CNoiseGateProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
    NoiseGate.prepare (spec);

    isActive=true;
}

void CNoiseGateProcessor::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer &)
{
    this->update();
    if(!isActive)
        return;
    if(isBypassed)
        return;
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    NoiseGate.process(context);
}

void CNoiseGateProcessor::reset()
{
    NoiseGate.reset();
}