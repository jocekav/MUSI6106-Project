/*
  ==============================================================================

    Effects.cpp
    Created: 22 Feb 2022 11:04:09am
    Author:  Vedant Kalbag

  ==============================================================================
*/

#include "Effects.h"

void CAmplifierIf::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    //TODO: Write CAmplifierIf prepareToPlay
    // Initialize distortion algorithm from Amplifiers.h based on the param
}

void CAmplifierIf::reset()
{
    //TODO: Write CAmplifierIf reset
}

void CAmplifierIf::update()
{
    //TODO: Write update for this that calls update for the CDistortionBase child classes
}

void CAmplifierIf::processBlock(juce::AudioSampleBuffer &buffer, juce::MidiBuffer &)
{
    //TODO: Write CAmplifierIf processBlock
}