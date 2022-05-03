//
// Created by Vedant Kalbag on 09/03/22.
#include "juce_TriodeWaveShaper.h"
#include "./SGA/WaveNet.h"
#include "./SGA/WaveNetLoader.h"
#include "BaseProcessor.h"
#ifndef GUITARAMPPLUGIN_AMPLIFIERS_H
#define GUITARAMPPLUGIN_AMPLIFIERS_H
//==========================================================================
class CDistortionBase
{
public:
    virtual void process (juce::AudioSampleBuffer& buffer) = 0;
    void update(double sampleRate, int numChannels, int numSamples)
    {
        m_fSampleRate = sampleRate;
        m_iNumChannels = numChannels;
        m_iNumSamples = numSamples;
    }
    virtual void reset() = 0;
//    CDistortionBase(double sampleRate, int numChannels, int numSamples);
    CDistortionBase(){}
    virtual ~CDistortionBase() {};
protected:
    double m_fSampleRate ;
    int m_iNumChannels;
    int m_iNumSamples;
    bool m_bisInitialised = false;
    bool m_isBypassed = false;
};
//============================================================================================
//   AMPLIFIER INTERFACE
//============================================================================================
class CAmpIf : public ProcessorBase
{
public:
    CAmpIf(juce::AudioProcessorValueTreeState* apvts, int instanceNumber);
    ~CAmpIf() override;
    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params, int i);
    const juce::String getName() const override { return "AmpNode" + suffix; }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void reset() override;
    void update();
    juce::AudioProcessorValueTreeState* m_pAPVTS;
private:

    enum ampNode
    {
        BypassAmpIndex,
        WaveshaperIndex,
        AnalogAmpIndex,
        SGAIndex,

        NumAmps
    };
    ampNode actualAmp;
    ampNode previousAmp;

//    ProcessorBase *CAmp;
    CDistortionBase *CAmp;
    juce::AudioBuffer<float> mixBuffer;

    float m_fSampleRate;
    int m_iBlockLength, m_iNumChannels;
    bool isBypassed = false;
    bool isActive;
    bool ampInit = false;
    std::string suffix;
};



//==========================================================================
class CBypassAmp : public CDistortionBase
{
public:
    void process(juce::AudioSampleBuffer& buffer) override
    {
        return;
    }
    CBypassAmp();
};

////==========================================================================
class CTanhDist : public CDistortionBase
{
public:

//    CTanhDist(double sampleRate, int numChannels, int numSamples);
    CTanhDist(double sampleRate, int numChannels, int numSamples)
    {
        m_fSampleRate = sampleRate;
        m_iNumSamples = numSamples;
        m_iNumChannels = numChannels;
        auto& waveshaper = TanhProcessorChain.template get<waveshaperIndex>();
        waveshaper.functionToUse = [](float x) { return std::tanh(x); };

        auto& preGain = TanhProcessorChain.template get<preGainIndex>();
        preGain.setGainDecibels(5.0f);
        auto& postGain = TanhProcessorChain.template get<postGainIndex>();
        postGain.setGainDecibels(0.0f);

        // PREPARE TO PLAY STUFF
        auto channels = static_cast<juce::uint32> (m_iNumChannels);
        juce::dsp::ProcessSpec spec{ m_fSampleRate, static_cast<juce::uint32>(m_iNumSamples)};
        TanhProcessorChain.prepare(spec);
    }
    void reset() override
    {
        TanhProcessorChain.reset();
    }

    void process(juce::AudioSampleBuffer& buffer) override
    {
        if(m_isBypassed)
            return;
        for (auto i = 0; i < m_iNumChannels; ++i)
            buffer.clear(i, 0, buffer.getNumSamples());

        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);

        TanhProcessorChain.process(context);
    }
private:
    enum
    {
        preGainIndex,
        waveshaperIndex,
        postGainIndex
    };
    juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::WaveShaper<float>,
    juce::dsp::Gain<float>> TanhProcessorChain;

};
//================================================================================================================
//  ANALOG TUBE AMP - VEDANT
//================================================================================================================
class AnalogTubeAmp : public CDistortionBase
{
public:
    AnalogTubeAmp(double sampleRate, int numChannels, int numSamples)
    {
        m_fSampleRate = sampleRate;
        m_iNumSamples = numSamples;
        m_iNumChannels = numChannels;
        juce::dsp::IIR::Coefficients<float>::Ptr coeffs(new juce::dsp::IIR::Coefficients<float>(tonestackCalcParam(m_fSampleRate)));
        auto channels = static_cast<juce::uint32> (m_iNumChannels);
        juce::dsp::ProcessSpec spec{ m_fSampleRate, static_cast<juce::uint32>(m_iNumSamples)};
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
    void process(juce::AudioSampleBuffer& buffer) override
    {
        if(m_isBypassed)
            return;
        for (auto i = 0; i < m_iNumChannels; ++i)
            buffer.clear(i, 0, buffer.getNumSamples());

        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);

        ampProcessorChain.process(context);
    }
    void reset() override
    {
        ampProcessorChain.reset();
    }
private:
    // Tonestack Params based on the TMB Fender Bassman tone stack
    const double C1 = 0.25e-9;
    const double C2 = 20e-9;
    const double C3 = C2;
    const double R1 = 250e3;
    const double R2 = 1e6;
    const double R3 = 25e3;
    const double R4 = 56e3;

    const double t = 0.5;
    const double l = 0.5;
    const double m = 0.5;
    enum
    {
        preGainIndex,
        firstTubeIndex,
        tonestackIndex,
        driveGainIndex,
        secondTubeIndex,
        postGainIndex
    };
    // Tone Stack param calculator
    std::array<float, 8> tonestackCalcParam(double sampleRate)
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
    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;

    juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::SingleTubeProcessor<float>,
    juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>, juce::dsp::Gain<float>,
    juce::dsp::SingleTubeProcessor<float>, juce::dsp::Gain<float>> ampProcessorChain;

    bool isActive;
    std::string suffix;

};

//================================================================================================================
//  Smart Guitar Amp Vedant
//================================================================================================================

class SmartGuitarAmp : public CDistortionBase
{
public:
    SmartGuitarAmp(double sampleRate, int numChannels, int numSamples): waveNet(1, 1, 1, 1, "linear", { 1 })
    {
        m_fSampleRate = sampleRate;
        m_iNumSamples = numSamples;
        m_iNumChannels = numChannels;
        waveNet.prepareToPlay(m_iNumSamples);

        //Prepare to Play
        waveNet.prepareToPlay(m_iNumSamples);
        juce::File default_tone("/Users/vedant/Desktop/Programming/MUSI6106-Project/plugin_framework/ProcessorGraphTest/Models/bias2_high_gain.json");
        m_isBypassed = true;
        WaveNetLoader loader(default_tone);
        float levelAdjust = loader.levelAdjust;
//        int numChannels = loader.numChannels;
        int inputChannels = loader.inputChannels;
        int outputChannels = loader.outputChannels;
        int filterWidth = loader.filterWidth;
        std::vector<int> dilations = loader.dilations;
        std::string activation = loader.activation;
        waveNet.setParams(inputChannels, outputChannels, numChannels, filterWidth, activation, dilations, levelAdjust);
        loader.loadVariables(waveNet);

        isActive = true;
        m_isBypassed = false;
    }
    void update()
    {

    }
    void process(juce::AudioSampleBuffer& buffer) override
    {
        if (!isActive)
            return;
        if (isBypassed)
            return;
        this->update();

        buffer.applyGain(5.0);
        waveNet.process(buffer.getArrayOfReadPointers(), buffer.getArrayOfWritePointers(), buffer.getNumSamples());

        for (int c = 1; c < buffer.getNumChannels(); ++c)
            buffer.copyFrom(c, 0, buffer, 0, 0, buffer.getNumSamples());
    }
    void reset() override
    {

    }
private:
    enum SGAmodel
    {
        CleanIndex,
        CrunchIndex,
        HighGainIndex,
        FullDriveIndex,

        NumModels
    };
    SGAmodel actualModel, previousModel;

    bool isBypassed = false;
    bool isActive;
    bool isInit = false;
//
//    double auxSampleRate;
//    int auxSamplesPerBlock;

    float ampMaster = 1.0;
    WaveNet waveNet; // Amp Clean Channel / Lead Channel

};




#endif //GUITARAMPPLUGIN_AMPLIFIERS_H
