/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"
#include "TableListBoxTutorial.h"
//#include "PresetHandler.hpp"

//==============================================================================
/**
*/

struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider (juce::Graphics&,
                           int x, int y, int width, int height,
                           float sliderPosProportional,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           juce::Slider&) override;
    
    void drawToggleButton (juce::Graphics &g,
                           juce::ToggleButton &toggle,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override;
    
    void drawLinearSlider(juce::Graphics &,
                          int x, int y, int width, int height,
                          float     sliderPos,
                          float     minSliderPos,
                          float     maxSliderPos,
                          const juce::Slider::SliderStyle,
                          juce::Slider &) override;
    void drawComboBox(juce::Graphics&,
                      int     width,
                      int     height,
                      bool     isButtonDown,
                      int ,
                      int,
                      int,
                      int,
                      juce::ComboBox &) override;
};

struct CustomRotarySlider : juce::Slider
{
    CustomRotarySlider(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) : juce::Slider(SliderStyle::RotaryHorizontalVerticalDrag,
                                        juce::Slider::TextEntryBoxPosition::NoTextBox),
    param(&rap),
    suffix(unitSuffix)
    {
        setLookAndFeel(&lnf);
    }
    
    ~CustomRotarySlider()
    {
        setLookAndFeel(nullptr);
    }
    
    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    juce::String getDisplayString() const;
private:
    LookAndFeel lnf;
    juce::RangedAudioParameter* param;
    juce::String suffix;
};

struct CustomToggle : juce::ToggleButton
{
    CustomToggle(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) : juce::ToggleButton(),
    param(&rap),
    suffix(unitSuffix)
    {
        setLookAndFeel(&lnf);
    }
    
    ~CustomToggle()
    {
        setLookAndFeel(nullptr);
    }
    
    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    juce::String getDisplayString() const;
private:
    LookAndFeel lnf;
    
    juce::RangedAudioParameter* param;
    juce::String suffix;
};

struct CustomTextButton : juce::TextButton
{
    CustomTextButton(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) : juce::TextButton(),
    param(&rap),
    suffix(unitSuffix)
    {
        setLookAndFeel(&lnf);
    }
    
    ~CustomTextButton()
    {
        setLookAndFeel(nullptr);
    }
    
    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    juce::String getDisplayString() const;
private:
    LookAndFeel lnf;
    
    juce::RangedAudioParameter* param;
    juce::String suffix;
};

struct CustomComboBox : juce::ComboBox
{
    CustomComboBox(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) : juce::ComboBox(),
    param(&rap),
    suffix(unitSuffix)
    {
        setLookAndFeel(&lnf);
    }
    
    ~CustomComboBox()
    {
        setLookAndFeel(nullptr);
    }
    
    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    juce::String getDisplayString() const;
private:
    LookAndFeel lnf;
    
    juce::RangedAudioParameter* param;
    juce::String suffix;
};


class ProcessorGraphTestAudioProcessorEditor  : public juce::AudioProcessorEditor,
public juce::Button::Listener
{
public:
    ProcessorGraphTestAudioProcessorEditor (ProcessorGraphTestAudioProcessor&);
    ~ProcessorGraphTestAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked (juce::Button* button);
    void updateToggleState(juce::Button* button);
    void drawNoiseGate();
    void drawAmp();
    void drawReverb();
    void drawCompressor();
    void drawEQ();
    void drawPhaser();
    void drawDelay();
    
    void loadData();
    
    
    enum RadioButtonIDs
    {
        EffectShown = 1001
    };
    

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ProcessorGraphTestAudioProcessor& audioProcessor;
    
    PresetHandler presetHandler;
    
    // Effect chain buttons
        juce::TextButton presetPopUpButton;
//        CustomTextButton presetPopUpButton;
        TableTutorialComponent presetTable;
        bool showTable = false;
        
        juce::ToggleButton gateButton,
        ampButton,
        verbButton,
        compressorButton,
        eqButton,
        phaserButton,
        delayButton;
        
        using APVTS = juce::AudioProcessorValueTreeState;
        
        // Chain in and out gain
        CustomRotarySlider inputGainSlider,
        outputGainSlider;
        
        APVTS::SliderAttachment inputGainSliderAttachment,
        outputGainSliderAttachment;
        
        // Noise Gate Components
        CustomRotarySlider gateThresholdSlider,
        gateRatioSlider,
        gateAttackSlider,
        gateReleaseSlider;
        
        APVTS::SliderAttachment gateThresholdSliderAttachment,
        gateRatioSliderAttachment,
        gateAttackSliderAttachment,
        gateReleaseSliderAttachment;
        
        juce::Label gateThresholdSliderLabel,
        gateRatioSliderLabel,
        gateAttackSliderLabel,
        gateReleaseSliderLabel;
        
        CustomToggle gateBypassToggle;
        APVTS::ButtonAttachment gateBypassAttachment;
        
        // Amp Components
//        CustomRotarySlider ampDriveSlider,
//        ampBlendSlider,
//        ampPreLpfSlider,
//        ampPreHpfSlider,
//        ampEmphasisFreqSlider,
//        ampEmphasisGainSlider,
//        ampPostLpfSlider,
//        ampPostHpfSlider;
//
//        APVTS::SliderAttachment ampDriveSliderAttachment,
//        ampBlendSliderAttachment,
//        ampPreLpfSliderAttachment,
//        ampPreHpfSliderAttachment,
//        ampEmphasisFreqSliderAttachment,
//        ampEmphasisGainSliderAttachment,
//        ampPostLpfSliderAttachment,
//        ampPostHpfSliderAttachment;
//
//        juce::Label ampDriveSliderLabel,
//        ampBlendSliderLabel,
//        ampPreLpfSliderLabel,
//        ampPreHpfSliderLabel,
//        ampEmphasisFreqSliderLabel,
//        ampEmphasisGainSliderLabel,
//        ampPostLpfSliderLabel,
//        ampPostHpfSliderLabel;
//
//        juce::ComboBox ampTypeComboBox;
        juce::Label ampTypeComboBoxLabel;
        CustomComboBox ampTypeComboBox;
        APVTS::ComboBoxAttachment ampTypeComboBoxAttachment;

//        CustomToggle ampBypassToggle;
//    APVTS::ButtonAttachment ampBypassAttachment;
        
        // Reverb Components
        CustomRotarySlider rvbBlendSlider,
        rvbRoomSizeSlider,
        rvbDampingSlider;

        APVTS::SliderAttachment rvbBlendSliderAttachment,
        rvbRoomSizeSliderAttachment,
        rvbDampingSliderAttachment;

        juce::Label rvbBlendSliderLabel,
        rvbRoomSizeSliderLabel,
        rvbDampingSliderLabel;
        
        CustomToggle verbBypassToggle;
    APVTS::ButtonAttachment verbBypassAttachment;
        
        // Compessor Components
        CustomRotarySlider compThresholdSlider,
        compRatioSlider,
        compAttackSlider,
        compReleaseSlider,
        compMakeUpGainSlider;

        APVTS::SliderAttachment compThresholdSliderAttachment,
        compRatioSliderAttachment,
        compAttackSliderAttachment,
        compReleaseSliderAttachment,
        compMakeUpGainSliderAttachment;

        juce::Label compThresholdSliderLabel,
        compRatioSliderLabel,
        compAttackSliderLabel,
        compReleaseSliderLabel,
        compMakeUpGainSliderLabel;
        
        CustomToggle compBypassToggle;
    APVTS::ButtonAttachment compBypassAttachment;
        
    // EQ Components
    CustomRotarySlider
    eqLowPassFreqSlider,
    eqLowPassQSlider,
    eqHighPassFreqSlider,
    eqHighPassQSlider,
    eqLowMidFreqSlider,
    eqLowMidQSlider,
    eqLowMidGainSlider,
    eqMidFreqSlider,
    eqMidQSlider,
    eqMidGainSlider,
    eqHighMidFreqSlider,
    eqHighMidQSlider,
    eqHighMidGainSlider;

    APVTS::SliderAttachment
    eqLowPassFreqSliderAttachment,
    eqLowPassQSliderAttachment,
    eqHighPassFreqSliderAttachment,
    eqHighPassQSliderAttachment,
    eqLowMidFreqSliderAttachment,
    eqLowMidQSliderAttachment,
    eqLowMidGainSliderAttachment,
    eqMidFreqSliderAattachment,
    eqMidQSliderAttachment,
    eqMidGainSliderAttachment,
    eqHighMidFreqSliderAttachment,
    eqHighMidQSliderAttachment,
    eqHighMidGainSliderAttachment;

    juce::Label eqLowPassFreqSliderLabel,
    eqLowPassQSliderLabel,
    eqHighPassFreqSliderLabel,
    eqHighPassQSliderLabel,
    eqLowMidFreqSliderLabel,
    eqLowMidQSliderLabel,
    eqLowMidGainSliderLabel,
    eqMidFreqSliderLabel,
    eqMidQSliderLabel,
    eqMidGainSliderLabel,
    eqHighMidFreqSliderLabel,
    eqHighMidQSliderLabel,
    eqHighMidGainSliderLabel;

        CustomToggle eqBypassToggle;
    APVTS::ButtonAttachment eqBypassAttachment;
        
        // Phaser Components
        CustomRotarySlider phaserRateSlider,
        phaserDepthSlider,
        phaserFcSlider,
        phaserFeedbackSlider,
        phaserBlendSlider;
        
        APVTS::SliderAttachment phaserRateSliderAttachment,
        phaserDepthSliderAttachment,
        phaserFcSliderAttachment,
        phaserFeedbackSliderAttachment,
        phaserBlendSliderAttachment;
        
        juce::Label phaserRateSliderLabel,
        phaserDepthSliderLabel,
        phaserFcSliderLabel,
        phaserFeedbackSliderLabel,
        phaserBlendSliderLabel;
        
        CustomToggle phaserBypassToggle;
    APVTS::ButtonAttachment phaserBypassAttachment;
    
    // Delay Components
    CustomRotarySlider delayTimeSlider,
    delayBlendSlider;
    
    APVTS::SliderAttachment delayTimeSliderAttachment,
    delayBlendSliderAttachment;
    
    juce::Label delayTimeSliderLabel,
    delayBlendSliderLabel;
    
    CustomToggle delayBypassToggle;
    APVTS::ButtonAttachment delayBypassAttachment;
    
        
    juce::Label effectTitleLabel;
    
    std::vector<juce::Component*> getNoiseGateComps();
    
    std::vector<juce::Component*> getAmpComps();
    
    std::vector<juce::Component*> getReverbComps();
    
    std::vector<juce::Component*> getCompressorComps();
    
    std::vector<juce::Component*> getEqComps();
    
    std::vector<juce::Component*> getPhaserComps();

    std::vector<juce::Component*> getDelayComps();
    
    std::vector<juce::Component*> getChainComps();
        
    LookAndFeel lnf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProcessorGraphTestAudioProcessorEditor)
};
