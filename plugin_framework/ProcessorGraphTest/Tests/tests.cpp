//
// Created by Vedant Kalbag on 02/04/22.
//
//#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "../JuceLibraryCode/JuceHeader.h"
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
ProcessorGraphTestAudioProcessor testPlugin;

TEST_CASE("Plugin instance name check", "[name]")
{
    CHECK_THAT(testPlugin.getName().toStdString(),
               Catch::Matchers::Equals("ProcessorGraphTest"));
}

TEST_CASE("Set and get param values directly from the apvts", "[params-apvts]")
{
    REQUIRE(testPlugin.apvts.getRawParameterValue("CompressorBypass_0")->load() == false);
    testPlugin.apvts.state.setProperty("CompressorBypass_0",true,nullptr);
//    REQUIRE(static_cast<bool>(testPlugin.apvts.getRawParameterValue("CompressorBypass_0")->load()) == true);
    REQUIRE(static_cast<bool>(testPlugin.apvts.state.getProperty("CompressorBypass_0")) == true);
}

TEST_CASE("Set and get param values from a node", "[params-node]")
{

}

TEST_CASE("Creating instances of a node", "[node-instances]")
{

}



