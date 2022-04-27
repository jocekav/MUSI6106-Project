//
// Created by Vedant Kalbag on 29/03/22.
//

#include "Effects_new.h"

//================================================================================================================
//  EQ Processor Node
//================================================================================================================

CEqualizerProcessor::CEqualizerProcessor(juce::AudioProcessorValueTreeState *apvts, int instanceNumber)
{
    m_pAPVTS = apvts;
    suffix = "_" + std::to_string(instanceNumber);
//    this->update();
}
void CEqualizerProcessor::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params, int i = 0)
{
    std::function<juce::String(float,int)> valueToTextFunction=[](float x, int l) {return juce::String(x,4);};
    std::function<float(const juce::String&)> textToValueFunction=[](const juce::String& str) {return str.getFloatValue();};

    std::string num = std::to_string(i);
    // TODO: ADD EQ PARAMS
    std::string byp = "EqualizerBypass_" + num;
    std::string lpf = "EqualizerLPF_" + num;
    std::string lpq = "EqualizerLPFQ_" + num;
    std::string hpf = "EqualizerHPF_" + num;
    std::string hpq = "EqualizerHPFQ_" + num;
    std::string lomidf = "EqualizerLMF_" + num;
    std::string lomidq = "EqualizerLMQ_" + num;
    std::string lomidgain = "EqualizerLMGain_" + num;
    std::string midf = "EqualizerMF_" + num;
    std::string midq = "EqualizerMQ_" + num;
    std::string midgain = "EqualizerMGain_" + num;
    std::string himidf = "EqualizerHMF_" + num;
    std::string himidq = "EqualizerHMQ_" + num;
    std::string himidgain = "EqualizerHMGain_" + num;


    params.push_back(std::make_unique<juce::AudioParameterBool>(byp, "Bypass", false));
    //LowPass
    params.push_back(std::make_unique<juce::AudioParameterFloat>(lpf,"Low Pass Cutoff Frequency",juce::NormalisableRange<float>(20.0f,22000.0f,1.0f,0.95f),20000.0f,"Hz",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(lpq,"Low Pass Q",juce::NormalisableRange<float>(0.0f,20.0f,1.0f,0.25f),1.0f,"Q",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));

    //High Pass
    params.push_back(std::make_unique<juce::AudioParameterFloat>(hpf,"High Pass Cutoff Frequency",juce::NormalisableRange<float>(20.0f,22000.0f,1.0f,0.2f),20.0f,"Hz",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(hpq,"High Pass Q",juce::NormalisableRange<float>(0.0f,20.0f,1.0f,0.25f),1.0f,"Q",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));

    //Low Mid
    params.push_back(std::make_unique<juce::AudioParameterFloat>(lomidf,"Low-Mid Centre Frequency",juce::NormalisableRange<float>(20.0f,300.0f,1.0f,0.8f),200.0f,"Hz",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(lomidq,"Low-Mid Q",juce::NormalisableRange<float>(0.0f,20.0f,1.0f,0.25f),1.0f,"Q",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(lomidgain,"Low-Mid Gain",juce::NormalisableRange<float>(-40.0f,40.0f),0.0f,"dB",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));

    //Mid
    params.push_back(std::make_unique<juce::AudioParameterFloat>(midf,"Mid Centre Frequency",juce::NormalisableRange<float>(250.0f,2500.0f,1.0f,0.5f),800.0f,"Hz",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(midq,"Mid Q",juce::NormalisableRange<float>(0.0f,20.0f,1.0f,0.25f),1.0f,"Q",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(midgain,"Mid Gain",juce::NormalisableRange<float>(-40.0f,40.0f),0.0f,"dB",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));

    //High Mid
    params.push_back(std::make_unique<juce::AudioParameterFloat>(himidf,"High-Mid Centre Frequency",juce::NormalisableRange<float>(2000.0f,20000.0f,1.0f,0.5f),4000.0f,"Hz",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(himidq,"High-Mid Q",juce::NormalisableRange<float>(0.0f,20.0f,1.0f,0.25f),1.0f,"Q",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(himidgain,"High-Mid Gain",juce::NormalisableRange<float>(-40.0f,40.0f),0.0f,"dB",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));

}

void CEqualizerProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    update();
    reset();
    isActive=true;
}

void CEqualizerProcessor::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer &)
{
    this->update();
    if(!isActive)
        return;
    if(isBypassed)
        return;
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();
    auto numChannels = juce::jmin(totalNumInputChannels,totalNumOutputChannels);
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        auto channelMaxVal = 0.0f;
        //Filter Audio
        lowPass[channel].processSamples(channelData, numSamples);
        highPass[channel].processSamples(channelData, numSamples);
        lowMid[channel].processSamples(channelData, numSamples);
        midFilter[channel].processSamples(channelData, numSamples);
        highMid[channel].processSamples(channelData, numSamples);
    }
}

void CEqualizerProcessor::reset()
{
    // Reset DSP parameters
    for(int channel=0;channel<2;++channel)
    {
        lowPass[channel].reset();
        highPass[channel].reset();
        lowMid[channel].reset();
        midFilter[channel].reset();
        highMid[channel].reset();
    }
}

void CEqualizerProcessor::update()
{

    isBypassed = static_cast<bool>(m_pAPVTS->getRawParameterValue("EqualizerBypass" + suffix)->load());


    lowPassFreq.setTargetValue(m_pAPVTS->getRawParameterValue("EqualizerLPF" + suffix)->load());
    lowPassQ.setTargetValue(m_pAPVTS->getRawParameterValue("EqualizerLPFQ" + suffix)->load());

    highPassFreq.setTargetValue(m_pAPVTS->getRawParameterValue("EqualizerHPF" + suffix)->load());
    highPassQ.setTargetValue(m_pAPVTS->getRawParameterValue("EqualizerHPFQ" + suffix)->load());

    lowMidFreq.setTargetValue(m_pAPVTS->getRawParameterValue("EqualizerLMF" + suffix)->load());
    lowMidQ.setTargetValue(m_pAPVTS->getRawParameterValue("EqualizerLMQ" + suffix)->load());
    lowMidGain.setTargetValue(m_pAPVTS->getRawParameterValue("EqualizerLMGain" + suffix)->load());

    midFreq.setTargetValue(m_pAPVTS->getRawParameterValue("EqualizerMF" + suffix)->load());
    midQ.setTargetValue(m_pAPVTS->getRawParameterValue("EqualizerMQ" + suffix)->load());
    midGain.setTargetValue(m_pAPVTS->getRawParameterValue("EqualizerMGain" + suffix)->load());

    highMidFreq.setTargetValue(m_pAPVTS->getRawParameterValue("EqualizerHMF" + suffix)->load());
    highMidQ.setTargetValue(m_pAPVTS->getRawParameterValue("EqualizerHMQ" + suffix)->load());
    highMidGain.setTargetValue(m_pAPVTS->getRawParameterValue("EqualizerHMGain" + suffix)->load());
    //==============================================================================
    auto sr=getSampleRate();
    for(int channel=0;channel<2;++channel)
    {
        lowPass[channel].setCoefficients(juce::IIRCoefficients::makeLowPass (sr, lowPassFreq.getNextValue(),lowPassQ.getNextValue()));
        highPass[channel].setCoefficients(juce::IIRCoefficients::makeHighPass(sr, highPassFreq.getNextValue(), highPassQ.getNextValue()));
        lowMid[channel].setCoefficients(juce::IIRCoefficients::makePeakFilter(sr, lowMidFreq.getNextValue(), lowMidQ.getNextValue(), juce::Decibels::decibelsToGain(lowMidGain.getNextValue())));
        midFilter[channel].setCoefficients(juce::IIRCoefficients::makePeakFilter(sr, midFreq.getNextValue(), midQ.getNextValue(), juce::Decibels::decibelsToGain(midGain.getNextValue())));
        highMid[channel].setCoefficients(juce::IIRCoefficients::makePeakFilter(sr, highMidFreq.getNextValue(), highMidQ.getNextValue(), juce::Decibels::decibelsToGain(highMidGain.getNextValue())));
    }
}


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

    isBypassed = static_cast<bool>(m_pAPVTS->getRawParameterValue("CompressorBypass"+suffix)->load());
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
    isBypassed = static_cast<bool>(m_pAPVTS->getRawParameterValue("ReverbBypass"+suffix)->load());
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
    if(!isActive)
        return;
    if(isBypassed)
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
    isBypassed = static_cast<bool>(m_pAPVTS->getRawParameterValue("PhaserBypass"+suffix)->load());
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

    isBypassed = static_cast<bool>(m_pAPVTS->getRawParameterValue("NoiseGateBypass"+suffix)->load());
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


// struct containing all the words, each word has an array of the values







