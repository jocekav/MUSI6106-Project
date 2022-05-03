//
// Created by Vedant Kalbag on 29/03/22.
//

#include "./SGA/WaveNet.h"
#include "./SGA/WaveNetLoader.h"
#include "BaseProcessor.h"
#include "Amplifiers.h"
//#include "juce_TriodeWaveShaper.h"
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
#endif //PROCESSORGRAPHTEST_EFFECTS_NEW_H
