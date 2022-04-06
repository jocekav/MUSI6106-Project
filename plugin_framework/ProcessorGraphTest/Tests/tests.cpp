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

