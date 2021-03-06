//
// Created by Vedant Kalbag on 29/03/22.
//
#include "BaseProcessor.h"
#include "juce_TriodeWaveShaper.h"
#include "./SGA/WaveNet.h"
#include "./SGA/WaveNetLoader.h"

#ifndef PROCESSORGRAPHTEST_EFFECTS_NEW_H
#define PROCESSORGRAPHTEST_EFFECTS_NEW_H
//================================================================================================================
//  EQ Processor Node
//================================================================================================================
class CEqualizerProcessor  : public ProcessorBase
{
public:

    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params, int i);
    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params);

    const juce::String getName() const override { return "EQ"+suffix; }
    CEqualizerProcessor(juce::AudioProcessorValueTreeState* apvts, int instanceNumber);

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void reset() override;
    void update();
    juce::AudioProcessorValueTreeState* m_pAPVTS;
private:
    //Filters
    juce::IIRFilter lowPass [2];
    juce::IIRFilter lowMid[2];
    juce::IIRFilter midFilter[2];
    juce::IIRFilter highMid[2];
    juce::IIRFilter highPass[2];

    juce::dsp::Gain<float> gainCorrection;
    bool isBypassed = false;
    juce::LinearSmoothedValue<float> lowPassFreq, lowPassQ, lowMidFreq, lowMidQ, lowMidGain, midFreq, midQ, midGain, highMidFreq, highMidQ, highMidGain, highPassFreq, highPassQ;
    bool isActive=false;
    std::string suffix;
};

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
    juce::dsp::Gain<float> gainCorrection;
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
    juce::dsp::Gain<float> gainCorrection;
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

    ProcessorBase *CAmp;

    double auxSampleRate;
    int auxSamplesPerBlock;

 juce::AudioBuffer<float> mixBuffer;
//
   using Filter = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
   Filter preLowPassFilter, preHighPassFilter, postLowPassFilter, postHighPassFilter;
   Filter preEmphasisFilter, postEmphasisFilter;
   Filter tubeLowPassFilter, tubeHighPassFilter;



    bool isBypassed = false;
    bool isActive;
    bool ampInit = false;
    std::string suffix;
};

//================================================================================================================
//  Dummy Amp for bypass
//================================================================================================================
class CBypassAmp : public ProcessorBase
{
public:
    const juce::String getName() const override { return "BypassAmp" + suffix; }
    CBypassAmp();
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void reset() override;
    void update();
    juce::AudioProcessorValueTreeState* m_pAPVTS;

private:

    bool isBypassed = false;
    bool isActive;
    std::string suffix;
};

//================================================================================================================
//  Static tanh waveshaping
//================================================================================================================
class CTanhWaveshaping : public ProcessorBase
{
public:
    const juce::String getName() const override { return "TanhWaveshaping" + suffix; }
    CTanhWaveshaping(); 
    CTanhWaveshaping(juce::AudioProcessorValueTreeState* apvts, int instanceNumber);
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void update();
    void reset();
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

    enum
    {
        preGainIndex,
        waveshaperIndex,
        postGainIndex
    };


    juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::WaveShaper<float>, 
        juce::dsp::Gain<float>> TanhProcessorChain;

    bool isBypassed = false;
    bool isActive;
    std::string suffix;
};

//================================================================================================================
//  Analog Tube Preamp Emulation Processor Nodes
//================================================================================================================
class CAmpAnalogUsBlues : public ProcessorBase
{
public:
    const juce::String getName() const override { return "AnalogAmp" + suffix; }
    CAmpAnalogUsBlues();
    CAmpAnalogUsBlues(juce::AudioProcessorValueTreeState* apvts, int instanceNumber);
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void update();
    void reset();
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

    bool isBypassed = false;
    bool isActive;
    std::string suffix;
};




//================================================================================================================
//  Smart Guitar Processor Nodes
//================================================================================================================
class CSmartGuitarAmp : public ProcessorBase
{
public:
    const juce::String getName() const override { return "SGAmp" + suffix; }
    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params, int i);
    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params);
    CSmartGuitarAmp(juce::AudioProcessorValueTreeState* apvts, int instanceNumber);
    CSmartGuitarAmp();

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

    double auxSampleRate;
    int auxSamplesPerBlock;

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

//================================================================================================================
//  Delay Processor Node
//================================================================================================================

class CDelayProcessor  : public ProcessorBase
{
public:
    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params, int i);
    static void addToParameterLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &params);
    const juce::String getName() const override { return "Delay"+suffix; }
    CDelayProcessor(juce::AudioProcessorValueTreeState* apvts, int instanceNumber);
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;
    void reset() override;
    void update();
    juce::AudioProcessorValueTreeState* m_pAPVTS;
private:
//    juce::dsp::Phaser<float> Phaser;
//    juce::AudioSampleBuffer delayBuffer;
    int delayBufferSamples;
    int delayBufferChannels;
    int delayWritePosition;
    float m_fSampleRate = 0.f;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayLine;
    bool isBypassed = false;
    juce::LinearSmoothedValue<float> delaytime, feedback, blend;
    bool isActive;
    std::string suffix;
    float delayInSamples = 0, Blend = 0;
};


#endif //PROCESSORGRAPHTEST_EFFECTS_NEW_H
