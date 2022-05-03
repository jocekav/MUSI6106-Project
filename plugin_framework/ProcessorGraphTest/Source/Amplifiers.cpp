#include "Amplifiers.h"

CAmpIf::CAmpIf(juce::AudioProcessorValueTreeState *apvts, int instanceNumber)
{
    m_pAPVTS = apvts;
    auto numChannels = static_cast<juce::uint32> (fmin (getMainBusNumInputChannels(), getMainBusNumOutputChannels()));
    juce::dsp::ProcessSpec spec { getSampleRate(),static_cast<juce::uint32>(getBlockSize()), static_cast<juce::uint32>(getTotalNumInputChannels())};
    suffix = "_" + std::to_string(instanceNumber);
    this->update();
}
CAmpIf::~CAmpIf() noexcept
{
    delete CAmp;
    CAmp = nullptr;
}
void CAmpIf::update()
{
    m_fSampleRate = getSampleRate();
    m_iNumChannels = getTotalNumInputChannels();
    m_iBlockLength = getBlockSize();
    previousAmp = actualAmp;
    actualAmp = static_cast<ampNode> (m_pAPVTS->getRawParameterValue("Amp" + suffix)->load());
    if (!ampInit)
    {

        CAmp = new CTanhDist(m_fSampleRate, m_iNumChannels, m_iBlockLength);
        ampInit = true;
    }
    else if (previousAmp != actualAmp)
    {
        reset();
        delete CAmp;
        switch (actualAmp) {
//            case BypassAmpIndex :
//                CAmp = new CBypassAmp();
//
//                break;
            case WaveshaperIndex:
                CAmp = new CTanhDist(m_fSampleRate, m_iNumChannels, m_iBlockLength);
                break;
            case AnalogAmpIndex:
                CAmp = new AnalogTubeAmp(m_fSampleRate, m_iNumChannels, m_iBlockLength);
                break;
            case SGAIndex:
                CAmp = new SmartGuitarAmp(m_fSampleRate, m_iNumChannels, m_iBlockLength);
                break;
        }
        CAmp->update(m_fSampleRate, m_iNumChannels, m_iBlockLength);
//        CAmp->prepareToPlay(auxSampleRate, auxSamplesPerBlock);
    }
}

void CAmpIf::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer &)
{
    this->update();
    if (!isActive)
        return;
    if (isBypassed)
        return;
    CAmp->process(buffer);
}

void CAmpIf::reset()
{
    CAmp->reset();
}

void CAmpIf::addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params, int i)
{
    std::string num = std::to_string(i);
    std::string choice = "Amp_" + num;

    params.push_back(std::make_unique<juce::AudioParameterChoice>(choice, "Amp Model", juce::StringArray{ "BypassAmp", "TanhWaveshaping", "AnalogAmp", "SGA" }, 1));
}

void CAmpIf::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    m_fSampleRate = static_cast<float>(sampleRate);
    m_iBlockLength = samplesPerBlock;
    this->update();
    isActive = true;
}