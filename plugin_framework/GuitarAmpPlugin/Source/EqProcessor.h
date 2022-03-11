//
//  EqProcessor.h
//  GuitarAmpPlugin
//
//  Created by Jocelyn Kavanagh on 3/9/22.
//  Copyright © 2022 LL. All rights reserved.
//
#include <JuceHeader.h>
#include <BaseProcessor.h>

#ifndef EqProcessor_h
#define EqProcessor_h

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
class EQ_v1AudioProcessor  : public ProcessorBase
{
public:
    //==============================================================================
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
    }
    
    ~EQ_v1AudioProcessor()
    {
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
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // set lowest range to 20hz and highest to 20000hz and default to 20hz for low cut frequency
        layout.add(std::make_unique<juce::AudioParameterFloat>("LowCutFreq", "LowCutFreq", juce::NormalisableRange<float>(20, 20000, 1, 0.25), 20));

    // set lowest range to 20hz and highest to 20000hz and default to 20000hz for high cut frequency
        layout.add(std::make_unique<juce::AudioParameterFloat>("HighCutFreq", "HighCutFreq", juce::NormalisableRange<float>(20, 20000, 1, 0.25), 20000));

    // set lowest range to 20hz and highest to 20000hz and default to 20hz for peak frequency, adjust skew to sound better to user
        layout.add(std::make_unique<juce::AudioParameterFloat>("PeakFreq", "PeakFreq", juce::NormalisableRange<float>(20, 20000, 1, 0.25), 750));

    // set lowest range to -24 and highest to 24 and default to 0 for peak gain with 0.5 step size
        layout.add(std::make_unique<juce::AudioParameterFloat>("PeakGain", "PeakGain", juce::NormalisableRange<float>(-24, 24, 0.5, 1), 0));

    // set lowest range to 0.1 and highest to 10 and default to 2 for peak Q value with 0.05 step size
        layout.add(std::make_unique<juce::AudioParameterFloat>("PeakQ", "PeakQ", juce::NormalisableRange<float>(0.1, 10, 0.05, 1), 1));

    // filter choice uses string array and filter slopes will be multiples of 12 per db/octave
        juce::StringArray stringArray;
        for (int i = 0; i < 4; i++)
        {
            juce::String buildStr;
            buildStr << (12 + (i * 12));
            buildStr << " db/Oct";
            stringArray.add(buildStr);
        }

    // make lowcut slope params
        layout.add(std::make_unique<juce::AudioParameterChoice>("LowCutSlope", "LowCutSlope", stringArray, 0));

    // make highcut slope params
        layout.add(std::make_unique<juce::AudioParameterChoice>("HighCutSlope", "HighCutSlope", stringArray, 0));



        return layout;
    }
    
    
    
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

        auto chainSettings = getChainSettings(apTreeState);
        
        updatePeakFilter(chainSettings);
        
        
        auto cutCoeffs = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.fLowCutFreq, getSampleRate(), 2 * (chainSettings.iLowCutSlope) + 1);

        auto &leftLowCut = LeftChain.get<ChainPositions::LowCut>();

        updateCutFilter(leftLowCut, cutCoeffs, chainSettings.lowCutSlope);
        
        auto &rightLowCut = RightChain.get<ChainPositions::LowCut>();
        
        updateCutFilter(rightLowCut, cutCoeffs, chainSettings.lowCutSlope);
        
        juce::dsp::AudioBlock<float> block(buffer);
        
        // separate input buffer into 2 channels
        auto leftBlock = block.getSingleChannelBlock(0);
        auto rightBlock = block.getSingleChannelBlock(1);
        
        // set up left and right context to pass to left and right chain
        juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
        juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);
        
        LeftChain.process(leftContext);
        RightChain.process(rightContext);
        
        auto highCutCoeffs = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.fHighCutFreq, getSampleRate(), 2 * (chainSettings.iHighCutSlope) + 1);
        
        auto &leftHighCut = LeftChain.get<ChainPositions::HighCut>();
        
        updateCutFilter(leftHighCut, highCutCoeffs, chainSettings.highCutSlope);
        
        auto &rightHighCut = RightChain.get<ChainPositions::HighCut>();
        
        updateCutFilter(rightHighCut, highCutCoeffs, chainSettings.highCutSlope);
        
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
        
        auto chainSettings = getChainSettings(apTreeState);
        
        updatePeakFilter(chainSettings);
        
        auto lowCutCoeffs = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.fLowCutFreq, sampleRate, 2 * (chainSettings.iLowCutSlope) + 1);

        auto &leftLowCut = LeftChain.get<ChainPositions::LowCut>();
        
        updateCutFilter(leftLowCut, lowCutCoeffs, chainSettings.lowCutSlope);
        
        auto &rightLowCut = RightChain.get<ChainPositions::LowCut>();
        
        updateCutFilter(rightLowCut, lowCutCoeffs, chainSettings.lowCutSlope);
        
        auto highCutCoeffs = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.fHighCutFreq, sampleRate, 2 * (chainSettings.iHighCutSlope) + 1);
        
        auto &leftHighCut = LeftChain.get<ChainPositions::HighCut>();
        
        updateCutFilter(leftHighCut, highCutCoeffs, chainSettings.highCutSlope);
        
        auto &rightHighCut = RightChain.get<ChainPositions::HighCut>();
        
        updateCutFilter(rightHighCut, highCutCoeffs, chainSettings.highCutSlope);
        
    }
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQ_v1AudioProcessor)
};



#endif /* EqProcessor_h */
