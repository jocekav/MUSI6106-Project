//
// Created by Vedant Kalbag on 29/03/22.
// Git version

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
    params.push_back(std::make_unique<juce::AudioParameterFloat>(lpq,"Low Pass Q",juce::NormalisableRange<float>(1.0f,20.0f,1.0f,0.25f),1.0f,"Q",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));

    //High Pass
    params.push_back(std::make_unique<juce::AudioParameterFloat>(hpf,"High Pass Cutoff Frequency",juce::NormalisableRange<float>(20.0f,22000.0f,1.0f,0.2f),20.0f,"Hz",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(hpq,"High Pass Q",juce::NormalisableRange<float>(1.0f,20.0f,1.0f,0.25f),1.0f,"Q",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));

    //Low Mid
    params.push_back(std::make_unique<juce::AudioParameterFloat>(lomidf,"Low-Mid Centre Frequency",juce::NormalisableRange<float>(20.0f,300.0f,1.0f,0.8f),200.0f,"Hz",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(lomidq,"Low-Mid Q",juce::NormalisableRange<float>(1.0f,20.0f,1.0f,0.25f),1.0f,"Q",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(lomidgain, "Low-Mid Gain", -40.0f, 40.0f, -2.0f));

    //Mid
    params.push_back(std::make_unique<juce::AudioParameterFloat>(midf,"Mid Centre Frequency",juce::NormalisableRange<float>(250.0f,2500.0f,1.0f,0.5f),800.0f,"Hz",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(midq,"Mid Q",juce::NormalisableRange<float>(1.0f,20.0f,1.0f,0.25f),1.0f,"Q",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(midgain, "Mid Gain", -40.0f, 40.0f, -2.0f));

    //High Mid
    params.push_back(std::make_unique<juce::AudioParameterFloat>(himidf,"High-Mid Centre Frequency",juce::NormalisableRange<float>(2000.0f,20000.0f,1.0f,0.5f),4000.0f,"Hz",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(himidq,"High-Mid Q",juce::NormalisableRange<float>(1.0f,20.0f,1.0f,0.25f),1.0f,"Q",juce::AudioProcessorParameter::genericParameter,valueToTextFunction,textToValueFunction));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(himidgain, "High-Mid Gain", -40.0f, 40.0f, -2.0f ));

}

void CEqualizerProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    this->reset();
    this->update();
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
    //gainCorrection.prepare(spec);
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
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    //gainCorrection.process(context);

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
        float lowMidGainInDB = lowMidGain.getNextValue();
        float midGainInDB = midGain.getNextValue();
        float highMidGainInDB = highMidGain.getNextValue();

        lowMid[channel].setCoefficients(juce::IIRCoefficients::makePeakFilter(sr, lowMidFreq.getNextValue(), lowMidQ.getNextValue(),juce::Decibels::decibelsToGain(lowMidGainInDB )));
        midFilter[channel].setCoefficients(juce::IIRCoefficients::makePeakFilter(sr, midFreq.getNextValue(), midQ.getNextValue(), juce::Decibels::decibelsToGain(midGainInDB)));
        highMid[channel].setCoefficients(juce::IIRCoefficients::makePeakFilter(sr, highMidFreq.getNextValue(), highMidQ.getNextValue(), juce::Decibels::decibelsToGain(highMidGainInDB)));
    }
    //gainCorrection.setGainDecibels(1.0f);
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
    inputgain.setTargetValue((m_pAPVTS->getRawParameterValue("CompressorInputGain"+suffix)->load()));
    threshold.setTargetValue(m_pAPVTS->getRawParameterValue("CompressorThreshold"+suffix)->load());
    ratio.setTargetValue(m_pAPVTS->getRawParameterValue("CompressorRatio"+suffix)->load());
    attack.setTargetValue(m_pAPVTS->getRawParameterValue("CompressorAttack"+suffix)->load());
    release.setTargetValue(m_pAPVTS->getRawParameterValue("CompressorRelease"+suffix)->load());
    makeupgain.setTargetValue((m_pAPVTS->getRawParameterValue("CompressorMakeupGain"+suffix)->load()));

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
    gain.setTargetValue((m_pAPVTS->getRawParameterValue("GainValue" + suffix)->load()));
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
    //gainCorrection.setGainDecibels(1.0f);
}

void CReverbProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    Reverb.reset();
    this->update();
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
    //gainCorrection.prepare(spec);
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
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    //gainCorrection.process(context);
}

void CReverbProcessor::reset()
{
    Reverb.reset();
    //gainCorrection.reset();
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

    //gainCorrection.setGainDecibels(2.12904f);
}

void CPhaserProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
    Phaser.prepare(spec);
    //gainCorrection.prepare(spec);

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
    //gainCorrection.process(context);
}

void CPhaserProcessor::reset()
{
    Phaser.reset();
    //gainCorrection.reset();
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

//================================================================================================================
//  Delay Processor Node
//================================================================================================================

CDelayProcessor::CDelayProcessor(juce::AudioProcessorValueTreeState* apvts, int instanceNumber)
{
    m_pAPVTS = apvts;
    suffix = "_" + std::to_string(instanceNumber);
    this->update();
}
void CDelayProcessor::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params, int i =0)
{
    std::string num = std::to_string(i);
    std::string byp = "DelayBypass_" + num;
    std::string dly = "DelayTime_" + num;
//    std::string fdbk = "DelayFeedback_" + num;
    std::string blnd = "DelayBlend_" + num;

    params.push_back(std::make_unique<juce::AudioParameterBool>(byp, "Bypass", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(dly, "Delay Time", juce::NormalisableRange<float>(0, 2000.f), 1000.f,"ms"));
//    params.push_back(std::make_unique<juce::AudioParameterFloat>(fdbk, "Feedback", juce::NormalisableRange<float>(0.f, 1.f), 0.f,"%"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(blnd, "Wet/Dry", juce::NormalisableRange<float>(0.f, 1.f), 1.f,"%"));
}
void CDelayProcessor::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params)
{

}
void CDelayProcessor::update()
{
    isBypassed = static_cast<bool>(m_pAPVTS->getRawParameterValue("DelayBypass"+suffix)->load());
    delaytime.setTargetValue(m_pAPVTS->getRawParameterValue("DelayTime"+suffix)->load());
    blend.setTargetValue(m_pAPVTS->getRawParameterValue("DelayBlend"+suffix)->load());

    float fDelayValue = delaytime.getNextValue();
    Blend = blend.getNextValue();
    delayInSamples = fDelayValue * (float)getSampleRate() / 1000; //Delay Time is in ms
    delayLine.setDelay(delayInSamples);
}
void CDelayProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    m_fSampleRate = sampleRate;
    delayBufferSamples = (int)(2000.f * (float)sampleRate)/1000 + 1 ; // 2000 is the max delay in ms
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
    delayLine.prepare(spec);
    delayLine.setMaximumDelayInSamples(static_cast<int>(2000.f*sampleRate/1000));
    isActive = true;
}
void CDelayProcessor::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer &)
{
    this->update();
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

// if you've got more output channels than input clears extra outputs
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const int bufferLength = buffer.getNumSamples();
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    delayLine.process(context);
//    for (int channel = 0; channel < totalNumInputChannels; ++channel)
//    {
//        auto* channelData = buffer.getWritePointer(channel);
//
//        for (int i = 0; i < bufferLength; i++)
//        {
//
////            delayLine.pushSample(channel, channelData[i]);
////            channelData[i] = channelData[i]*(1-Blend) + (Blend *delayLine.popSample(channel, delayInSamples));
//        }
//    }
//    delayBuffer.process(context);
//    const int numInputChannels = getTotalNumInputChannels();
//    const int numOutputChannels = getTotalNumOutputChannels();
//    const int numSamples = buffer.getNumSamples();
//
//    //======================================
//    this->update();
//    if(!isActive)
//        return;
//    if(isBypassed)
//        return;
//    int localWritePosition;
//
//    for (int channel = 0; channel < numInputChannels; ++channel)
//    {
//        float* channelData = buffer.getWritePointer (channel);
//        float* delayData = delayBuffer.getWritePointer (channel);
//        localWritePosition = delayWritePosition;
//
//        for (int sample = 0; sample < numSamples; ++sample)
//        {
//            const float in = channelData[sample];
//            float out = 0.0f;
//
//            float readPosition = fmodf ((float)localWritePosition - delayInSamples + (float)delayBufferSamples, delayBufferSamples);
//            int localReadPosition = static_cast<int>(floorf(readPosition));
//
//            if (localReadPosition != localWritePosition) {
//                float fraction = readPosition - (float)localReadPosition;
//                float delayed1 = delayData[(localReadPosition + 0)];
//                float delayed2 = delayData[(localReadPosition + 1) % delayBufferSamples];
//                out = delayed1 + fraction * (delayed2 - delayed1);
//
//                channelData[sample] = in + Blend * (out - in);
//                delayData[localWritePosition] = in + out * Feedback;
//            }
//
//            if (++localWritePosition >= delayBufferSamples)
//                localWritePosition -= delayBufferSamples;
//        }
//    }
//
//    delayWritePosition = localWritePosition;

    //======================================

//    for (int channel = numInputChannels; channel < numOutputChannels; ++channel)
//        buffer.clear (channel, 0, numSamples);
}
void CDelayProcessor::reset()
{
//    isActive = false;
    delayLine.reset();

}
// struct containing all the words, each word has an array of the values


//================================================================================================================

CAmpIf::CAmpIf(juce::AudioProcessorValueTreeState* apvts, int instanceNumber) : actualAmp{ BypassAmpIndex }, previousAmp{ BypassAmpIndex }
{
    m_pAPVTS = apvts;
    suffix = "_" + std::to_string(instanceNumber);


  //addParameter (prmDrive = new juce::AudioParameterFloat ("DRIVE", "Drive", {0.f, 40.f}, 20.f, "dB"));
  //  addParameter (prmMix = new juce::AudioParameterFloat ("MIX", "Mix", {0.f, 100.f}, 100.f, "%"));

    this->update();
}

CAmpIf::CAmpIf() //: actualAmp{ BypassAmpIndex }, previousAmp{ BypassAmpIndex }
{    
}

void CAmpIf::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params, int i = 0)
{
    std::string num = std::to_string(i);
    std::string choice = "Amp_" + num;

    params.push_back(std::make_unique<juce::AudioParameterChoice>(choice, "Amp Model", juce::StringArray{ "BypassAmp", "TanhWaveshaping", "AnalogAmp", "SGA" }, 0));
}

void CAmpIf::update()
{
   // driveVolume.setTargetValue (juce::Decibels::decibelsToGain (prmDrive->get()));
   //auto mix = prmMix->get() * 0.01f;
   //dryVolume.setTargetValue (1.f - mix);
   //wetVolume.setTargetValue (mix);
   // ------------------------------------------------------------------------------------------
   //*preLowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass (getSampleRate(), prmPreLP->get());
   //*preHighPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass (getSampleRate(), prmPreHP->get());
   //*postLowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass (getSampleRate(), prmPostLP->get());
   //*postHighPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass (getSampleRate(), prmPostHP->get());
   //*preEmphasisFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf (getSampleRate(), prmEPfreq->get(),1.f / sqrt (2.f), juce::Decibels::decibelsToGain (prmEPgain->get()));
   //*postEmphasisFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf (getSampleRate(), prmEPfreq->get(),1.f / sqrt (2.f), juce::Decibels::decibelsToGain (-prmEPgain->get()));
   //*tubeLowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(getSampleRate(), 15000);
   //*tubeHighPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(getSampleRate(), 45);




    previousAmp = actualAmp;

    actualAmp = static_cast<ampNode> (m_pAPVTS->getRawParameterValue("Amp" + suffix)->load());

    if (!ampInit)
    {
        CAmp = static_cast<ProcessorBase*> (new CBypassAmp());
        ampInit = true;
    }
    else if (previousAmp != actualAmp)
    {
        reset();
        //delete[] CAmp;
        switch (actualAmp) {
        case BypassAmpIndex :
            CAmp = static_cast<ProcessorBase*> (new CBypassAmp());
            break;
        case WaveshaperIndex:
            CAmp = static_cast<ProcessorBase*> (new CTanhWaveshaping());
            break;
        case AnalogAmpIndex:
            CAmp = static_cast<ProcessorBase*> (new CAmpAnalogUsBlues());
            break;
        case SGAIndex:
            CAmp = static_cast<ProcessorBase*> (new CSmartGuitarAmp());
            break;
        }
        CAmp->prepareToPlay(auxSampleRate, auxSamplesPerBlock);
    }

}

void CAmpIf::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    this->update();
    auxSampleRate = sampleRate;
    auxSamplesPerBlock = samplesPerBlock;
    auto numChannels = static_cast<juce::uint32> (fmin (getMainBusNumInputChannels(), getMainBusNumOutputChannels()));
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), numChannels};



    preLowPassFilter.prepare (spec);
    preHighPassFilter.prepare (spec);
    postLowPassFilter.prepare (spec);
    postHighPassFilter.prepare (spec);

    preEmphasisFilter.prepare (spec);
    postEmphasisFilter.prepare (spec);
    mixBuffer.setSize (static_cast<int>(numChannels), samplesPerBlock);

    CAmp->prepareToPlay(sampleRate, samplesPerBlock);
}

void CAmpIf::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiBuffer)
{
    this->update();
    if (!isActive)
        return;
    if (isBypassed)
        return;




 // Pre-Filtering
    // ------------------------------------------------------------------------------------------
    //preLowPassFilter.process (context);
    //preHighPassFilter.process (context);
    //preEmphasisFilter.process (context);


    // Drive Volume
    // ------------------------------------------------------------------------------------------
    //driveVolume.applyGain (buffer, numSamples);

    // ADD DISTORTION
    // ------------------------------------------------------------------------------------------
    //m_pCDistortion->process(buffer);

    // Post-Filtering
    // ------------------------------------------------------------------------------------------
    //postLowPassFilter.process (context);
    //postHighPassFilter.process (context);
    //postEmphasisFilter.process (context);


    // Mix processing
    // ------------------------------------------------------------------------------------------
    //dryVolume.applyGain (mixBuffer, numSamples);
    //wetVolume.applyGain (buffer, numSamples);

    
    CAmp->processBlock(buffer, midiBuffer);


    


}

void CAmpIf::reset()
{
    //mixBuffer.applyGain (0.f);

    //driveVolume.reset (getSampleRate(), 0.05);
    //dryVolume.reset (getSampleRate(), 0.05);
    //wetVolume.reset (getSampleRate(), 0.05);

    //preLowPassFilter.reset();
    //preHighPassFilter.reset();
    //postLowPassFilter.reset();
    //postHighPassFilter.reset();

    //preEmphasisFilter.reset();
    //postEmphasisFilter.reset();
    CAmp->reset();
}

//================================================================================================================

CBypassAmp::CBypassAmp()
{
}

void CBypassAmp::prepareToPlay(double sampleRate, int samplesPerBlock) 
{
}

void CBypassAmp::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiBuffer) 
{
}

void CBypassAmp::reset() 
{
}

//================================================================================================================
CTanhWaveshaping::CTanhWaveshaping()
{
    auto& waveshaper = TanhProcessorChain.template get<waveshaperIndex>();
    waveshaper.functionToUse = [](float x) { return std::tanh(x); };

    auto& preGain = TanhProcessorChain.template get<preGainIndex>();
    preGain.setGainDecibels(5.0f);
    auto& postGain = TanhProcessorChain.template get<postGainIndex>();
    postGain.setGainDecibels(0.0f);
}

void CTanhWaveshaping::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    auto channels = static_cast<juce::uint32> (fmin(getMainBusNumInputChannels(), getMainBusNumOutputChannels()));
    juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32> (samplesPerBlock) };
    TanhProcessorChain.prepare(spec);
}

void CTanhWaveshaping::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&)
{
    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    const auto numChannels = fmin(totalNumInputChannels, totalNumOutputChannels);
    const auto numSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    TanhProcessorChain.process(context);
}

void CTanhWaveshaping::reset()
{
    TanhProcessorChain.reset();
}

//================================================================================================================
CAmpAnalogUsBlues::CAmpAnalogUsBlues()
{
}

void CAmpAnalogUsBlues::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    //std::array<float, 8> tonestackCoeff = tonestackCalcParam(sampleRate);
    juce::dsp::IIR::Coefficients<float>::Ptr coeffs(new juce::dsp::IIR::Coefficients<float>(tonestackCalcParam(sampleRate)));

    auto channels = static_cast<juce::uint32> (fmin(getMainBusNumInputChannels(), getMainBusNumOutputChannels()));
    juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32> (samplesPerBlock), channels };

    auto& filterTonestack = ampProcessorChain.template get<tonestackIndex>();
    filterTonestack.state = *coeffs;

    auto& pregain = ampProcessorChain.template get<preGainIndex>();
    pregain.setGainDecibels(5);
    auto& drivegain = ampProcessorChain.template get<driveGainIndex>();
    drivegain.setGainDecibels(0);
    auto& postgain = ampProcessorChain.template get<postGainIndex>();
    postgain.setGainDecibels(-3);


    ampProcessorChain.prepare(spec);
}

void CAmpAnalogUsBlues::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&)
{
    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    const auto numChannels = fmin(totalNumInputChannels, totalNumOutputChannels);
    const auto numSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    ampProcessorChain.process(context);
}

void CAmpAnalogUsBlues::reset()
{
    ampProcessorChain.reset();
}

// Tone Stack param calculator
std::array<float, 8> CAmpAnalogUsBlues::tonestackCalcParam(double sampleRate)
{
    std::array<float, 8> filterCoeff{};
    double c = 2 * sampleRate;

    double b1 = t * C1 * R1 + m * C3 * R3 + l * (C1 * R2 + C2 * R2) + (C1 * R3 + C2 * R3);
    double b2 = t * (C1 * C2 * R2 * R4 + C1 * C3 * R1 * R4) - m * m * (C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3) + m * (C1 * C3 * R1 * R3 + C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3) + l * (C1 * C2 * R1 * R2 + C1 * C2 * R2 * R4 + C1 * C3 * R2 * R4) + l * m * (C1 * C3 * R2 * R3 + C2 * C3 * R2 * R3) + (C1 * C2 * R1 * R3 + C1 * C2 * R3 * R4 + C1 * C3 * R3 * R4);
    double b3 = l * m * (C1 * C2 * C3 * R1 * R2 * R3 + C1 * C2 * C3 * R2 * R3 * R4) - m * m * (C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4) + m * (C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4) + t * (C1 * C2 * C3 * R1 * R3 * R4) - t * m * (C1 * C2 * C3 * R1 * 3 * R4) + t * l * (C1 * C2 * C3 * R1 * R2 * R4);
    double a0 = 1;
    double a1 = (C1 * R1 + C1 * R3 + C2 * R3 + C2 * R4 + C3 * R4) + m * C3 * R3 + l * (C1 * R2 + C2 * R2);
    double a2 = m * (C1 * C3 * R1 * R3 - C2 * C3 * R3 * R4 + C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3) + l * m * (C1 * C3 * R2 * R3 + C2 * C3 * R2 * R3) - m * m * (C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3) + l * (C1 * C2 * R2 * R4 + C1 * C2 * R1 * R2 + C1 * C3 * R2 * R4 + C2 * C3 * R2 * R4) + (C1 * C2 * R1 * R4 + C1 * C3 * R1 * R4 + C1 * C2 * R3 * R4 + C1 * C2 * R1 * R3 + C1 * C3 * R3 * R4 + C2 * C3 * R3 * R4);
    double a3 = l * m * (C1 * C2 * C3 * R1 * R2 * R3 + C1 * C2 * C3 * R2 * R3 * R4) - m * m * (C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4) + m * (C1 * C2 * C3 * R3 * R3 * R4 + C1 * C2 * C3 * R1 * R3 * R3 - C1 * C2 * C3 * R1 * R3 * R4) + l * C1 * C2 * C3 * R1 * R2 * R4 + C1 * C2 * C3 * R1 * R3 * R4;

    double A0 = -a0 - a1 * c - a2 * c * c - a3 * c * c * c;
    double A1 = (-3 * a0) - (a1 * c) + (a2 * c * c) + (3 * a3 * c * c * c);
    double A2 = (-3 * a0 + a1 * c + a2 * c * c - 3 * a3 * c * c * c);
    double A3 = (-a0 + a1 * c - a2 * c * c + a3 * c * c * c);
    double B0 = (-b1 * c - b2 * c * c - b3 * c * c * c);
    double B1 = (-b1 * c + b2 * c * c + 3 * b3 * c * c * c);
    double B2 = (b1 * c + b2 * c * c - 3 * b3 * c * c * c);
    double B3 = (b1 * c - b2 * c * c + b3 * c * c * c);

    filterCoeff[0] = static_cast<float>(B0 / A0);
    filterCoeff[1] = static_cast<float>(B1 / A0);
    filterCoeff[2] = static_cast<float>(B2 / A0);
    filterCoeff[3] = static_cast<float>(B3 / A0);
    filterCoeff[4] = static_cast<float>(1);
    filterCoeff[5] = static_cast<float>(A1 / A0);
    filterCoeff[6] = static_cast<float>(A2 / A0);
    filterCoeff[7] = static_cast<float>(A3 / A0);

    return filterCoeff;
}

//================================================================================================================
//================================================================================================================
//  SmartGuitarAmp Processor Node
//================================================================================================================
void CSmartGuitarAmp::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params, int i = 0)
{
    std::string num = std::to_string(i);
    std::string choice = "SGA_" + num;

    params.push_back(std::make_unique<juce::AudioParameterChoice>(choice, "SGA Model", juce::StringArray{ "Clean", "Crunch", "High Gain", "Full Drive" }, 0));

    //params.push_back(std::make_unique<juce::AudioParameterBool>(SGAmpBypass, "SGAmpBypass", false));
    //params.push_back(std::make_unique<juce::AudioParameterFloat>(SGAmpGain, "SGAmpGain", -30.f, 30.f, 0.f));
}

void CSmartGuitarAmp::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params)
{
    //std::string SGAmpBypass = "SGAmpBypass";
    //std::string SGAmpGain = "SGAmpGain";

    //params.push_back(std::make_unique<juce::AudioParameterBool>(SGAmpBypass, "SGAmpBypass", false));
    //params.push_back(std::make_unique<juce::AudioParameterFloat>(SGAmpGain, "SGAmpGain", -30.f, 30.f, 0.f));
}

CSmartGuitarAmp::CSmartGuitarAmp(juce::AudioProcessorValueTreeState* apvts, int instanceNumber) :
    waveNet(1, 1, 1, 1, "linear", { 1 })
{
    m_pAPVTS = apvts;
    suffix = "_" + std::to_string(instanceNumber);
    this->update();
}

CSmartGuitarAmp::CSmartGuitarAmp() :
    waveNet(1, 1, 1, 1, "linear", { 1 })
{
}

void CSmartGuitarAmp::update()
{
    //previousModel = actualModel;

    ////actualModel = static_cast<SGAmodel> (m_pAPVTS->getRawParameterValue("SGA" + suffix)->load());

    //if (!isInit)
    //{
    //    //CAmp = static_cast<ProcessorBase*> (new CBypassAmp());
    //    isInit = true;
    //}
    //else if (previousModel != actualModel)
    //{
    //    reset();

    //    switch (actualModel) {
    //    case CleanIndex:
    //        //CAmp = static_cast<ProcessorBase*> (new CBypassAmp());
    //    case CrunchIndex:
    //        //CAmp = static_cast<ProcessorBase*> (new CTanhWaveshaping());
    //        break;
    //    case HighGainIndex:
    //        //CAmp = static_cast<ProcessorBase*> (new CPreampProcessorChain());
    //        break;
    //    case FullDriveIndex:
    //        //CAmp = static_cast<ProcessorBase*> (new CPreampProcessorChain());
    //        break;
    //    }
    //    prepareToPlay(auxSampleRate, auxSamplesPerBlock);
    //}

}

void CSmartGuitarAmp::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    this->update();
    auxSampleRate = sampleRate;
    auxSamplesPerBlock = samplesPerBlock;   
    waveNet.prepareToPlay(samplesPerBlock);

    auto dir = juce::File::getSpecialLocation(juce::File::currentApplicationFile);



    int numTries = 0;
    while (!dir.getChildFile("Models").exists() && numTries++ < 15)
        dir = dir.getParentDirectory();

    juce::File default_tone(dir.getChildFile("Models").getChildFile("bias2_high_gain.json"));

    //    juce::File default_tone("C:/Users/smjef/Github/MUSI6106-Project/plugin_framework/ProcessorGraphTest/Models/bias2_high_gain.json");
//    this->suspendProcessing(true);
//    WaveNetLoader loader(default_tone);
//    float levelAdjust = loader.levelAdjust;
//    int numChannels = loader.numChannels;
//    int inputChannels = loader.inputChannels;
//    int outputChannels = loader.outputChannels;
//    int filterWidth = loader.filterWidth;
//    std::vector<int> dilations = loader.dilations;
//    std::string activation = loader.activation;
//    waveNet.setParams(inputChannels, outputChannels, numChannels, filterWidth, activation, dilations, levelAdjust);
//    loader.loadVariables(waveNet);

    isActive = true;
    this->suspendProcessing(false);
}

void CSmartGuitarAmp::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&)
{
    if (!isActive)
        return;
    if (isBypassed)
        return;

    buffer.applyGain(5.0);
    waveNet.process(buffer.getArrayOfReadPointers(), buffer.getArrayOfWritePointers(), buffer.getNumSamples());

    for (int c = 1; c < buffer.getNumChannels(); ++c)
        buffer.copyFrom(c, 0, buffer, 0, 0, buffer.getNumSamples());
}

void CSmartGuitarAmp::reset()
{
    waveNet.prepareToPlay(1024);
}


//================================================================================================================
//  Cabinet Simulator Processor Node
//================================================================================================================

CabSimProcessor::CabSimProcessor(juce::AudioProcessorValueTreeState* apvts, int instanceNumber)
{
    m_pAPVTS = apvts;
    suffix = "_" + std::to_string(instanceNumber);

    auto dir = juce::File::getSpecialLocation(juce::File::currentApplicationFile);
    int numTries = 0;
    while (!dir.getChildFile("Resources").exists() && numTries++ < 15)
        dir = dir.getParentDirectory();


    auto& convolution = convolutionCabSim;

    convolution.loadImpulseResponse(dir.getChildFile("Resources").getChildFile("guitar_amp.wav"),
        juce::dsp::Convolution::Stereo::no,
        juce::dsp::Convolution::Trim::no,
        1024);
}

void CabSimProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    auto channels = static_cast<juce::uint32> (fmin(getMainBusNumInputChannels(), getMainBusNumOutputChannels()));

    juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32> (samplesPerBlock) };
    convolutionCabSim.prepare(spec);
}

void CabSimProcessor::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&)
{
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);
    convolutionCabSim.process(context);
}

void CabSimProcessor::reset()
{
    convolutionCabSim.reset();
}
