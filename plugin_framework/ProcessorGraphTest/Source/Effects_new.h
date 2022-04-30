//
// Created by Vedant Kalbag on 29/03/22.
//
#include "BaseProcessor.h"
#include "juce_TriodeWaveShaper.h"
#include "./sga/WaveNet.h"
#include "./sga/WaveNetLoader.h"


#ifndef PROCESSORGRAPHTEST_EFFECTS_NEW_H
#define PROCESSORGRAPHTEST_EFFECTS_NEW_H

//================================================================================================================
//  Compressor Processor Node
//================================================================================================================

class CCompressorProcessor  : public ProcessorBase
{
public:
    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params, int i);
    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params);

    const juce::String getName() const override { return "Compressor"+suffix; }
    CCompressorProcessor(juce::AudioProcessorValueTreeState* apvts, int instanceNumber);

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void reset() override;
    void update();
    juce::AudioProcessorValueTreeState* m_pAPVTS;
private:
    juce::dsp::Compressor<float> Compressor;
    juce::dsp::Gain<float> IGain,MGain;
    bool isBypassed = false;
    juce::LinearSmoothedValue<float> inputgain, threshold, ratio, attack, release, makeupgain;
    bool isActive=false;
    std::string suffix;
};

//================================================================================================================
//  Gain Processor Node
//================================================================================================================

class CGainProcessor  : public ProcessorBase
{
public:
    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params, int i);
    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params);
    const juce::String getName() const override { return "Gain"+suffix; }
    CGainProcessor(juce::AudioProcessorValueTreeState* apvts, int instanceNumber);
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void reset() override;
    void update();
    juce::AudioProcessorValueTreeState* m_pAPVTS;
private:
    juce::dsp::Gain<float> Gain;
    juce::LinearSmoothedValue<float> gain;
    bool isActive;
    std::string suffix;
};

//================================================================================================================
//  Reverb Processor Node
//================================================================================================================
class CReverbProcessor  : public ProcessorBase
{
public:
    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params, int i);
    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params);
    const juce::String getName() const override { return "Reverb"+suffix; }
    CReverbProcessor(juce::AudioProcessorValueTreeState* apvts, int instanceNumber);
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void reset() override;
    void update();

    juce::AudioProcessorValueTreeState* m_pAPVTS;
private:
    juce::Reverb Reverb;
    juce::Reverb::Parameters reverbParams;
    bool isBypassed = false;
    juce::LinearSmoothedValue<float> blend, dry, wet, roomsize, damping;
    bool isActive;
    std::string suffix;
};

//================================================================================================================
//  Phaser Processor Node
//================================================================================================================
class CPhaserProcessor  : public ProcessorBase
{
public:
    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params, int i);
    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params);
    const juce::String getName() const override { return "Phaser"+suffix; }
    CPhaserProcessor(juce::AudioProcessorValueTreeState* apvts, int instanceNumber);
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void reset() override;
    void update();
    juce::AudioProcessorValueTreeState* m_pAPVTS;
private:
    juce::dsp::Phaser<float> Phaser;
    bool isBypassed = false;
    juce::LinearSmoothedValue<float> rate, depth, fc, feedback, blend;
    bool isActive;
    std::string suffix;
};

//================================================================================================================
//  Noise Gate Processor Node
//================================================================================================================

class CNoiseGateProcessor  : public ProcessorBase
{
public:
    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params, int i);
    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params);
    const juce::String getName() const override { return "Noise Gate"+suffix; }
    CNoiseGateProcessor(juce::AudioProcessorValueTreeState* apvts, int instanceNumber);

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void reset() override;
    void update();
    juce::AudioProcessorValueTreeState* m_pAPVTS;
private:
    juce::dsp::NoiseGate<float> NoiseGate;
//    juce::dsp::Gain<float> IGain,MGain;
    bool isBypassed = false;
    juce::LinearSmoothedValue<float> inputgain, threshold, ratio, attack, release, makeupgain;
    bool isActive;
    std::string suffix;
};

//================================================================================================================
//  Amp Interface Class
//================================================================================================================
class CAmpIf : public ProcessorBase
{
public:
    CAmpIf(juce::AudioProcessorValueTreeState* apvts, int instanceNumber);
    CAmpIf();
    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params, int i);
    const juce::String getName() const override { return "Bypass" + suffix; }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void reset();
    void update();
    juce::AudioProcessorValueTreeState* m_pAPVTS;
    //juce::StringArray processorChoices{ "Bypass", "TanhWaveshaping", "AnalogAmp", "SGAmp" };
private:

    enum ampNode
    {
        WaveshaperIndex,
        AnalogAmpIndex,
        SGAIndex,

        NumAmps
    };
    ampNode actualAmp;
    ampNode previousAmp;

    ProcessorBase *CAmp;

    double auxSampleRate;
    int auxSamplesPerBlock;

    bool isBypassed = false;
    bool isActive;
    bool ampInit = false;
    std::string suffix;
};


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

    enum
    {
        preGainIndex,
        firstTubeIndex,
        tonestackIndex,
        driveGainIndex,
        secondTubeIndex,
        postGainIndex
    };

    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;

    juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::SingleTubeProcessor<float>,
        juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>, juce::dsp::Gain<float>,
        juce::dsp::SingleTubeProcessor<float>, juce::dsp::Gain<float>> ampProcessorChain;


    //enum
    //{
    //    preGainIndex,
    //    firstTubeIndex,
    //    postGainIndex
    //};

    //using Filter = juce::dsp::IIR::Filter<float>;
    //using FilterCoefs = juce::dsp::IIR::Coefficients<float>;

    //juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::SingleTubeProcessor<float>,juce::dsp::Gain<float>> ampProcessorChain;


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

//================================================================================================================
//  Cabinet Simulator Processor Nodes
//================================================================================================================
class CabSimProcessor : public ProcessorBase
{
public:
    const juce::String getName() const override { return "CabSim" + suffix; }
    CabSimProcessor(juce::AudioProcessorValueTreeState* apvts, int instanceNumber);
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void reset() override;
    juce::AudioProcessorValueTreeState* m_pAPVTS;

private:
    bool isActive;
    std::string suffix;
    juce::dsp::Convolution convolutionCabSim;
};
//==============================================================================
#endif //PROCESSORGRAPHTEST_EFFECTS_NEW_H


