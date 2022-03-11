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



//==============================================================================
class ProcessorBase : public juce::AudioProcessor
{
public:
    //==============================================================================
    ProcessorBase()
        : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo())
            .withOutput("Output", juce::AudioChannelSet::stereo()))
    {}

    //==============================================================================
    void prepareToPlay(double, int) override {}
    void releaseResources() override {}
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override {}

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }

    //==============================================================================
    const juce::String getName() const override { return {}; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0; }

    //==============================================================================
    int getNumPrograms() override { return 0; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessorBase)
};


//==============================================================================
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

//==============================================================================
class CabSimProcessor : public ProcessorBase
{
public:
    CabSimProcessor() {
        auto dir = juce::File::getCurrentWorkingDirectory();

        int numTries = 0;

        while (!dir.getChildFile("Resources").exists() && numTries++ < 15)
            dir = dir.getParentDirectory();

        auto& convolution = convolutionCabSim;

        convolution.loadImpulseResponse(dir.getChildFile("Resources").getChildFile("guitar_amp.wav"),
            juce::dsp::Convolution::Stereo::yes,
            juce::dsp::Convolution::Trim::no,
            1024);

    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        auto channels = static_cast<juce::uint32> (fmin(getMainBusNumInputChannels(), getMainBusNumOutputChannels()));

        juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32> (samplesPerBlock) };
        convolutionCabSim.prepare(spec);


    }

    void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {
        auto totalNumInputChannels = getTotalNumInputChannels();
        auto totalNumOutputChannels = getTotalNumOutputChannels();

        for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            buffer.clear(i, 0, buffer.getNumSamples());

        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto context = juce::dsp::ProcessContextReplacing<float>(block);
        convolutionCabSim.process(context);
    }

    void reset() override
    {
        convolutionCabSim.reset();
    }

    const juce::String getName() const override { return "CabSimulator"; }

private:
    juce::dsp::Convolution convolutionCabSim;

};



class CAmplifierDistortion //: public juce::AudioProcessor
{
public:
    enum class DistortionType
    {
        AlgorithmNone = 0,
        AlgorithmTanh,
        AlgorithmAtan,
        AlgorithmHardClipper,
        AlgorithmRectifier,
        AlgorithmSine,
        AlgorithmTubeModel
    };
//    Error_t create(CAmplifierDistortion*& pCInstance);
    Error_t resetDistortion();
    Error_t prepare(juce::dsp::ProcessSpec spec, int channels, int samplesPerBlock);
    Error_t updateProcessing();
    Error_t process(juce::AudioSampleBuffer& buffer, int numChannels, int numInputChannels, int numOutputChannels);

private:
    CAmplifierDistortion(juce::AudioParameterFloat *prmPreLP, juce::AudioParameterFloat *prmPreHP, juce::AudioParameterFloat *prmPreLP);
    ~CAmplifierDistortion();
    bool isActive = true;




    juce::AudioBuffer<float> mixBuffer;

    using Filter = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    Filter preLowPassFilter, preHighPassFilter, postLowPassFilter, postHighPassFilter;
    Filter preEmphasisFilter, postEmphasisFilter;
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

    float V_gk_cutoff = 5.5f;
    float sqrt_K = static_cast<float> (sqrt(K_vb + (V_ak * V_ak)));
    float TriodeWaveshaper(float V_gk);
};

