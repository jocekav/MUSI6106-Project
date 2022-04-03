/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
//EQ_v1AudioProcessor::EQ_v1AudioProcessor()
//#ifndef JucePlugin_PreferredChannelConfigurations
//     : AudioProcessor (BusesProperties()
//                     #if ! JucePlugin_IsMidiEffect
//                      #if ! JucePlugin_IsSynth
//                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
//                      #endif
//                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
//                     #endif
//                       )
//#endif
//{
//}
//
//EQ_v1AudioProcessor::~EQ_v1AudioProcessor()
//{
//}
//
////==============================================================================
//const juce::String EQ_v1AudioProcessor::getName() const
//{
//    return JucePlugin_Name;
//}
//
//bool EQ_v1AudioProcessor::acceptsMidi() const
//{
//   #if JucePlugin_WantsMidiInput
//    return true;
//   #else
//    return false;
//   #endif
//}
//
//bool EQ_v1AudioProcessor::producesMidi() const
//{
//   #if JucePlugin_ProducesMidiOutput
//    return true;
//   #else
//    return false;
//   #endif
//}
//
//bool EQ_v1AudioProcessor::isMidiEffect() const
//{
//   #if JucePlugin_IsMidiEffect
//    return true;
//   #else
//    return false;
//   #endif
//}
//
//double EQ_v1AudioProcessor::getTailLengthSeconds() const
//{
//    return 0.0;
//}
//
//int EQ_v1AudioProcessor::getNumPrograms()
//{
//    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
//                // so this should be at least 1, even if you're not really implementing programs.
//}
//
//int EQ_v1AudioProcessor::getCurrentProgram()
//{
//    return 0;
//}
//
//void EQ_v1AudioProcessor::setCurrentProgram (int index)
//{
//}
//
//const juce::String EQ_v1AudioProcessor::getProgramName (int index)
//{
//    return {};
//}
//
//void EQ_v1AudioProcessor::changeProgramName (int index, const juce::String& newName)
//{
//}

////==============================================================================
//void EQ_v1AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
//{
//    juce::dsp::ProcessSpec processSpec;
//
//    processSpec.maximumBlockSize = samplesPerBlock;
//
//    // mono chain only has 1 channel
//    processSpec.numChannels = 1;
//
//    processSpec.sampleRate = sampleRate;
//
//    LeftChain.prepare(processSpec);
//    RightChain.prepare(processSpec);
//
//    auto chainSettings = getChainSettings(apTreeState);
//
//    updatePeakFilter(chainSettings);
//
//    auto lowCutCoeffs = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.fLowCutFreq, sampleRate, 2 * (chainSettings.iLowCutSlope) + 1);
//
//    auto &leftLowCut = LeftChain.get<ChainPositions::LowCut>();
//
//    updateCutFilter(leftLowCut, lowCutCoeffs, chainSettings.lowCutSlope);
//
//    auto &rightLowCut = RightChain.get<ChainPositions::LowCut>();
//
//    updateCutFilter(rightLowCut, lowCutCoeffs, chainSettings.lowCutSlope);
//
//    auto highCutCoeffs = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.fHighCutFreq, sampleRate, 2 * (chainSettings.iHighCutSlope) + 1);
//
//    auto &leftHighCut = LeftChain.get<ChainPositions::HighCut>();
//
//    updateCutFilter(leftHighCut, highCutCoeffs, chainSettings.highCutSlope);
//
//    auto &rightHighCut = RightChain.get<ChainPositions::HighCut>();
//
//    updateCutFilter(rightHighCut, highCutCoeffs, chainSettings.highCutSlope);
//
//}

//void EQ_v1AudioProcessor::releaseResources()
//{
//    // When playback stops, you can use this as an opportunity to free up any
//    // spare memory, etc.
//}

//#ifndef JucePlugin_PreferredChannelConfigurations
//bool EQ_v1AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
//{
//  #if JucePlugin_IsMidiEffect
//    juce::ignoreUnused (layouts);
//    return true;
//  #else
//    // This is the place where you check if the layout is supported.
//    // In this template code we only support mono or stereo.
//    // Some plugin hosts, such as certain GarageBand versions, will only
//    // load plugins that support stereo bus layouts.
//    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
//     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
//        return false;
//
//    // This checks if the input layout matches the output layout
//   #if ! JucePlugin_IsSynth
//    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
//        return false;
//   #endif
//
//    return true;
//  #endif
//}
//#endif

//==============================================================================
//bool EQ_v1AudioProcessor::hasEditor() const
//{
//    return true; // (change this to false if you choose to not supply an editor)
//}
//
//juce::AudioProcessorEditor* EQ_v1AudioProcessor::createEditor()
//{
////    return new EQ_v1AudioProcessorEditor (*this);
//    return new juce::GenericAudioProcessorEditor(*this);
//}

//==============================================================================
//void EQ_v1AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
//{
//    // You should use this method to store your parameters in the memory block.
//    // You could do that either as raw data, or use the XML or ValueTree classes
//    // as intermediaries to make it easy to save and load complex data.
//}
//
//void EQ_v1AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
//{
//    // You should use this method to restore your parameters from this memory block,
//    // whose contents will have been created by the getStateInformation() call.
//}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState &apTreeState)
{
    ChainSettings settings;
    
    // returns a unit within our defined range rather than normalized
    settings.fLowCutFreq = apTreeState.getRawParameterValue("LowCutFreq") -> load();
    settings.fHighCutFreq = apTreeState.getRawParameterValue("HighCutFreq") -> load();
    settings.fPeakFreq = apTreeState.getRawParameterValue("PeakFreq") -> load();
    settings.fPeakGainInDbs = apTreeState.getRawParameterValue("PeakGain") -> load();
    settings.fPeakQuality = apTreeState.getRawParameterValue("PeakQ") -> load();
    settings.iLowCutSlope = apTreeState.getRawParameterValue("LowCutSlope") -> load();
    settings.iHighCutSlope = apTreeState.getRawParameterValue("HighCutSlope") -> load();
    
    return settings;
}

//void EQ_v1AudioProcessor::updatePeakFilter(const ChainSettings &chainSettings)
//{
//    // get coeffs for the peak filter using chain settings
//    auto peakCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), chainSettings.fPeakFreq, chainSettings.fPeakQuality, juce::Decibels::decibelsToGain(chainSettings.fPeakGainInDbs));
//
//    updateCoefficients(LeftChain.get<ChainPositions::Peak>().coefficients, peakCoeffs);
//    updateCoefficients(RightChain.get<ChainPositions::Peak>().coefficients, peakCoeffs);
//
//}

//void EQ_v1AudioProcessor::updateCoefficients(Coeffs &oldCoeffs, const Coeffs &newCoeffs)
//{
//    *oldCoeffs = *newCoeffs;
//}

//juce::AudioProcessorValueTreeState::ParameterLayout EQ_v1AudioProcessor::createParameterLayout()
//{
//    juce::AudioProcessorValueTreeState::ParameterLayout layout;
//
//// set lowest range to 20hz and highest to 20000hz and default to 20hz for low cut frequency
//    layout.add(std::make_unique<juce::AudioParameterFloat>("LowCutFreq", "LowCutFreq", juce::NormalisableRange<float>(20, 20000, 1, 0.25), 20));
//
//// set lowest range to 20hz and highest to 20000hz and default to 20000hz for high cut frequency
//    layout.add(std::make_unique<juce::AudioParameterFloat>("HighCutFreq", "HighCutFreq", juce::NormalisableRange<float>(20, 20000, 1, 0.25), 20000));
//
//// set lowest range to 20hz and highest to 20000hz and default to 20hz for peak frequency, adjust skew to sound better to user
//    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakFreq", "PeakFreq", juce::NormalisableRange<float>(20, 20000, 1, 0.25), 750));
//
//// set lowest range to -24 and highest to 24 and default to 0 for peak gain with 0.5 step size
//    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakGain", "PeakGain", juce::NormalisableRange<float>(-24, 24, 0.5, 1), 0));
//
//// set lowest range to 0.1 and highest to 10 and default to 2 for peak Q value with 0.05 step size
//    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakQ", "PeakQ", juce::NormalisableRange<float>(0.1, 10, 0.05, 1), 1));
//
//// filter choice uses string array and filter slopes will be multiples of 12 per db/octave
//    juce::StringArray stringArray;
//    for (int i = 0; i < 4; i++)
//    {
//        juce::String buildStr;
//        buildStr << (12 + (i * 12));
//        buildStr << " db/Oct";
//        stringArray.add(buildStr);
//    }
//
//// make lowcut slope params
//    layout.add(std::make_unique<juce::AudioParameterChoice>("LowCutSlope", "LowCutSlope", stringArray, 0));
//
//// make highcut slope params
//    layout.add(std::make_unique<juce::AudioParameterChoice>("HighCutSlope", "HighCutSlope", stringArray, 0));
//
//
//
//    return layout;
//}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
//    return new EQ_v1AudioProcessor();
    return new GraphProcessor();
}
