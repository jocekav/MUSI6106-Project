/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

enum Slope
{
    Slope_12,
    Slope_24,
    Slope_36,
    Slope_48
};

struct ChainSettings
{
    float fPeakFreq {0};
    float fPeakGainInDbs{0};
    float fPeakQuality{1};
    float fLowCutFreq{0};
    float fHighCutFreq{0};
    int iLowCutSlope{0};
    int iHighCutSlope{0};
    
    Slope lowCutSlope {Slope::Slope_12};
    Slope highCutSlope {Slope::Slope_12};
    
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState &apTreeState);

//==============================================================================
/**
*/
class EQ_v1AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    EQ_v1AudioProcessor();
    ~EQ_v1AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    juce::AudioProcessorValueTreeState apTreeState {*this, nullptr, "Parameters", createParameterLayout()};
private:
    // create aliases to make code easier to write and follow; establish filter chains
    using Filter = juce::dsp::IIR::Filter<float>;
    // chain filters together to get full dB range -> each has 12 db/oct for low/high pass so that gives 48
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    
    // define for whole mono signal path (low -> peak -> high)
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;
    
    MonoChain LeftChain, RightChain;
    
    
    enum ChainPositions
    {
        LowCut,
        Peak,
        HighCut
    };
    
    void updatePeakFilter(const ChainSettings& chainSettings);
    
    // alias for coefficients pointer in Juce dsp
    using Coeffs = Filter::CoefficientsPtr;

    static void updateCoefficients(Coeffs &oldCoeffs, const Coeffs &newCoeffs);
    
    template<typename ChainType, typename CoefficientType>
    
    void updateCutFilter(ChainType &leftLowCut, const CoefficientType &cutCoeffs, const Slope &lowCutSlope)
    {
        // reset bypassed coeffs - bypass all
            leftLowCut.template setBypassed<0>(true);
            leftLowCut.template setBypassed<1>(true);
            leftLowCut.template setBypassed<3>(true);
            leftLowCut.template setBypassed<4>(true);
        
        
            //set coeffs to be used based on chain settings
            switch (lowCutSlope)
            {
                case Slope_12:
                {
                    *leftLowCut.template get<0>().coefficients = *cutCoeffs[0];
                    leftLowCut.template setBypassed<0>(false);
                    break;
                }
                case Slope_24:
                    *leftLowCut.template get<0>().coefficients = *cutCoeffs[0];
                    leftLowCut.template setBypassed<0>(false);
                    *leftLowCut.template get<1>().coefficients = *cutCoeffs[1];
                    leftLowCut.template setBypassed<1>(false);
                    break;
                case Slope_36:
                    *leftLowCut.template get<0>().coefficients = *cutCoeffs[0];
                    leftLowCut.template setBypassed<0>(false);
                    *leftLowCut.template get<1>().coefficients = *cutCoeffs[1];
                    leftLowCut.template setBypassed<1>(false);
                    *leftLowCut.template get<2>().coefficients = *cutCoeffs[2];
                    leftLowCut.template setBypassed<2>(false);
                    break;
                case Slope_48:
                    *leftLowCut.template get<0>().coefficients = *cutCoeffs[0];
                    leftLowCut.template setBypassed<0>(false);
                    *leftLowCut.template get<1>().coefficients = *cutCoeffs[1];
                    leftLowCut.template setBypassed<1>(false);
                    *leftLowCut.template get<2>().coefficients = *cutCoeffs[2];
                    leftLowCut.template setBypassed<2>(false);
                    *leftLowCut.template get<3>().coefficients = *cutCoeffs[3];
                    leftLowCut.template setBypassed<3>(false);
                    break;
            }
    }
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQ_v1AudioProcessor)
};
