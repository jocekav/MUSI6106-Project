//
// Created by Vedant Kalbag on 29/03/22.
//
#include "BaseProcessor.h"

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

    const juce::String getName() const override { return "Compressor"; }
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
    const juce::String getName() const override { return "Gain"; }
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
    const juce::String getName() const override { return "Reverb"; }
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
    const juce::String getName() const override { return "Gain"; }
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
    const juce::String getName() const override { return "Noise Gate"; }
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
//  Amplifier Processor Node
//================================================================================================================


#endif //PROCESSORGRAPHTEST_EFFECTS_NEW_H
