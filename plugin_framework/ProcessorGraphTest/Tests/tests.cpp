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

void setBuffer(int numChannels, int numSamples, juce::AudioBuffer<float>&buffer, float sampleToSet) {
    for (auto i = 0; i < numChannels; i++)
    {
        for (auto j = 0; j < numSamples; j++) { buffer.setSample(i, j, sampleToSet); }
    }
}
void checkBuffer(int numChannels, int numSamples, juce::AudioBuffer<float> &buffer, float outputToCheck) {
    for (auto i = 0; i < numChannels; i++)
    {
        for (auto j = 0; j < numSamples; j++)
        {
            auto const sample = buffer.getSample(i, j);
            REQUIRE(sample == outputToCheck);
        }
    }
}
float checkBufferRatio(int numChannels, int numSamples, juce::AudioBuffer<float>& buffer, float inputToCheck) {
    float aveSample = 0;
    for (auto i = 0; i < numChannels; i++)
    {
        for (auto j = 0; j < numSamples; j++)
        {
            auto const sample = buffer.getSample(i, j);
            // require the ratio to be less than 1
            REQUIRE(float(sample)/float(inputToCheck) <= 1);

            aveSample += sample;
        }
    }
    return juce::Decibels::gainToDecibels(float(aveSample) / ((numChannels * numSamples) * (float(inputToCheck))));
}


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

   /* juce::Value bypassReverb = processor.apvts.getParameterAsValue("ReverbBypass_0");
    bypassReverb = true;*/

    // CompressorInputGain_0 should be set to 0.f in defaults, test
    juce::Value compressorInputGainTest = processor.apvts.getParameterAsValue("CompressorInputGain_0");

    juce::Value testVal;
    testVal.setValue(0.f);


    REQUIRE(compressorInputGainTest.operator==(testVal));

    // check what happens when we set input gain to 0.88f
    float intermediateValue = 0.88f;
    compressorInputGainTest = intermediateValue;
    juce::Value compressorInputGain2 = processor.apvts.getParameterAsValue("CompressorInputGain_0");
    juce::Value theTestVal;
    theTestVal.setValue(intermediateValue);
    REQUIRE(compressorInputGain2.operator==(theTestVal));
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

    ProcessorGraphTestAudioProcessor processor;


    std::cout << "about to getparameterasvalue" << std::endl;
    
    // audioparmeterwithid poimter and setting it to the addres return by createandaddparaeeer
    //setValueNotifyingHost
    //getparemerterasvalue
    juce::Value bypassPhaser = processor.apvts.getParameterAsValue("PhaserBypass_0");
    bypassPhaser = true;
    juce::Value bypassCompressor = processor.apvts.getParameterAsValue("CompressorBypass_0");
    bypassCompressor = true;
    juce::Value bypassEqualizer = processor.apvts.getParameterAsValue("EqualizerBypass_0");
    bypassEqualizer = true;
    juce::Value bypassNoiseGate = processor.apvts.getParameterAsValue("NoiseGateBypass_0");
    bypassNoiseGate = true;
    juce::Value bypassReverb = processor.apvts.getParameterAsValue("ReverbBypass_0");
    bypassReverb = true;

    juce::Value gain0 = processor.apvts.getParameterAsValue("GainValue_0");
    gain0 = juce::Decibels::gainToDecibels(1);
    juce::Value gain1 = processor.apvts.getParameterAsValue("GainValue_1");
    gain1 = juce::Decibels::gainToDecibels(1);
    std::cout << juce::Decibels::decibelsToGain(processor.apvts.getRawParameterValue("GainValue_0")->load()) << std::endl;
    std::cout << processor.apvts.getRawParameterValue("GainValue_0")->load() << std::endl;
    




    juce::XmlElement::TextFormat text;
    std::cout << processor.apvts.state.toXmlString(text) << std::endl;


    std::cout << "about to preparetoplay" << std::endl;
    processor.prepareToPlay(44100.0, numSamples);



    std::cout << "about to preparetoplay" << std::endl;
    processor.prepareToPlay(44100.0, numSamples);

    setBuffer(numChannels, numSamples, buffer, 0.0f);
    processor.processBlock(buffer, midi);

    checkBuffer(numChannels, numSamples, buffer, 0.0f);
    processor.releaseResources();

    setBuffer(numChannels, numSamples, buffer, 0.5f);
    processor.processBlock(buffer, midi);

    checkBuffer(numChannels, numSamples, buffer, 0.5f);
    processor.releaseResources();

}


// REMAINING TESTS:
//check only one effect at a time
//get the ratio of output to input, then convert to db.it should be 0dB difference.
TEST_CASE("bypass each effect", "[processor]")
{
    constexpr auto numChannels = 2;
    constexpr auto numSamples = 64;
    auto midi = juce::MidiBuffer{};
    auto buffer = juce::AudioBuffer<float>{ numChannels, numSamples };
    ProcessorGraphTestAudioProcessor processor;

    juce::Value bypassPhaser = processor.apvts.getParameterAsValue("PhaserBypass_0");
    juce::Value bypassCompressor = processor.apvts.getParameterAsValue("CompressorBypass_0");
    juce::Value bypassEqualizer = processor.apvts.getParameterAsValue("EqualizerBypass_0");
    juce::Value bypassNoiseGate = processor.apvts.getParameterAsValue("NoiseGateBypass_0");
    juce::Value bypassReverb = processor.apvts.getParameterAsValue("ReverbBypass_0");
    juce::Value gain0 = processor.apvts.getParameterAsValue("GainValue_0");
    juce::Value gain1 = processor.apvts.getParameterAsValue("GainValue_1");
    gain0 = juce::Decibels::gainToDecibels(1);
    gain1 = juce::Decibels::gainToDecibels(1);
    bypassPhaser = true;
    bypassCompressor = true;
    bypassEqualizer = true;
    bypassNoiseGate = true;
    bypassReverb = true;

    ////////////////////
    // PHASOR ON
    ////////////////////
    bypassPhaser = false;
    std::cout << "about to preparetoplay" << std::endl;
    processor.prepareToPlay(44100.0, numSamples);

    setBuffer(numChannels, numSamples, buffer, 0.5f);
    processor.processBlock(buffer, midi);

    float ratio = checkBufferRatio(numChannels, numSamples, buffer, 0.5f);
    std::cout << "phasor: " << ratio << std::endl;
    processor.releaseResources();
    bypassPhaser = true;


    ////////////////////
    // COMPRESSOR ON
    ////////////////////
    bypassCompressor = false;

    setBuffer(numChannels, numSamples, buffer, 0.5f);
    processor.processBlock(buffer, midi);

    ratio = checkBufferRatio(numChannels, numSamples, buffer, 0.5f);
    std::cout << "Compressor: " << ratio << std::endl;
    processor.releaseResources();
    bypassCompressor = true;
    
    ////////////////////
    // EQ ON
    ////////////////////
    bypassEqualizer = false;
    setBuffer(numChannels, numSamples, buffer, 0.5f);
    processor.processBlock(buffer, midi);
    ratio = checkBufferRatio(numChannels, numSamples, buffer, 0.5f);
    std::cout << "Equalizer: " << ratio << std::endl;
    processor.releaseResources();
    bypassEqualizer = true;

    ////////////////////
    // NOISE GATE ON
    ////////////////////
    bypassNoiseGate = false;
    setBuffer(numChannels, numSamples, buffer, 0.5f);
    processor.processBlock(buffer, midi);
    ratio = checkBufferRatio(numChannels, numSamples, buffer, 0.5f);
    std::cout << "Gate: " << ratio << std::endl;
    processor.releaseResources();
    bypassNoiseGate = true;

    ////////////////////
    // REVERB ON
    ////////////////////
    bypassReverb = false;
    setBuffer(numChannels, numSamples, buffer, 0.5f);
    processor.processBlock(buffer, midi);
    ratio = checkBufferRatio(numChannels, numSamples, buffer, 0.5f);
    std::cout << "Reverb: " << ratio << std::endl;
    processor.releaseResources();
    bypassReverb = true;
}



//bypass one at a time
//
//make sure max and min values dont make the outputs exceed 1 - for all effects on at once
//







