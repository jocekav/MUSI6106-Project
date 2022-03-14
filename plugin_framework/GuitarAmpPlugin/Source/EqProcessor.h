//
//  EqProcessor.h
//  GuitarAmpPlugin
//
//  Created by Jocelyn Kavanagh on 3/9/22.
//  Copyright © 2022 LL. All rights reserved.
//
#include <JuceHeader.h>

#ifndef EqProcessor_h
#define EqProcessor_h

class ProcessorBase;
#include "BaseProcessor.h"

//==============================================================================


// EQ vars and class
//==============================================================================
enum Slope
{
    Slope_12,
    Slope_24,
    Slope_36,
    Slope_48
};

struct ChainSettings
{
    float fPeakFreq {7500};
    float fPeakGainInDbs{1};
    float fPeakQuality{1};
    float fLowCutFreq{20};
    float fHighCutFreq{20000};
    int iLowCutSlope{0};
    int iHighCutSlope{0};
    
    Slope lowCutSlope {Slope::Slope_12};
    Slope highCutSlope {Slope::Slope_12};
    
};
//
//ChainSettings getChainSettings(juce::AudioProcessorValueTreeState &apTreeState)
//{
//    ChainSettings settings;
//
//    // returns a unit within the defined range rather than normalized
//    settings.fLowCutFreq = apTreeState.getRawParameterValue("LowCutFreq") -> load();
//    settings.fHighCutFreq = apTreeState.getRawParameterValue("HighCutFreq") -> load();
//    settings.fPeakFreq = apTreeState.getRawParameterValue("PeakFreq") -> load();
//    settings.fPeakGainInDbs = apTreeState.getRawParameterValue("PeakGain") -> load(*);
//    settings.fPeakQuality = apTreeState.getRawParameterValue("PeakQ") -> load();
//    settings.iLowCutSlope = apTreeState.getRawParameterValue("LowCutSlope") -> load();
//    settings.iHighCutSlope = apTreeState.getRawParameterValue("HighCutSlope") -> load();
//
//    return settings;
//}
//==============================================================================
/**
*/
class EQ_v1AudioProcessor  : public ProcessorBase
{
public:
    //==============================================================================
    ChainSettings m_EqParams;
    
    EQ_v1AudioProcessor()
//    #ifndef JucePlugin_PreferredChannelConfigurations
//         : AudioProcessor (BusesProperties()
//                         #if ! JucePlugin_IsMidiEffect
//                          #if ! JucePlugin_IsSynth
//                           .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
//                          #endif
//                           .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
//                         #endif
//                           )
//    #endif
    {
        ChainSettings m_EqParams;
        
        // set lowest range to 20hz and highest to 20000hz and default to 20hz for low cut frequency
        m_EqParams.fLowCutFreq = 20.0f;
        m_EqParams.fHighCutFreq = 20000.0f;
        m_EqParams.fPeakFreq = 750.0f;
        m_EqParams.fPeakGainInDbs = 0.0f;
        m_EqParams.fPeakQuality = 1.0f;
        m_EqParams.iLowCutSlope = Slope::Slope_12;
        m_EqParams.iHighCutSlope = Slope::Slope_12;

    }
    
    ~EQ_v1AudioProcessor()
    {
    }
    
    void updateEqParams(float lowCutFreq, float highCutFreq, float peakFreq, float peakGainInDbs, float peakQuality, int lowCutSlope, int highCutSlope)
    {
        m_EqParams.fLowCutFreq = lowCutFreq;
        m_EqParams.fHighCutFreq = highCutFreq;
        m_EqParams.fPeakFreq = peakFreq;
        m_EqParams.fPeakGainInDbs = peakGainInDbs;
        m_EqParams.fPeakQuality = peakQuality;
        m_EqParams.iLowCutSlope = lowCutSlope;
        m_EqParams.iHighCutSlope = highCutSlope;
    }
    //==============================================================================

#ifndef JucePlugin_PreferredChannelConfigurations
bool isBusesLayoutSupported (const BusesLayout& layouts) const override
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif
//
    bool isMidiEffect() const override
    {
       #if JucePlugin_IsMidiEffect
        return true;
       #else
        return false;
       #endif
    }
//  ==============================================================================
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        juce::ScopedNoDenormals noDenormals;
        auto totalNumInputChannels  = getTotalNumInputChannels();
        auto totalNumOutputChannels = getTotalNumOutputChannels();

        // In case we have more outputs than inputs, this code clears any output
        // channels that didn't contain input data, (because these aren't
        // guaranteed to be empty - they may contain garbage).
        // This is here to avoid people getting screaming feedback
        // when they first compile a plugin, but obviously you don't need to keep
        // this code if your algorithm always overwrites all the output channels.
        for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            buffer.clear (i, 0, buffer.getNumSamples());

//        auto chainSettings = getChainSettings(apTreeState);

        
        updatePeakFilter(m_EqParams);
        
        
        auto cutCoeffs = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(m_EqParams.fLowCutFreq, getSampleRate(), 2 * (m_EqParams.iLowCutSlope) + 1);

        auto &leftLowCut = LeftChain.get<ChainPositions::LowCut>();

        updateCutFilter(leftLowCut, cutCoeffs, m_EqParams.lowCutSlope);
        
        auto &rightLowCut = RightChain.get<ChainPositions::LowCut>();
        
        updateCutFilter(rightLowCut, cutCoeffs, m_EqParams.lowCutSlope);
        
        juce::dsp::AudioBlock<float> block(buffer);
        
        // separate input buffer into 2 channels
        auto leftBlock = block.getSingleChannelBlock(0);
        auto rightBlock = block.getSingleChannelBlock(1);
        
        // set up left and right context to pass to left and right chain
        juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
        juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);
        
        LeftChain.process(leftContext);
        RightChain.process(rightContext);
        
        auto highCutCoeffs = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(m_EqParams.fHighCutFreq, getSampleRate(), 2 * (m_EqParams.iHighCutSlope) + 1);
        
        auto &leftHighCut = LeftChain.get<ChainPositions::HighCut>();
        
        updateCutFilter(leftHighCut, highCutCoeffs, m_EqParams.highCutSlope);
        
        auto &rightHighCut = RightChain.get<ChainPositions::HighCut>();
        
        updateCutFilter(rightHighCut, highCutCoeffs, m_EqParams.highCutSlope);
        
    }
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        juce::dsp::ProcessSpec processSpec;
        
        processSpec.maximumBlockSize = samplesPerBlock;
        
        // mono chain only has 1 channel
        processSpec.numChannels = 1;
        
        processSpec.sampleRate = sampleRate;
        
        LeftChain.prepare(processSpec);
        RightChain.prepare(processSpec);
        
//        auto chainSettings = getChainSettings(apTreeState);
        
        updatePeakFilter(m_EqParams);
        
        auto lowCutCoeffs = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(m_EqParams.fLowCutFreq, sampleRate, 2 * (m_EqParams.iLowCutSlope) + 1);

        auto &leftLowCut = LeftChain.get<ChainPositions::LowCut>();
        
        updateCutFilter(leftLowCut, lowCutCoeffs, m_EqParams.lowCutSlope);
        
        auto &rightLowCut = RightChain.get<ChainPositions::LowCut>();
        
        updateCutFilter(rightLowCut, lowCutCoeffs, m_EqParams.lowCutSlope);
        
        auto highCutCoeffs = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(m_EqParams.fHighCutFreq, sampleRate, 2 * (m_EqParams.iHighCutSlope) + 1);
        
        auto &leftHighCut = LeftChain.get<ChainPositions::HighCut>();
        
        updateCutFilter(leftHighCut, highCutCoeffs, m_EqParams.highCutSlope);
        
        auto &rightHighCut = RightChain.get<ChainPositions::HighCut>();
        
        updateCutFilter(rightHighCut, highCutCoeffs, m_EqParams.highCutSlope);
        
    }
    
    

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
    
    void updatePeakFilter(const ChainSettings &chainSettings)
    {
        // get coeffs for the peak filter using chain settings
        auto peakCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), chainSettings.fPeakFreq, chainSettings.fPeakQuality, juce::Decibels::decibelsToGain(chainSettings.fPeakGainInDbs));
        
        updateCoefficients(LeftChain.get<ChainPositions::Peak>().coefficients, peakCoeffs);
        updateCoefficients(RightChain.get<ChainPositions::Peak>().coefficients, peakCoeffs);
        
    }
    
    // alias for coefficients pointer in Juce dsp
    using Coeffs = Filter::CoefficientsPtr;

    void updateCoefficients(Coeffs &oldCoeffs, const Coeffs &newCoeffs)
    {
        *oldCoeffs = *newCoeffs;
    }
    
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



#endif /* EqProcessor_h */
