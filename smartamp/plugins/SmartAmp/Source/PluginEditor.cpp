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
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to

    // Set Widget Graphics
    ampSilverKnobLAF.setLookAndFeel(ImageCache::getFromMemory(BinaryData::knob_silver_png, BinaryData::knob_silver_pngSize));

    ampOnButton.setImages(true, true, true,
        ImageCache::getFromMemory(BinaryData::power_switch_up_png, BinaryData::power_switch_up_pngSize), 1.0, Colours::transparentWhite,
        Image(), 1.0, Colours::transparentWhite,
        ImageCache::getFromMemory(BinaryData::power_switch_up_png, BinaryData::power_switch_up_pngSize), 1.0, Colours::transparentWhite,
        0.0);
    addAndMakeVisible(ampOnButton);
    ampOnButton.addListener(this);

    addAndMakeVisible(loadButton);
    loadButton.setButtonText("Load Tone");
    loadButton.addListener(this);

    addAndMakeVisible(modelLabel);
    modelLabel.setText(processor.loaded_tone_name, juce::NotificationType::dontSendNotification);
    modelLabel.setJustificationType(juce::Justification::left);
    modelLabel.setColour(juce::Label::textColourId, juce::Colours::black);

    ampLED.setImages(true, true, true,
        ImageCache::getFromMemory(BinaryData::led_blue_on_png, BinaryData::led_blue_on_pngSize), 1.0, Colours::transparentWhite,
        Image(), 1.0, Colours::transparentWhite,
        ImageCache::getFromMemory(BinaryData::led_blue_on_png, BinaryData::led_blue_on_pngSize), 1.0, Colours::transparentWhite,
        0.0);
    addAndMakeVisible(ampLED);

    ampCleanGainKnob.addListener(this);
    ampCleanGainKnob.setValue(processor.ampCleanGainKnobState);

    addAndMakeVisible(ampMasterKnob);
    ampMasterKnob.setLookAndFeel(&ampSilverKnobLAF);
    ampMasterKnob.addListener(this);
    ampMasterKnob.setRange(-24.0, 0.0);
    ampMasterKnob.setValue(processor.ampMasterKnobState);
    ampMasterKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampMasterKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20 );
    ampMasterKnob.setNumDecimalPlacesToDisplay(1);
    ampMasterKnob.setDoubleClickReturnValue(true, -12.0);

    // Size of plugin GUI
    setSize (600, 200);

    // Load the preset wavenet json model from the project resources
    if (processor.custom_tone == 0) {
        processor.loadConfigAmp();
    } else {
        processor.loadConfig(processor.loaded_tone);
    }
}

WaveNetVaAudioProcessorEditor::~WaveNetVaAudioProcessorEditor()
{
}

//==============================================================================
void WaveNetVaAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    // Set On/Off amp graphic
    if (processor.amp_state == 0) {
        ampOnButton.setImages(true, true, true,
            ImageCache::getFromMemory(BinaryData::power_switch_down_png, BinaryData::power_switch_down_pngSize), 1.0, Colours::transparentWhite,
            Image(), 1.0, Colours::transparentWhite,
            ImageCache::getFromMemory(BinaryData::power_switch_down_png, BinaryData::power_switch_down_pngSize), 1.0, Colours::transparentWhite,
            0.0);
        ampLED.setImages(true, true, true,
            ImageCache::getFromMemory(BinaryData::led_blue_off_png, BinaryData::led_blue_off_pngSize), 1.0, Colours::transparentWhite,
            Image(), 1.0, Colours::transparentWhite,
            ImageCache::getFromMemory(BinaryData::led_blue_off_png, BinaryData::led_blue_off_pngSize), 1.0, Colours::transparentWhite,
            0.0);
    }
    else {
        ampOnButton.setImages(true, true, true,
            ImageCache::getFromMemory(BinaryData::power_switch_up_png, BinaryData::power_switch_up_pngSize), 1.0, Colours::transparentWhite,
            Image(), 1.0, Colours::transparentWhite,
            ImageCache::getFromMemory(BinaryData::power_switch_up_png, BinaryData::power_switch_up_pngSize), 1.0, Colours::transparentWhite,
            0.0);
        ampLED.setImages(true, true, true,
            ImageCache::getFromMemory(BinaryData::led_blue_on_png, BinaryData::led_blue_on_pngSize), 1.0, Colours::transparentWhite,
            Image(), 1.0, Colours::transparentWhite,
            ImageCache::getFromMemory(BinaryData::led_blue_on_png, BinaryData::led_blue_on_pngSize), 1.0, Colours::transparentWhite,
            0.0);
    }
}

void WaveNetVaAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    loadButton.setBounds(50, 40, 125, 25);
    modelLabel.setBounds(50, 65, 400, 25);
    ampMasterKnob.setBounds(300, 40, 75, 105);
    ampOnButton.setBounds(200, 40, 35, 45);
    ampLED.setBounds(400, 40, 15, 25);
}

void WaveNetVaAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &ampOnButton) {
        ampOnButtonClicked();
    } else if (button == &loadButton) {
        loadButtonClicked();
    }
}

void WaveNetVaAudioProcessorEditor::loadButtonClicked()
{
    FileChooser chooser("Select a .json tone...",
        {},
        "*.json");
    if (chooser.browseForFileToOpen())
    {
        File file = chooser.getResult();
        processor.loadConfig(file);
        fname = file.getFileName();
        modelLabel.setText(fname, juce::NotificationType::dontSendNotification);
        processor.loaded_tone = file;
        processor.loaded_tone_name = fname;
        processor.custom_tone = 1;
    }
}

void WaveNetVaAudioProcessorEditor::ampOnButtonClicked() {
    if (processor.amp_state == 0) {
        processor.amp_state = 1;
    }
    else {
        processor.amp_state = 0;
    }
    repaint();
}

void WaveNetVaAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    // Amp
    if (slider == &ampCleanGainKnob)
        processor.set_ampCleanDrive(slider->getValue());
    else if (slider == &ampMasterKnob)
        processor.set_ampMaster(slider->getValue());
}
