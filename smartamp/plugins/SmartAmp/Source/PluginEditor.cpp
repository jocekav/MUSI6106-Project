/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WaveNetVaAudioProcessorEditor::WaveNetVaAudioProcessorEditor (WaveNetVaAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    addAndMakeVisible(loadButton);
    loadButton.setButtonText("Load Tone111");
    loadButton.addListener(this);

    // Size of plugin GUI
    setSize (600, 200);


}

WaveNetVaAudioProcessorEditor::~WaveNetVaAudioProcessorEditor()
{
}

//==============================================================================
void WaveNetVaAudioProcessorEditor::paint (Graphics& g)
{
}

void WaveNetVaAudioProcessorEditor::resized()
{
    loadButton.setBounds(50, 40, 125, 25);
}

void WaveNetVaAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &loadButton) {
        loadButtonClicked();
    }
}

void WaveNetVaAudioProcessorEditor::loadButtonClicked()
{
//    juce::File path ("/Users/shanjiang/Documents/GitHub/SmartGuitarAmp/models/bluej_fullD_p0153.json");
//
//    auto fileText = path.loadFileAsString();
//    var configJSON = JSON::parse(fileText);
//    DBG(configJSON["residual_channels"].toString().toStdString());
    
    FileChooser chooser("Select a .json tone...",
        {},
        "*.json");
    if (chooser.browseForFileToOpen())
    {
        File file = chooser.getResult();
        processor.loadConfig(file);
        fname = file.getFileName();
        DBG("something happened");
        DBG(file.getFullPathName());
//        processor.loaded_tone = file;
//        processor.loaded_tone_name = fname;
    }
}




