//
// Created by Vedant Kalbag on 02/04/22.
//
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "../Source/PluginProcessor.h"
//unsigned int Factorial( unsigned int number ) {
//    return number <= 1 ? number : Factorial(number-1)*number;
//}
//
//TEST_CASE( "Factorials are computed", "[factorial]" ) {
//    REQUIRE( Factorial(1) == 1 );
//    REQUIRE( Factorial(2) == 2 );
//    REQUIRE( Factorial(3) == 6 );
//    REQUIRE( Factorial(10) == 3628800 );
//}

TEST_CASE("processor: Name", "[processor]")
{
    std::cout << "Testing processor name" << std::endl;
    //    auto processor     = ProcessorGraphTestAudioProcessor();
    ProcessorGraphTestAudioProcessor processor;
    REQUIRE(processor.getName() == juce::String {"ProcessorGraphTest"});
    processor.releaseResources();
}


TEST_CASE("processor: BusesLayoutSupportMono", "[processor]")
{
    std::cout << "Testing processor mono support" << std::endl;
//    auto processor     = ProcessorGraphTestAudioProcessor();
    ProcessorGraphTestAudioProcessor processor;
    auto layout        = juce::AudioProcessor::BusesLayout {};
    layout.inputBuses  = juce::AudioChannelSet::mono();
    layout.outputBuses = juce::AudioChannelSet::mono();

    REQUIRE(processor.isBusesLayoutSupported(layout));
    processor.releaseResources();
}

TEST_CASE("processor: BusesLayoutSupportStereo", "[processor]")
{
    std::cout << "Testing processor stereo support" << std::endl;
    //    auto processor     = ProcessorGraphTestAudioProcessor();
    ProcessorGraphTestAudioProcessor processor;
    auto layout = juce::AudioProcessor::BusesLayout{};
    layout.inputBuses = juce::AudioChannelSet::stereo();
    layout.outputBuses = juce::AudioChannelSet::stereo();

    REQUIRE(processor.isBusesLayoutSupported(layout));
    processor.releaseResources();
}

TEST_CASE("processor: latencyAddition", "[processor]")
{
    std::cout << "Testing processor latency increase" << std::endl;
    ProcessorGraphTestAudioProcessor processor;
    processor.setLatencySamples(9);
   
    REQUIRE(processor.getLatencySamples() == 9);
    processor.releaseResources();
}


TEST_CASE("processor: apvts value set/get", "[params-apvts]")
{
    std::cout << "Testing apvts param value set/get" << std::endl;
    //    auto processor     = ProcessorGraphTestAudioProcessor();
    ProcessorGraphTestAudioProcessor processor;
    REQUIRE(processor.apvts.getRawParameterValue("CompressorBypass_0")->load() == false);
    processor.apvts.state.setProperty("CompressorBypass_0",true,nullptr);
//    REQUIRE(static_cast<bool>(testPlugin.apvts.getRawParameterValue("CompressorBypass_0")->load()) == true);
    REQUIRE(static_cast<bool>(processor.apvts.state.getProperty("CompressorBypass_0")) == true);
}


TEST_CASE("processor: create nodes and test naming","[create-nodes]")
{
    //    auto processor     = ProcessorGraphTestAudioProcessor();
    ProcessorGraphTestAudioProcessor processor;
    processor.prepareToPlay(44100, 2048);
    REQUIRE(processor.inputGainNode->getProcessor()->getName() == "Gain_0");
    REQUIRE(processor.outputGainNode->getProcessor()->getName() == "Gain_1");

    for (int i=0; i<processor.apvts.state.getNumProperties();i++)
    {
        std::cout << processor.apvts.state.getPropertyName(i).toString() << std::endl;
    }
}

TEST_CASE("processor: does not accept midi", "[midi]")
{
    ProcessorGraphTestAudioProcessor processor;
    REQUIRE(processor.acceptsMidi() == false);
}



// check that it never goes above 1


//next tests: put input through and check output for various configurations!
TEST_CASE("bypass all effects", "[processor]")
{
    constexpr auto numChannels = 2;
    constexpr auto numSamples = 64;

    auto midi = juce::MidiBuffer{};
    auto buffer = juce::AudioBuffer<float>{ numChannels, numSamples };

    // fill buffer with all 1.0
    for (auto i = 0; i < numChannels; i++)
    {
        for (auto j = 0; j < numSamples; j++) { buffer.setSample(i, j, 1.0f); }
    }

    ProcessorGraphTestAudioProcessor processor;
    
    std::cout << "about to getrawparametervalue" << std::endl;
    //processor.apvts.getRawParameterValue("gain")->store(0.0f);
    processor.apvts.state.setProperty("GainValue_0", 5.0f, nullptr);
    auto phaserBypP = processor.apvts.state.getPropertyPointer("PhaserByprass_0");
    
  //  processor.apvts.state.setProperty(phaserBypP, true, nullptr);
    processor.apvts.state.setProperty("NoiseGateBypass_0", true, nullptr);
    processor.apvts.state.setProperty("CompressorBypass_0", true, nullptr);
    processor.apvts.state.setProperty("ReverbBypass_0", 1.0f, nullptr);
    juce::XmlElement::TextFormat text;
    std::cout << processor.apvts.state.toXmlString(text) << std::endl;

    //AudioProcessorParameterWithID* pParam = parameters.getParameter("GainValue_0"); 
    //pParam->beginChangeGesture(); 
    //pParam->setValueNotifyingHost(paramValue); 
   // pParam->endChangeGesture();

        
    // processor.apvts.state.setProperty("PhaserBypass", 0.0f, nullptr);



    //processor.apvts.state.setProperty("CompressorBypass_0", true, nullptr);
    // bypass all effects:
    //processor.apvts.state.setProperty("CompressorBypass_0", true, nullptr);

    std::cout << "about to preparetoplay" << std::endl;
    processor.prepareToPlay(44100.0, numSamples);
    processor.processBlock(buffer, midi);

    // buffer should be silent
    for (auto i = 0; i < numChannels; i++)
    {
        for (auto j = 0; j < numSamples; j++)
        {
            auto const sample = buffer.getSample(i, j);
            REQUIRE(sample == 1.0f);
        }
    }

    processor.releaseResources();
}