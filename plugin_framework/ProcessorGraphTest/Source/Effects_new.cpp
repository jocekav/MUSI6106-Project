//
// Created by Vedant Kalbag on 29/03/22.
//

#include "Effects_new.h"

//================================================================================================================
//  Compressor Processor Node
//================================================================================================================
void CCompressorProcessor::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params, int i = 0)
{
    std::string num = std::to_string(i);
    std::string byp = "CompressorBypass_" + num;
    std::string ipg = "CompressorInputGain_" + num;
    std::string thr = "CompressorThreshold_" + num;
    std::string rto = "CompressorRatio_" + num;
    std::string atk = "CompressorAttack_" + num;
    std::string rel = "CompressorRelease_" + num;
    std::string mkg = "CompressorMakeupGain_" + num;

    params.push_back(std::make_unique<juce::AudioParameterBool>(byp, "Bypass", true));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ipg, "Input Gain", -30.f, 30.f, 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(thr, "Threshold", -60.f, 0.f, -6.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(rto, "Ratio", 1.f, 40.f, 1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(atk, "Attack", 0.5f, 1000.f, 10.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(rel, "Release", 0.5f, 2000.f, 10.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(mkg, "Makeup Gain", -30.f, 30.f, 0.f));
}
void CCompressorProcessor::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params)
{
    std::string byp = "CompressorBypass";
    std::string ipg = "CompressorInputGain";
    std::string thr = "CompressorThreshold";
    std::string rto = "CompressorRatio";
    std::string atk = "CompressorAttack";
    std::string rel = "CompressorRelease";
    std::string mkg = "CompressorMakeupGain";

    params.push_back(std::make_unique<juce::AudioParameterBool>(byp, "Bypass", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ipg, "Input Gain", -30.f, 30.f, 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(thr, "Threshold", -60.f, 0.f, -6.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(rto, "Ratio", 1.f, 40.f, 1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(atk, "Attack", 0.5f, 1000.f, 10.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(rel, "Release", 0.5f, 2000.f, 10.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(mkg, "Makeup Gain", -30.f, 30.f, 0.f));
}

CCompressorProcessor::CCompressorProcessor(juce::AudioProcessorValueTreeState* apvts, int instanceNumber)
{
    m_pAPVTS = apvts;
    suffix = "_" + std::to_string(instanceNumber);
    this->update();
}

void CCompressorProcessor::update()
{

    isBypassed = m_pAPVTS->getRawParameterValue("CompressorBypass"+suffix)->load();
    inputgain.setTargetValue(juce::Decibels::decibelsToGain (m_pAPVTS->getRawParameterValue("CompressorInputGain"+suffix)->load()));
    threshold.setTargetValue(m_pAPVTS->getRawParameterValue("CompressorThreshold"+suffix)->load());
    ratio.setTargetValue(m_pAPVTS->getRawParameterValue("CompressorRatio"+suffix)->load());
    attack.setTargetValue(m_pAPVTS->getRawParameterValue("CompressorAttack"+suffix)->load());
    release.setTargetValue(m_pAPVTS->getRawParameterValue("CompressorRelease"+suffix)->load());
    makeupgain.setTargetValue(juce::Decibels::decibelsToGain (m_pAPVTS->getRawParameterValue("CompressorMakeupGain"+suffix)->load()));

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
//void CGainProcessor::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params, std::string i = "0")
void CGainProcessor::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params, int i = 0)
{
    std::string num = std::to_string(i);
    std::string gn = "GainValue_" + num;
    params.push_back(std::make_unique<juce::AudioParameterFloat>(gn, "Gain", -30.f, 30.f, 0.f));
}
void CGainProcessor::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params)
{
    std::string gn = "GainValue";
    params.push_back(std::make_unique<juce::AudioParameterFloat>(gn, "Gain", -30.f, 30.f, 0.f));
}

CGainProcessor::CGainProcessor(juce::AudioProcessorValueTreeState* apvts, int instanceNumber)
{
    m_pAPVTS = apvts;
    suffix = "_" + std::to_string(instanceNumber);
    this->update();
}

void CGainProcessor::update()
{

    gain.setTargetValue(juce::Decibels::decibelsToGain (m_pAPVTS->getRawParameterValue("GainValue"+suffix)->load()));
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
void CReverbProcessor::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params, int i = 0)
{

    std::string num = std::to_string(i);
    std::string byp = "ReverbBypass_" + num;
    std::string dmp = "ReverbDamping_" + num;
    std::string rmsz = "ReverbRoomSize_" + num;
    std::string blnd = "ReverbBlend_" + num;

    params.push_back(std::make_unique<juce::AudioParameterBool>(byp, "Bypass", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(dmp, "Damping", 0.f, 1.f, 0.25f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(rmsz, "Room Size", 0.f, 1.f, 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(blnd, "Blend", 0.f, 1.f, 0.25f));

}
void CReverbProcessor::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params)
{
    std::string byp = "ReverbBypass";
    std::string dmp = "ReverbDamping";
    std::string rmsz = "ReverbRoomSize";
    std::string blnd = "ReverbBlend";

    params.push_back(std::make_unique<juce::AudioParameterBool>(byp, "Bypass", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(dmp, "Damping", 0.f, 1.f, 0.25f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(rmsz, "Room Size", 0.f, 1.f, 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(blnd, "Blend", 0.f, 1.f, 0.25f));

}

CReverbProcessor::CReverbProcessor(juce::AudioProcessorValueTreeState* apvts, int instanceNumber)
{
    m_pAPVTS = apvts;
    suffix = "_" + std::to_string(instanceNumber);
    this->update();
}

void CReverbProcessor::update()
{
    isBypassed = m_pAPVTS->getRawParameterValue("ReverbBypass"+suffix)->load();
    damping.setTargetValue(m_pAPVTS->getRawParameterValue("ReverbDamping"+suffix)->load());
    roomsize.setTargetValue(m_pAPVTS->getRawParameterValue("ReverbRoomSize"+suffix)->load());

    blend.setTargetValue(m_pAPVTS->getRawParameterValue("ReverbBlend"+suffix)->load());
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
void CPhaserProcessor::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params, int i = 0)
{
    std::string num = std::to_string(i);
    std::string byp = "PhaserBypass_" + num;
    std::string rat = "PhaserRate_" + num;
    std::string dep = "PhaserDepth_" + num;
    std::string fc = "PhaserFc_" + num;
    std::string fdbk = "PhaserFeedback_" + num;
    std::string blnd = "PhaserBlend_" + num;

    params.push_back(std::make_unique<juce::AudioParameterBool>(byp, "Bypass", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(rat, "Rate", juce::NormalisableRange<float>(0.1f, 20.f), 0.f,"Hz"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(dep, "Depth", juce::NormalisableRange<float>(0.f, 1.f), 0.25f,""));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(fc, "Centre Frequency", juce::NormalisableRange<float>(0.f, 1000.f, 1, 0.25), 25.f,"Hz"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(fdbk, "Feedback", juce::NormalisableRange<float>(-1.f, 1.f), 0.f,"%"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(blnd, "Wet/Dry", juce::NormalisableRange<float>(0.f, 1.f), 0.25f,"%"));
}
void CPhaserProcessor::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params)
{
    params.push_back(std::make_unique<juce::AudioParameterBool>("PhaserBypass", "Bypass", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PhaserRate", "Rate", juce::NormalisableRange<float>(0.1f, 20.f), 0.f,"Hz"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PhaserDepth", "Depth", juce::NormalisableRange<float>(0.f, 1.f), 0.25f,""));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PhaserFc", "Centre Frequency", juce::NormalisableRange<float>(0.f, 1000.f, 1, 0.25), 25.f,"Hz"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PhaserFeedback", "Feedback", juce::NormalisableRange<float>(-1.f, 1.f), 0.f,"%"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PhaserBlend", "Wet/Dry", juce::NormalisableRange<float>(0.f, 1.f), 0.25f,"%"));
}

CPhaserProcessor::CPhaserProcessor(juce::AudioProcessorValueTreeState* apvts, int instanceNumber)
{
    m_pAPVTS = apvts;
    suffix = "_" + std::to_string(instanceNumber);
    this->update();
}

void CPhaserProcessor::update()
{
    isBypassed = m_pAPVTS->getRawParameterValue("PhaserBypass"+suffix)->load();
    rate.setTargetValue(m_pAPVTS->getRawParameterValue("PhaserRate"+suffix)->load());
    depth.setTargetValue(m_pAPVTS->getRawParameterValue("PhaserDepth"+suffix)->load());
    fc.setTargetValue(m_pAPVTS->getRawParameterValue("PhaserFc"+suffix)->load());
    feedback.setTargetValue(m_pAPVTS->getRawParameterValue("PhaserFeedback"+suffix)->load());
    blend.setTargetValue(m_pAPVTS->getRawParameterValue("PhaserBlend"+suffix)->load());

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
void CNoiseGateProcessor::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params, int i = 0)
{
    std::string num = std::to_string(i);
    std::string byp = "NoiseGateBypass_" + num;
    std::string thr = "NoiseGateThreshold_" + num;
    std::string rat = "NoiseGateRatio_" + num;
    std::string atk = "NoiseGateAttack_" + num;
    std::string rel = "NoiseGateRelease_" + num;


    params.push_back(std::make_unique<juce::AudioParameterBool>(byp, "Bypass", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(thr, "Threshold", -60.f, 0.f, -6.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(rat, "Ratio", 1.f, 40.f, 1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(atk, "Attack", 0.5f, 1000.f, 10.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(rel, "Release", 0.5f, 2000.f, 10.f));
}
void CNoiseGateProcessor::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params)
{
    params.push_back(std::make_unique<juce::AudioParameterBool>("NoiseGateBypass", "Bypass", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("NoiseGateThreshold", "Threshold", -60.f, 0.f, -6.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("NoiseGateRatio", "Ratio", 1.f, 40.f, 1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("NoiseGateAttack", "Attack", 0.5f, 1000.f, 10.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("NoiseGateRelease", "Release", 0.5f, 2000.f, 10.f));
}

CNoiseGateProcessor::CNoiseGateProcessor(juce::AudioProcessorValueTreeState* apvts, int instanceNumber)
{
    m_pAPVTS = apvts;
    suffix = "_" + std::to_string(instanceNumber);
    this->update();
}

void CNoiseGateProcessor::update()
{

    isBypassed = m_pAPVTS->getRawParameterValue("NoiseGateBypass"+suffix)->load();
    threshold.setTargetValue(m_pAPVTS->getRawParameterValue("NoiseGateThreshold"+suffix)->load());
    ratio.setTargetValue(m_pAPVTS->getRawParameterValue("NoiseGateRatio"+suffix)->load());
    attack.setTargetValue(m_pAPVTS->getRawParameterValue("NoiseGateAttack"+suffix)->load());
    release.setTargetValue(m_pAPVTS->getRawParameterValue("NoiseGateRelease"+suffix)->load());

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