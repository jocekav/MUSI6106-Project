/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2020 - Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             AudioProcessorGraphTutorial
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Explores the audio processor graph.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_plugin_client, juce_audio_processors,
                   juce_audio_utils, juce_core, juce_data_structures, juce_dsp,
                   juce_events, juce_graphics, juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2019, linux_make

 type:             AudioProcessor
 mainClass:        TutorialProcessor

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/


#pragma once

#include "juce_TriodeWaveShaper.h"
#include "./sga/WaveNet.h"
#include "./sga/WaveNetLoader.h"

//==============================================================================
class ProcessorBase  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ProcessorBase()
        : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo())
                                           .withOutput ("Output", juce::AudioChannelSet::stereo()))
    {}

    //==============================================================================
    void prepareToPlay (double, int) override {}
    void releaseResources() override {}
    void processBlock (juce::AudioSampleBuffer&, juce::MidiBuffer&) override {}

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override          { return nullptr; }
    bool hasEditor() const override                              { return false; }

    //==============================================================================
    const juce::String getName() const override                  { return {}; }
    bool acceptsMidi() const override                            { return false; }
    bool producesMidi() const override                           { return false; }
    double getTailLengthSeconds() const override                 { return 0; }

    //==============================================================================
    int getNumPrograms() override                                { return 0; }
    int getCurrentProgram() override                             { return 0; }
    void setCurrentProgram (int) override                        {}
    const juce::String getProgramName (int) override             { return {}; }
    void changeProgramName (int, const juce::String&) override   {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock&) override       {}
    void setStateInformation (const void*, int) override         {}

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProcessorBase)
};

//==============================================================================
class WaveNetVaProcessor : public ProcessorBase {
public:
    WaveNetVaProcessor()
        : waveNet(1, 1, 1, 1, "linear", { 1 })
    {
    }
    
    ~WaveNetVaProcessor()
    {
    }
    
    const String getName() const
    {
        return JucePlugin_Name;
    }

    bool acceptsMidi() const
    {
       #if JucePlugin_WantsMidiInput
        return true;
       #else
        return false;
       #endif
    }

    bool producesMidi() const
    {
       #if JucePlugin_ProducesMidiOutput
        return true;
       #else
        return false;
       #endif
    }

    bool isMidiEffect() const
    {
       #if JucePlugin_IsMidiEffect
        return true;
       #else
        return false;
       #endif
    }

    double getTailLengthSeconds() const
    {
        return 0.0;
    }

    int getNumPrograms()
    {
        return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                    // so this should be at least 1, even if you're not really implementing programs.
    }

    int getCurrentProgram()
    {
        return 0;
    }

    void setCurrentProgram (int index)
    {
    }

    const String getProgramName (int index)
    {
        return {};
    }

    void changeProgramName (int index, const String& newName)
    {
    }
    
    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        waveNet.prepareToPlay(samplesPerBlock);

        File default_tone("/Users/shanjiang/Documents/GitHub/SmartGuitarAmp/models/test.json");
        loadConfig(default_tone);

    }
    
    void releaseResources()
    {
        // When playback stops, you can use this as an opportunity to free up any
        // spare memory, etc.
    }

    bool isBusesLayoutSupported (const BusesLayout& layouts) const
    {
        // This is the place where you check if the layout is supported.
        // In this template code we only support mono or stereo.
        if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
         && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
            return false;

        return true;
    }
    
    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {
//        buffer.applyGain(10.0);
        waveNet.process(buffer.getArrayOfReadPointers(), buffer.getArrayOfWritePointers(), buffer.getNumSamples());
        if ( waveNet.levelAdjust != 0.0) {
            buffer.applyGain(waveNet.levelAdjust);
        }
        for (int ch = 1; ch < buffer.getNumChannels(); ++ch)
        {
            buffer.copyFrom(ch, 0, buffer, 0, 0, buffer.getNumSamples());
        }
    }
    
    void getStateInformation (MemoryBlock& destData)
    {
        // You should use this method to store your parameters in the memory block.
        // You could do that either as raw data, or use the XML or ValueTree classes
        // as intermediaries to make it easy to save and load complex data.
    }

    void setStateInformation (const void* data, int sizeInBytes)
    {
        // You should use this method to restore your parameters from this memory block,
        // whose contents will have been created by the getStateInformation() call.
    }
    
    void loadConfig(File configFile)
    {
        this -> suspendProcessing(true);
        WaveNetLoader loader(dummyVar, configFile);
        float levelAdjust = loader.levelAdjust;
        int numChannels = loader.numChannels;
        int inputChannels = loader.inputChannels;
        int outputChannels = loader.outputChannels;
        int filterWidth = loader.filterWidth;
        std::vector<int> dilations = loader.dilations;
        std::string activation = loader.activation;
        waveNet.setParams(inputChannels, outputChannels, numChannels, filterWidth, activation, dilations, levelAdjust);
        loader.loadVariables(waveNet);
        this->suspendProcessing(false);
    }
    
    File loaded_tone;
    juce::String loaded_tone_name;
    
private:
    WaveNet waveNet;
    var dummyVar;
};



class OscillatorProcessor  : public ProcessorBase
{
public:
    OscillatorProcessor()
    {
        auto& osc = processorChain.template get<oscIndex>();
        osc.initialise([](float x) { return std::sin(x); }, 128);
        osc.setFrequency(220.0f);

        auto& gain = processorChain.template get<gainIndex>();
        gain.setGainDecibels(3.0f);
    }

    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock) };
        processorChain.prepare (spec);
    }

    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        processorChain.process(context);
    }

    void reset() override
    {
        processorChain.reset();
    }
    const juce::String getName() const override { return "Oscillator"; }
private:
    enum
    {
        oscIndex,
        gainIndex
    };
    juce::dsp::ProcessorChain<juce::dsp::Oscillator<float>, juce::dsp::Gain<float>> processorChain;
};

//==============================================================================
class GainProcessor  : public ProcessorBase
{
public:
    GainProcessor()
    {
        gain.setGainDecibels (-6.0f);
    }

    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
        gain.prepare (spec);
    }

    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        gain.process (context);
    }

    void reset() override
    {
        gain.reset();
    }

    const juce::String getName() const override { return "Gain"; }

private:

    juce::dsp::Gain<float> gain;
};

//==============================================================================
class FilterProcessor  : public ProcessorBase
{
public:
    FilterProcessor() {}

    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        *filter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass (sampleRate, 1000.0f);

        juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
        filter.prepare (spec);
    }

    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        filter.process (context);
    }

    void reset() override
    {
        filter.reset();
    }

    const juce::String getName() const override { return "Filter"; }

private:
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filter;
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
class WaveshaperProcessorChain : public ProcessorBase
{
public:
    WaveshaperProcessorChain() 
    {
        auto& waveshaper = ampProcessorChain.template get<waveshaperIndex>();
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        std::array<float, 8> tonestackCoeff = tonestackCalcParam(sampleRate);
        juce::dsp::IIR::Coefficients<float>::Ptr coeffs(new juce::dsp::IIR::Coefficients<float>(tonestackCoeff));

        auto channels = static_cast<juce::uint32> (fmin(getMainBusNumInputChannels(), getMainBusNumOutputChannels()));
        juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32> (samplesPerBlock), channels };

        auto& filterLow = ampProcessorChain.template get<filterLowIndex>();                  
        filterLow.state = FilterCoefs::makeFirstOrderLowPass(getSampleRate(), 15000);
        auto& filterHigh = ampProcessorChain.template get<filterHighIndex>();
        filterHigh.state = FilterCoefs::makeFirstOrderHighPass(getSampleRate(), 45);
        auto& filterTonestack = ampProcessorChain.template get<tonestackIndex>();
        filterTonestack.state = *coeffs;

        auto& pregain = ampProcessorChain.template get<preGainIndex>();
        pregain.setGainDecibels(0);
        auto& postgain = ampProcessorChain.template get<postGainIndex>();
        postgain.setGainDecibels(6);

        ampProcessorChain.prepare(spec);
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
        
        ampProcessorChain.process(context);
    }

    void reset() override
    {
        ampProcessorChain.reset();
    }

    const juce::String getName() const override { return "WaveshaperChain"; }

private:
    // Tonestack Params
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

    std::array<float, 8> tonestackCalcParam(double fs)
    {
        std::array<float, 8> filterCoeff{};
        double c = 2 * fs;

        double b1 = t * C1 * R1 + m * C3 * R3 + l * (C1 * R2 + C2 * R2) + (C1 * R3 + C2 * R3);
        double b2 = t * (C1 * C2 * R2 * R4 + C1 * C3 * R1 * R4) - m * m * (C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3) + m * (C1 * C3 * R1 * R3 + C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3) + l * (C1 * C2 * R1 * R2 + C1 * C2 * R2 * R4 + C1 * C3 * R2 * R4) + l * m * (C1 * C3 * R2 * R3 + C2 * C3 * R2 * R3) + (C1 * C2 * R1 * R3 + C1 * C2 * R3 * R4 + C1 * C3 * R3 * R4);
        double b3 = l * m * (C1 * C2 * C3 * R1 * R2 * R3 + C1 * C2 * C3 * R2 * R3 * R4) - m * m * (C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4) + m * (C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4) + t * (C1 * C2 * C3 * R1 * R3 * R4) - t * m * (C1 * C2 * C3 * R1 * 3 * R4) + t * l * (C1 * C2 * C3 * R1 * R2 * R4);
        double a0 = 1;
        double a1 = (C1 * R1 + C1 * R3 + C2 * R3 + C2 * R4 + C3 * R4) + m * C3 * R3 + l * (C1 * R2 + C2 * R2);
        double a2 = m * (C1 * C3 * R1 * R3 - C2 * C3 * R3 * R4 + C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3) + l * m * (C1 * C3 * R2 * R3 + C2 * C3 * R2 * R3) - m * m * (C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3) + l * (C1 * C2 * R2 * R4 + C1 * C2 * R1 * R2 + C1 * C3 * R2 * R4 + C2 * C3 * R2 * R4) + (C1 * C2 * R1 * R4 + C1 * C3 * R1 * R4 + C1 * C2 * R3 * R4 + C1 * C2 * R1 * R3 + C1 * C3 * R3 * R4 + C2 * C3 * R3 * R4);
        double a3 = l * m * (C1 * C2 * C3 * R1 * R2 * R3 + C1 * C2 * C3 * R2 * R3 * R4) - m * m * (C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4) + m * (C1 * C2 * C3 * R3 * R3 * R4 + C1 * C2 * C3 * R1 * R3 * R3 - C1 * C2 * C3 * R1 * R3 * R4) + l * C1 * C2 * C3 * R1 * R2 * R4 + C1 * C2 * C3 * R1 * R3 * R4;

        double A0 = -a0 - a1 * c - a2 * c * c - a3 * c * c * c;
        double A1 = (-3 * a0) - (a1 * c) + (a2 * c * c) + (3 * a3 * c * c * c);// / A0;
        double A2 = (-3 * a0 + a1 * c + a2 * c * c - 3 * a3 * c * c * c);// / A0;
        double A3 = (-a0 + a1 * c - a2 * c * c + a3 * c * c * c);// / A0;
        double B0 = (-b1 * c - b2 * c * c - b3 * c * c * c);// / A0;
        double B1 = (-b1 * c + b2 * c * c + 3 * b3 * c * c * c);// / A0;
        double B2 = (b1 * c + b2 * c * c - 3 * b3 * c * c * c);// / A0;
        double B3 = (b1 * c - b2 * c * c + b3 * c * c * c);// / A0;


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

    enum
    {
        preGainIndex,
        waveshaperIndex,
        filterLowIndex,
        filterHighIndex,
        postGainIndex,
        tonestackIndex
    };

    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;

    
    juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::TriodeWaveShaper<float>,
        juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>, juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>, 
        juce::dsp::Gain<float>, juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>> ampProcessorChain;
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


//==============================================================================
class PreampProcessor : public ProcessorBase
{
public:
    PreampProcessor() {}

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        std::array<float,8> tonestackCoeff = tonestackCalcParam(sampleRate);
        juce::dsp::IIR::Coefficients<float>::Ptr coeffs(new juce::dsp::IIR::Coefficients<float>(tonestackCoeff));

        auto channels = static_cast<juce::uint32> (fmin(getMainBusNumInputChannels(), getMainBusNumOutputChannels()));
        *tubeLowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(getSampleRate(), 15000);
        *tubeHighPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(getSampleRate(), 45);
        *tonestack.state = *coeffs;
        
        juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32> (samplesPerBlock), channels };

        tubeLowPassFilter.prepare(spec);
        tubeHighPassFilter.prepare(spec);
        tonestack.prepare(spec);

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
                channelData[i] = 2.5f * TriodeWaveshaper(channelData[i]);
        }

        tubeLowPassFilter.process(context);
        tubeHighPassFilter.process(context);
        tonestack.process(context);
    }

    void reset() override
    {
        tubeLowPassFilter.reset();
        tubeHighPassFilter.reset();
        tonestack.reset();
    }

    const juce::String getName() const override { return "Preamp"; }

private:
    using Filter = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    Filter tubeLowPassFilter, tubeHighPassFilter, tonestack;

    // Tonestack Params
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

    std::array<float,8> tonestackCalcParam(double fs)
    {
        std::array<float, 8> filterCoeff{};
        double c = 2 * fs;

        double b1 = t * C1 * R1 + m * C3 * R3 + l * (C1 * R2 + C2 * R2) + (C1 * R3 + C2 * R3);
        double b2 = t * (C1 * C2 * R2 * R4 + C1 * C3 * R1 * R4) - m * m * (C1 * C3 * R3 * R3 + C2 * C3 * R3*R3) + m * (C1 * C3 * R1 * R3 + C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3) + l * (C1 * C2 * R1 * R2 + C1 * C2 * R2 * R4 + C1 * C3 * R2 * R4) + l * m * (C1 * C3 * R2 * R3 + C2 * C3 * R2 * R3) + (C1 * C2 * R1 * R3 + C1 * C2 * R3 * R4 + C1 * C3 * R3 * R4);
        double b3 = l * m * (C1 * C2 * C3 * R1 * R2 * R3 + C1 * C2 * C3 * R2 * R3 * R4) - m * m * (C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4) + m * (C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4) + t * (C1 * C2 * C3 * R1 * R3 * R4) - t * m * (C1 * C2 * C3 * R1 * 3 * R4) + t * l * (C1 * C2 * C3 * R1 * R2 * R4);
        double a0 = 1;
        double a1 = (C1 * R1 + C1 * R3 + C2 * R3 + C2 * R4 + C3 * R4) + m * C3 * R3 + l * (C1 * R2 + C2 * R2);
        double a2 = m * (C1 * C3 * R1 * R3 - C2 * C3 * R3 * R4 + C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3) + l * m * (C1 * C3 * R2 * R3 + C2 * C3 * R2 * R3) - m * m * (C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3) + l * (C1 * C2 * R2 * R4 + C1 * C2 * R1 * R2 + C1 * C3 * R2 * R4 + C2 * C3 * R2 * R4) + (C1 * C2 * R1 * R4 + C1 * C3 * R1 * R4 + C1 * C2 * R3 * R4 + C1 * C2 * R1 * R3 + C1 * C3 * R3 * R4 + C2 * C3 * R3 * R4);
        double a3 = l * m * (C1 * C2 * C3 * R1 * R2 * R3 + C1 * C2 * C3 * R2 * R3 * R4) - m * m * (C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4) + m * (C1 * C2 * C3 * R3 * R3 * R4 + C1 * C2 * C3 * R1 * R3 * R3 - C1 * C2 * C3 * R1 * R3 * R4) + l * C1 * C2 * C3 * R1 * R2 * R4 + C1 * C2 * C3 * R1 * R3 * R4;

        double A0 = -a0 - a1*c - a2*c*c - a3*c*c*c;
        double A1 = (-3 * a0) - (a1 * c) + (a2 * c * c) + (3 * a3 * c * c * c);// / A0;
        double A2 = (-3 * a0 + a1 * c + a2 * c * c - 3 * a3 * c * c * c);// / A0;
        double A3 = (-a0 + a1 * c - a2 * c * c + a3 * c * c * c);// / A0;
        double B0 = (-b1 * c - b2 * c * c - b3 * c * c * c);// / A0;
        double B1 = (-b1 * c + b2 * c * c + 3 * b3 * c * c * c);// / A0;
        double B2 = (b1 * c + b2 * c * c - 3 * b3 * c * c * c);// / A0;
        double B3 = (b1 * c - b2 * c * c + b3 * c * c * c);// / A0;
      

        filterCoeff[0] = static_cast<float>(B0/A0);
        filterCoeff[1] = static_cast<float>(B1/A0);
        filterCoeff[2] = static_cast<float>(B2/A0);
        filterCoeff[3] = static_cast<float>(B3/A0);
        filterCoeff[4] = static_cast<float>(1);
        filterCoeff[5] = static_cast<float>(A1/A0);
        filterCoeff[6] = static_cast<float>(A2/A0);
        filterCoeff[7] = static_cast<float>(A3/A0);

        return filterCoeff;
    }

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

        return 22.8f * static_cast<float> (pow(E_1, E_x) / K_g);
    }
};





//==============================================================================
//==============================================================================
//==============================================================================
class TutorialProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    using AudioGraphIOProcessor = juce::AudioProcessorGraph::AudioGraphIOProcessor;
    using Node = juce::AudioProcessorGraph::Node;

    //==============================================================================
    TutorialProcessor()
        : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                           .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
          mainProcessor  (new juce::AudioProcessorGraph()),
          muteInput      (new juce::AudioParameterBool   ("mute",    "Mute Input2", true)),
          processorSlot1 (new juce::AudioParameterChoice ("slot1",   "Slot 1",     processorChoices, 0)),
          processorSlot2 (new juce::AudioParameterChoice ("slot2",   "Slot 2",     processorChoices, 0)),
          processorSlot3 (new juce::AudioParameterChoice ("slot3",   "Slot 3",     processorChoices, 0)),
          bypassSlot1    (new juce::AudioParameterBool   ("bypass1", "Bypass 1",   false)),
          bypassSlot2    (new juce::AudioParameterBool   ("bypass2", "Bypass 2",   false)),
          bypassSlot3    (new juce::AudioParameterBool   ("bypass3", "Bypass 3",   false))
    {
        
        addParameter (muteInput);

        addParameter (processorSlot1);
        addParameter (processorSlot2);
        addParameter (processorSlot3);

        addParameter (bypassSlot1);
        addParameter (bypassSlot2);
        addParameter (bypassSlot3);
    }

    //==============================================================================
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override
    {
        if (layouts.getMainInputChannelSet()  == juce::AudioChannelSet::disabled()
         || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::disabled())
            return false;

        if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
         && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
            return false;

        return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet();
    }

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        mainProcessor->setPlayConfigDetails (getMainBusNumInputChannels(),
                                             getMainBusNumOutputChannels(),
                                             sampleRate, samplesPerBlock);

        mainProcessor->prepareToPlay (sampleRate, samplesPerBlock);

        initialiseGraph();
    }

    void releaseResources() override
    {
        mainProcessor->releaseResources();
    }

    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages) override
    {
        for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
            buffer.clear (i, 0, buffer.getNumSamples());

        updateGraph();

        mainProcessor->processBlock (buffer, midiMessages);
    }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override          { return new juce::GenericAudioProcessorEditor (*this); }
    bool hasEditor() const override                              { return true; }

    //==============================================================================
    const juce::String getName() const override                  { return "Graph Tutorial"; }
    bool acceptsMidi() const override                            { return true; }
    bool producesMidi() const override                           { return true; }
    double getTailLengthSeconds() const override                 { return 0; }

    //==============================================================================
    int getNumPrograms() override                                { return 1; }
    int getCurrentProgram() override                             { return 0; }
    void setCurrentProgram (int) override                        {}
    const juce::String getProgramName (int) override             { return {}; }
    void changeProgramName (int, const juce::String&) override   {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock&) override       {}
    void setStateInformation (const void*, int) override         {}

private:
    //==============================================================================
    void initialiseGraph()
    {
        mainProcessor->clear();

        audioInputNode  = mainProcessor->addNode (std::make_unique<AudioGraphIOProcessor> (AudioGraphIOProcessor::audioInputNode));
        audioOutputNode = mainProcessor->addNode (std::make_unique<AudioGraphIOProcessor> (AudioGraphIOProcessor::audioOutputNode));
        midiInputNode   = mainProcessor->addNode (std::make_unique<AudioGraphIOProcessor> (AudioGraphIOProcessor::midiInputNode));
        midiOutputNode  = mainProcessor->addNode (std::make_unique<AudioGraphIOProcessor> (AudioGraphIOProcessor::midiOutputNode));

        connectAudioNodes();
        connectMidiNodes();
    }

    void updateGraph()
    {
        bool hasChanged = false;

        juce::Array<juce::AudioParameterChoice*> choices { processorSlot1,
                                                           processorSlot2,
                                                           processorSlot3 };

        juce::Array<juce::AudioParameterBool*> bypasses { bypassSlot1,
                                                          bypassSlot2,
                                                          bypassSlot3 };

        juce::ReferenceCountedArray<Node> slots;
        slots.add (slot1Node);
        slots.add (slot2Node);
        slots.add (slot3Node);

        for (int i = 0; i < 3; ++i)
        {
            auto& choice = choices.getReference (i);
            auto  slot   = slots  .getUnchecked (i);

            if (choice->getIndex() == 0)            // [1]
            {
                if (slot != nullptr)
                {
                    mainProcessor->removeNode (slot.get());
                    slots.set (i, nullptr);
                    hasChanged = true;
                }
            }
            else if (choice->getIndex() == 1)       // [2]
            {
                if (slot != nullptr)
                {
                    if (slot->getProcessor()->getName() == "Oscillator")
                        continue;

                    mainProcessor->removeNode (slot.get());
                }

                slots.set (i, mainProcessor->addNode (std::make_unique<OscillatorProcessor>()));
                hasChanged = true;
            }
            else if (choice->getIndex() == 2)       // [3]
            {
                if (slot != nullptr)
                {
                    if (slot->getProcessor()->getName() == "Gain")
                        continue;

                    mainProcessor->removeNode (slot.get());
                }

                slots.set (i, mainProcessor->addNode (std::make_unique<GainProcessor>()));
                hasChanged = true;
            }
            else if (choice->getIndex() == 3)       // [4]
            {
                if (slot != nullptr)
                {
                    if (slot->getProcessor()->getName() == "Filter")
                        continue;

                    mainProcessor->removeNode (slot.get());
                }

                slots.set (i, mainProcessor->addNode (std::make_unique<FilterProcessor>()));
                hasChanged = true;
            }
            else if (choice->getIndex() == 4)       // [4]
            {
                if (slot != nullptr)
                {
                    if (slot->getProcessor()->getName() == "Waveshaper")
                        continue;

                    mainProcessor->removeNode(slot.get());
                }

                slots.set(i, mainProcessor->addNode(std::make_unique<WaveshaperProcessor>()));
                hasChanged = true;
            }
            else if (choice->getIndex() == 5)       // [4]
            {
                if (slot != nullptr)
                {
                    if (slot->getProcessor()->getName() == "CabSimulator")
                        continue;

                    mainProcessor->removeNode(slot.get());
                }

                slots.set(i, mainProcessor->addNode(std::make_unique<CabSimProcessor>()));
                hasChanged = true;
            }
            else if (choice->getIndex() == 6)       // [4]
            {
                if (slot != nullptr)
                {
                    if (slot->getProcessor()->getName() == "Preamp")
                        continue;

                    mainProcessor->removeNode(slot.get());
                }

                slots.set(i, mainProcessor->addNode(std::make_unique<PreampProcessor>()));
                hasChanged = true;
            }
            else if (choice->getIndex() == 7)
            {
                if (slot != nullptr)
                {
                    if (slot->getProcessor()->getName() == "WaveshaperChain")
                        continue;

                    mainProcessor->removeNode(slot.get());
                }

                slots.set(i, mainProcessor->addNode(std::make_unique<WaveshaperProcessorChain>()));
                hasChanged = true;
            }
            else if (choice->getIndex() == 8)
            {
                if (slot != nullptr)
                {
                    if (slot->getProcessor()->getName() == "SmartGuitarAmp")
                        continue;

                    mainProcessor->removeNode(slot.get());
                }

                slots.set(i, mainProcessor->addNode(std::make_unique<WaveNetVaProcessor>()));
                hasChanged = true;
            }
        }

        if (hasChanged)
        {
            for (auto connection : mainProcessor->getConnections())     // [5]
                mainProcessor->removeConnection (connection);

            juce::ReferenceCountedArray<Node> activeSlots;

            for (auto slot : slots)
            {
                if (slot != nullptr)
                {
                    activeSlots.add (slot);                             // [6]

                    slot->getProcessor()->setPlayConfigDetails (getMainBusNumInputChannels(),
                                                                getMainBusNumOutputChannels(),
                                                                getSampleRate(), getBlockSize());
                }
            }

            if (activeSlots.isEmpty())                                  // [7]
            {
                connectAudioNodes();
            }
            else
            {
                for (int i = 0; i < activeSlots.size() - 1; ++i)        // [8]
                {
                    for (int channel = 0; channel < 2; ++channel)
                        mainProcessor->addConnection ({ { activeSlots.getUnchecked (i)->nodeID,      channel },
                                                        { activeSlots.getUnchecked (i + 1)->nodeID,  channel } });
                }

                for (int channel = 0; channel < 2; ++channel)           // [9]
                {
                    mainProcessor->addConnection ({ { audioInputNode->nodeID,         channel },
                                                    { activeSlots.getFirst()->nodeID, channel } });
                    mainProcessor->addConnection ({ { activeSlots.getLast()->nodeID,  channel },
                                                    { audioOutputNode->nodeID,        channel } });
                }
            }

            connectMidiNodes();

            for (auto node : mainProcessor->getNodes())                 // [10]
                node->getProcessor()->enableAllBuses();
        }

        for (int i = 0; i < 3; ++i)
        {
            auto  slot   = slots   .getUnchecked (i);
            auto& bypass = bypasses.getReference (i);

            if (slot != nullptr)
                slot->setBypassed (bypass->get());
        }

        audioInputNode->setBypassed (muteInput->get());

        slot1Node = slots.getUnchecked (0);
        slot2Node = slots.getUnchecked (1);
        slot3Node = slots.getUnchecked (2);
    }

    void connectAudioNodes()
    {
        for (int channel = 0; channel < 2; ++channel)
            mainProcessor->addConnection ({ { audioInputNode->nodeID,  channel },
                                            { audioOutputNode->nodeID, channel } });
    }

    void connectMidiNodes()
    {
        mainProcessor->addConnection ({ { midiInputNode->nodeID,  juce::AudioProcessorGraph::midiChannelIndex },
                                        { midiOutputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex } });
    }

    //==============================================================================
    juce::StringArray processorChoices { "Empty", "Oscillator", "Gain", "Filter", "Wavesahper", "CabSimulator", "Preamp", "WaveshaperChain", "SmartGuitarAmp"};

    std::unique_ptr<juce::AudioProcessorGraph> mainProcessor;

    juce::AudioParameterBool* muteInput;

    juce::AudioParameterChoice* processorSlot1;
    juce::AudioParameterChoice* processorSlot2;
    juce::AudioParameterChoice* processorSlot3;

    juce::AudioParameterBool* bypassSlot1;
    juce::AudioParameterBool* bypassSlot2;
    juce::AudioParameterBool* bypassSlot3;

    Node::Ptr audioInputNode;
    Node::Ptr audioOutputNode;
    Node::Ptr midiInputNode;
    Node::Ptr midiOutputNode;

    Node::Ptr slot1Node;
    Node::Ptr slot2Node;
    Node::Ptr slot3Node;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TutorialProcessor)
};
