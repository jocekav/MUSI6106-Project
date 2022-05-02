//
// Created by Vedant Kalbag on 09/03/22.
//
#include <JuceHeader.h>
#include "ErrorDef.h"

#include "Effects_new.h"
#include "BaseProcessor.h"
#include "juce_TriodeWaveShaper.h"
#include "./sga/WaveNet.h"
#include "./sga/WaveNetLoader.h"

#ifndef GUITARAMPPLUGIN_AMPLIFIERS_H
#define GUITARAMPPLUGIN_AMPLIFIERS_H

//================================================================================================================
//  Static tanh waveshaping
//================================================================================================================
class CTanhWaveshaping : public CAmpIf
{
public:
    const juce::String getName() const override { return "TanhWaveshaping" + suffix; }
    CTanhWaveshaping();
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void reset() override;


private:
    enum
    {
        preGainIndex,
        waveshaperIndex,
        postGainIndex
    };


    juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::WaveShaper<float>,
        juce::dsp::Gain<float>> TanhProcessorChain;

    bool isActive;
    std::string suffix;
};

//================================================================================================================
//  Analog Tube Preamp Emulation Processor Nodes
//================================================================================================================
class CPreampProcessorChain : public CAmpIf
{
public:
    const juce::String getName() const override { return "AnalogAmp" + suffix; }
    CPreampProcessorChain();
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void reset() override;

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

    // Tone Stack param calculator
    std::array<float, 8> tonestackCalcParam(double sampleRate);

    //enum
    //{
    //    preGainIndex,
    //    firstTubeIndex,
    //    tonestackIndex,
    //    driveGainIndex,
    //    secondTubeIndex,
    //    postGainIndex
    //};

    //using Filter = juce::dsp::IIR::Filter<float>;
    //using FilterCoefs = juce::dsp::IIR::Coefficients<float>;

    //juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::SingleTubeProcessor<float>,
    //    juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>, juce::dsp::Gain<float>,
    //    juce::dsp::SingleTubeProcessor<float>, juce::dsp::Gain<float>> ampProcessorChain;


    enum
    {
        preGainIndex,
        firstTubeIndex,
        postGainIndex
    };

    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;

    juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::SingleTubeProcessor<float>, juce::dsp::Gain<float>> ampProcessorChain;


    bool isActive;
    std::string suffix;
};

//================================================================================================================
//  Smart Guitar Processor Nodes
//================================================================================================================
class CSmartGuitarAmp : public ProcessorBase
{
public:
    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params, int i);
    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params);
    const juce::String getName() const override { return "SGAmp" + suffix; }
    CSmartGuitarAmp(juce::AudioProcessorValueTreeState* apvts, int instanceNumber);
    CSmartGuitarAmp();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void reset() override;
    void update();
    void resampleUp(const float** inputBuffer, float** outputBuffer, int numSamples);
    void resampleBack(float** inputBuffer, float** outputBuffer, int numSamples);
    juce::AudioProcessorValueTreeState* m_pAPVTS;
private:
    bool isBypassed = false;
    juce::LinearSmoothedValue<float> inputgain, threshold, ratio, attack, release, makeupgain;
    bool isActive;

    double auxSampleRate;

    float ampMaster = 1.0;
    WaveNet waveNet; // Amp Clean Channel / Lead Channel

    std::string suffix;
};







#endif //GUITARAMPPLUGIN_AMPLIFIERS_H
