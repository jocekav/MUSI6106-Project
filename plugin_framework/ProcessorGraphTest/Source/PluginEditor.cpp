/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


void LookAndFeel::drawRotarySlider (juce::Graphics& g,
                                    int x, int y, int width, int height,
                                    float sliderPosProportional,
                                    float rotaryStartAngle,
                                    float rotaryEndAngle,
                                    juce::Slider& slider)
{
    using namespace juce;
    
    auto bounds = Rectangle<float>(x, y, width, height);
    g.setColour(Colour(196u, 196u, 196u));
    g.fillEllipse(bounds);
    
    if (auto* customSlider = dynamic_cast<CustomRotarySlider*>(&slider))
    {
        auto center = bounds.getCentre();
        
        Path p;
        
        Rectangle<float> r;
        r.setLeft(center.getX() - 2);
        r.setRight(center.getX() + 2);
        r.setTop(bounds.getY());
        r.setBottom(center.getY());
        
        g.setColour(Colour(255u, 255u, 255u));
        
        p.addRoundedRectangle(r, 2.f);
        
        jassert(rotaryStartAngle < rotaryEndAngle);
        
        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);
        
        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));
        
        g.fillPath(p);
        
        g.setFont(customSlider->getTextHeight());
        auto text = customSlider->getDisplayString();
        auto strWidth = g.getCurrentFont().getStringWidth(text);
        
        r.setSize(strWidth + 4, customSlider->getTextHeight() + 2);
        r.setCentre(bounds.getCentreX(), bounds.getY() + bounds.getHeight() + 10);
        
        g.setColour(Colours::darkgrey);
        g.fillRect(r);
        
        g.setColour(Colours::white);
        g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);

    }
}

void LookAndFeel::drawLinearSlider(juce::Graphics& g,
                                    int x, int y, int width, int height,
                                   float sliderPos,
                                   float minSliderPos,
                                   float maxSliderPos,
                                   const juce::Slider::SliderStyle style,
                                   juce::Slider& slider)
{
    using namespace juce;
    
    auto bounds = Rectangle<float>(x, y, width, height);
    g.setColour(Colour(196u, 196u, 196u));
    
//    if (auto* customSlider = dynamic_cast<CustomVerticalSlider*>(&slider))
//    {
        auto center = bounds.getCentre();

        Path p;

        p.addRectangle ((float) x,
                            sliderPos, (float) width,
                            1.0f + height - sliderPos);

        auto baseColour = slider.findColour
     (Slider::rotarySliderFillColourId).withMultipliedSaturation
     (slider.isEnabled () ? 1.0f : 0.5f)
     .withMultipliedAlpha (0.8f);

        g.setColour (baseColour);
        g.fillPath (p);

        auto lineThickness = jmin (15.0f,
                             jmin (width, height) * 0.45f) * 0.1f;
        g.drawRect (slider.getLocalBounds ().
                                    toFloat (), lineThickness);

//    }
}

void LookAndFeel::drawToggleButton (juce::Graphics &g,
                       juce::ToggleButton &toggle,
                       bool shouldDrawButtonAsHighlighted,
                       bool shouldDrawButtonAsDown)
{
    using namespace juce;
    
    auto bounds = toggle.getLocalBounds();
    bounds.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth() - bounds.getWidth() / 4, bounds.getHeight());
    
    bool toggleState = toggle.getToggleState();
    
    
    if (auto* customSlider = dynamic_cast<CustomToggle*>(&toggle))
    {
        auto color = toggleState ? juce::Colour(196u, 196u, 196u) : juce::Colour(109u, 103u, 95u);
        g.setColour(color);
        g.fillRect(bounds);
    }
    else
    {
        auto color = toggleState ? juce::Colour(196u, 196u, 196u) : juce::Colour(109u, 103u, 95u);
        g.setColour(color);
        g.fillRect(bounds);
        
        auto signArea = bounds.removeFromRight((bounds.getWidth() / 4));
        signArea.removeFromBottom(bounds.getHeight() - bounds.getHeight() / 3.5);
        g.setColour(Colours::white);

        
        juce::Line<float> line (juce::Point<float> (signArea.getX() + 3, signArea.getY() + signArea.getHeight() / 2),
                                juce::Point<float> (signArea.getX() + signArea.getWidth() - 3, signArea.getY() + signArea.getHeight() / 2));
        g.drawLine (line, 2.0f);
        
        if (!toggleState)
        {
            juce::Line<float> line (juce::Point<float> (signArea.getX() + signArea.getWidth() / 2, signArea.getY() + 3),
                                    juce::Point<float> (signArea.getX() + signArea.getWidth() / 2, signArea.getY() + signArea.getHeight() - 2));
            g.drawLine (line, 2.0f);
        }
        
        auto text = toggle.getButtonText();
        g.drawFittedText(text, bounds, juce::Justification::centred, 1);
    }

}

void CustomToggle::paint(juce::Graphics &g)
{
    getLookAndFeel().drawToggleButton(g, *this, false, false);
}

void CustomRotarySlider::paint(juce::Graphics &g)
{
    using namespace juce;
    
    auto startAng = degreesToRadians(180.f + 45.f);
    auto endAng = degreesToRadians(180.f + - 45.f) + MathConstants<float>::twoPi;
    
    auto range = getRange();
    
    auto sliderBounds = getSliderBounds();
    
    g.setColour(Colour::fromRGBA(196u, 196u, 196u, 26.f));
    g.fillRect(sliderBounds);
    
    getLookAndFeel().drawRotarySlider(g, sliderBounds.getX() + 10,
                                      sliderBounds.getY() + 10,
                                      sliderBounds.getWidth() - 20,
                                      sliderBounds.getHeight() - 20,
                                      jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
                                      startAng,
                                      endAng,
                                      *this);
}

juce::Rectangle<int> CustomRotarySlider::getSliderBounds() const
{
//    return getLocalBounds();
    auto bounds = getLocalBounds();
    
    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    
    size -= getTextHeight() * 2;
    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(0);
    
    return r;
}

juce::String CustomRotarySlider::getDisplayString() const
{
    return juce::String(getValue());
}



//==============================================================================
ProcessorGraphTestAudioProcessorEditor::ProcessorGraphTestAudioProcessorEditor (ProcessorGraphTestAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),

//Gate Attachments
gateThresholdSlider(*audioProcessor.apvts.getParameter("NoiseGateThreshold_0"), "dB"),
gateRatioSlider(*audioProcessor.apvts.getParameter("NoiseGateRatio_0"), ""),
gateAttackSlider(*audioProcessor.apvts.getParameter("NoiseGateAttack_0"), "s"),
gateReleaseSlider(*audioProcessor.apvts.getParameter("NoiseGateRelease_0"), "s"),
gateBypassToggle(*audioProcessor.apvts.getParameter("NoiseGateBypass_0"), ""),

gateThresholdSliderAttachment(audioProcessor.apvts, "NoiseGateThreshold_0", gateThresholdSlider),
gateRatioSliderAttachment(audioProcessor.apvts, "NoiseGateRatio_0", gateRatioSlider),
gateAttackSliderAttachment(audioProcessor.apvts, "NoiseGateAttack_0", gateAttackSlider),
gateReleaseSliderAttachment(audioProcessor.apvts, "NoiseGateRelease_0", gateReleaseSlider),

//Reverb Attachments
rvbBlendSlider(*audioProcessor.apvts.getParameter("ReverbBlend_0"), "%"),
rvbRoomSizeSlider(*audioProcessor.apvts.getParameter("ReverbRoomSize_0"), ""),
rvbDampingSlider(*audioProcessor.apvts.getParameter("ReverbDamping_0"), ""),
verbBypassToggle(*audioProcessor.apvts.getParameter("ReverbBypass_0"), ""),

rvbBlendSliderAttachment(audioProcessor.apvts, "ReverbBlend_0", rvbBlendSlider),
rvbRoomSizeSliderAttachment(audioProcessor.apvts, "ReverbRoomSize_0", rvbRoomSizeSlider),
rvbDampingSliderAttachment(audioProcessor.apvts, "ReverbDamping_0", rvbDampingSlider),

//Compressor Attachments
compThresholdSlider(*audioProcessor.apvts.getParameter("CompressorThreshold_0"), "dB"),
compRatioSlider(*audioProcessor.apvts.getParameter("CompressorRatio_0"), ""),
compAttackSlider(*audioProcessor.apvts.getParameter("CompressorAttack_0"), "ms"),
compReleaseSlider(*audioProcessor.apvts.getParameter("CompressorRelease_0"), "ms"),
compMakeUpGainSlider(*audioProcessor.apvts.getParameter("CompressorMakeupGain_0"), "dB"),
compBypassToggle(*audioProcessor.apvts.getParameter("CompressorBypass_0"), ""),

compThresholdSliderAttachment(audioProcessor.apvts, "CompressorThreshold_0", compThresholdSlider),
compRatioSliderAttachment(audioProcessor.apvts, "CompressorRatio_0", compRatioSlider),
compAttackSliderAttachment(audioProcessor.apvts, "CompressorAttack_0", compAttackSlider),
compReleaseSliderAttachment(audioProcessor.apvts, "CompressorRelease_0", compReleaseSlider),
compMakeUpGainSliderAttachment(audioProcessor.apvts, "CompressorMakeupGain_0", compMakeUpGainSlider),

//Phaser Attachments
phaserRateSlider(*audioProcessor.apvts.getParameter("PhaserRate_0"), "Hz"),
phaserDepthSlider(*audioProcessor.apvts.getParameter("PhaserDepth_0"), ""),
phaserFcSlider(*audioProcessor.apvts.getParameter("PhaserFc_0"), "Hz"),
phaserFeedbackSlider(*audioProcessor.apvts.getParameter("PhaserFeedback_0"), "%"),
phaserBlendSlider(*audioProcessor.apvts.getParameter("PhaserBlend_0"), "%"),
phaserBypassToggle(*audioProcessor.apvts.getParameter("PhaserBypass_0"), ""),

phaserRateSliderAttachment(audioProcessor.apvts, "PhaserRate_0", phaserRateSlider),
phaserDepthSliderAttachment(audioProcessor.apvts, "PhaserDepth_0", phaserDepthSlider),
phaserFcSliderAttachment(audioProcessor.apvts, "PhaserFc_0", phaserFcSlider),
phaserFeedbackSliderAttachment(audioProcessor.apvts, "PhaserFeedback_0", phaserFeedbackSlider),
phaserBlendSliderAttachment(audioProcessor.apvts, "PhaserBlend_0", phaserBlendSlider),

//Gain Attachments
inputGainSlider(*audioProcessor.apvts.getParameter("GainValue_0"), "dB"),
outputGainSlider(*audioProcessor.apvts.getParameter("GainValue_1"), "dB"),

inputGainSliderAttachment(audioProcessor.apvts, "GainValue_0", inputGainSlider),
outputGainSliderAttachment(audioProcessor.apvts, "GainValue_1", outputGainSlider),

//EQ Attachments
eqBypassToggle(*audioProcessor.apvts.getParameter("EqualizerBypass_0"), ""),

//Amp Attachments
//ampBypassToggle(*audioProcessor.apvts.getParameter("ampBypass"), ""),

gateButton("GATE"),
ampButton("AMP"),
verbButton("REVERB"),
compressorButton("COMP"),
eqButton("EQ"),
phaserButton("PHASER")

{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    for (auto* comp : getReverbComps())
     {
         addAndMakeVisible(comp);
     }
     effectTitleLabel.setText("OUR REVERB", juce::dontSendNotification);
     
     for (auto* comp : getChainComps())
     {
         addAndMakeVisible(comp);
         comp -> setLookAndFeel(&lnf);
     }
     
     gateButton.setRadioGroupId(EffectShown);
     ampButton.setRadioGroupId(EffectShown);
     verbButton.setRadioGroupId(EffectShown);
     compressorButton.setRadioGroupId(EffectShown);
     eqButton.setRadioGroupId(EffectShown);
     phaserButton.setRadioGroupId(EffectShown);
     
     gateButton.onClick = [this] { updateToggleState (&gateButton); };
     ampButton.onClick = [this] { updateToggleState (&ampButton); };
     verbButton.onClick = [this] { updateToggleState (&verbButton); };
     compressorButton.onClick = [this] { updateToggleState (&compressorButton); };
     eqButton.onClick = [this] { updateToggleState (&eqButton); };
     phaserButton.onClick = [this] { updateToggleState (&phaserButton); };
     
     verbButton.setState(juce::Button::ButtonState::buttonDown);
    
    setSize (700, 500);
}

ProcessorGraphTestAudioProcessorEditor::~ProcessorGraphTestAudioProcessorEditor()
{
    for (auto* comp : getChainComps())
    {
        comp -> setLookAndFeel(nullptr);
    }
}

//==============================================================================
void ProcessorGraphTestAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.setColour(juce::Colour(35u, 33u, 33u));
        g.fillAll ();
        g.setColour(juce::Colour(109u, 103u, 95u));
        auto bounds = getLocalBounds();
        auto responseArea = bounds.removeFromBottom(bounds.getHeight() * 0.5);
        g.fillRect(bounds.getX() + 10, bounds.getY() + 10, bounds.getWidth() - 20, bounds.getHeight() - 20);
    //    switchEffectButton.setBounds(responseArea);
        
        g.setColour(juce::Colour(255u, 255u, 255u));
        juce::Line<float> line (juce::Point<float> (responseArea.getX(), responseArea.getY() + responseArea.getHeight() / 2 - 10),
                                juce::Point<float> (responseArea.getWidth(), responseArea.getY() + responseArea.getHeight() / 2 - 10));
        g.drawLine (line, 3.0f);
        
        g.drawText("IN", responseArea.getX(), responseArea.getY() + responseArea.getHeight() / 2, 20, 10, juce::Justification::centred);
        g.drawText("OUT", responseArea.getWidth() - 30, responseArea.getY() + responseArea.getHeight() / 2, 30, 10, juce::Justification::centred);
    }
void ProcessorGraphTestAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    auto bounds = getLocalBounds();
    auto chainBounds = bounds.removeFromBottom(bounds.getHeight() * 0.5);
    chainBounds.setBounds(chainBounds.getX() + 40, chainBounds.getY() + chainBounds.getHeight() / 3 - 10, chainBounds.getWidth() - 60, chainBounds.getHeight() / 3);
    auto gateArea = chainBounds.removeFromLeft(chainBounds.getWidth() / 6);
    auto compArea = chainBounds.removeFromLeft(chainBounds.getWidth() / 5);
    auto eqArea = chainBounds.removeFromLeft(chainBounds.getWidth() / 4);
    auto ampArea = chainBounds.removeFromLeft(chainBounds.getWidth() / 3);
    auto phaserArea = chainBounds.removeFromLeft(chainBounds.getWidth() / 2);
    auto verbArea = chainBounds.removeFromLeft(chainBounds.getWidth());
    
    eqButton.setBounds(eqArea);
    ampButton.setBounds(ampArea);
    gateButton.setBounds(gateArea);
    compressorButton.setBounds(compArea);
    phaserButton.setBounds(phaserArea);
    verbButton.setBounds(verbArea);
    
    gateBypassToggle.setBounds(gateArea.getX() + gateArea.getWidth() / 4, gateArea.getY() - 30, gateArea.getWidth() / 4, gateArea.getHeight() / 4);
//    ampBypassToggle.setBounds(ampArea.getX() + ampArea.getWidth() / 4, ampArea.getY() - 30, ampArea.getWidth() / 4, ampArea.getHeight() / 4);
//    eqBypassToggle.setBounds(eqArea.getX() + eqArea.getWidth() / 4, eqArea.getY() - 30, eqArea.getWidth() / 4, eqArea.getHeight() / 4);
    compBypassToggle.setBounds(compArea.getX() + compArea.getWidth() / 4, compArea.getY() - 30, compArea.getWidth() / 4, compArea.getHeight() / 4);
    verbBypassToggle.setBounds(verbArea.getX() + verbArea.getWidth() / 4, verbArea.getY() - 30, verbArea.getWidth() / 4, verbArea.getHeight() / 4);
    phaserBypassToggle.setBounds(phaserArea.getX() + phaserArea.getWidth() / 4, phaserArea.getY() - 30, phaserArea.getWidth() / 4, phaserArea.getHeight() / 4);

    bounds = getLocalBounds();
    auto inputArea = bounds.removeFromBottom(bounds.getHeight() * 0.5);
    inputArea.setBounds(-70, inputArea.getY() + inputArea.getHeight() - 80, inputArea.getWidth() / 3.5, inputArea.getHeight() / 3.5);
    
    inputGainSlider.setBounds(inputArea);
    
    bounds = getLocalBounds();
    auto outputArea = bounds.removeFromBottom(bounds.getHeight() * 0.5);
    outputArea.setBounds(outputArea.getX() + outputArea.getWidth() - 130, outputArea.getY() + outputArea.getHeight() - 80, outputArea.getWidth() / 3.5, outputArea.getHeight() / 3.5);
    
    outputGainSlider.setBounds(outputArea);
    
    drawNoiseGate();
    drawReverb();
    drawCompressor();
//    drawEQ();
    drawPhaser();
//    drawAmp();
        
    compressorButton.setState(juce::Button::ButtonState::buttonDown);
}

void ProcessorGraphTestAudioProcessorEditor::drawNoiseGate()
{
    auto bounds = getLocalBounds();
        auto responseArea = bounds.removeFromBottom(bounds.getHeight() * 0.5);
        
        auto labelArea = bounds.removeFromTop(bounds.getHeight() * .33);
        effectTitleLabel.setBounds(labelArea.getX() + 10, labelArea.getY() + 10, labelArea.getWidth() - 20, labelArea.getHeight() - 20);
        effectTitleLabel.setFont(juce::Font(32.f, juce::Font::bold));
        effectTitleLabel.setText("OUR NOISE GATE", juce::dontSendNotification);
        effectTitleLabel.setColour (juce::Label::textColourId, juce::Colours::white);
        effectTitleLabel.setJustificationType (juce::Justification::left);
        
        auto gateThresholdArea = bounds.removeFromLeft(bounds.getWidth() * 0.25);
        auto gateRatioArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
        auto gateAttackArea = bounds.removeFromLeft(bounds.getWidth() * 0.5);
        auto gateReleaseArea = bounds.removeFromLeft(bounds.getWidth());
        
        gateThresholdSlider.setBounds(gateThresholdArea.getX() + 20, gateThresholdArea.getY() + 20, gateThresholdArea.getWidth() - 40, gateThresholdArea.getHeight() - 40);
        gateThresholdSliderLabel.setBounds(gateThresholdArea.getX() + 20, gateThresholdArea.getY() + gateThresholdArea.getHeight() - 40, gateThresholdArea.getWidth() - 40, 20);
        gateThresholdSliderLabel.setText("Threshold", juce::dontSendNotification);
        gateThresholdSliderLabel.setJustificationType (juce::Justification::centred);
        
        gateRatioSlider.setBounds(gateRatioArea.getX() + 20, gateRatioArea.getY() + 20, gateRatioArea.getWidth() - 40, gateRatioArea.getHeight() - 40);
        gateRatioSliderLabel.setBounds(gateRatioArea.getX() + 20, gateRatioArea.getY() + gateRatioArea.getHeight() - 40, gateRatioArea.getWidth() - 40, 20);
        gateRatioSliderLabel.setText("Ratio", juce::dontSendNotification);
        gateRatioSliderLabel.setJustificationType (juce::Justification::centred);
        
        gateAttackSlider.setBounds(gateAttackArea.getX() + 20, gateAttackArea.getY() + 20, gateAttackArea.getWidth() - 40, gateAttackArea.getHeight() - 40);
        gateAttackSliderLabel.setBounds(gateAttackArea.getX() + 20, gateAttackArea.getY() + gateAttackArea.getHeight() - 40, gateAttackArea.getWidth() - 40, 20);
        gateAttackSliderLabel.setText("Attack", juce::dontSendNotification);
        gateAttackSliderLabel.setJustificationType (juce::Justification::centred);
        
        gateReleaseSlider.setBounds(gateReleaseArea.getX() + 20, gateReleaseArea.getY() + 20, gateReleaseArea.getWidth() - 40, gateReleaseArea.getHeight() - 40);
        gateReleaseSliderLabel.setBounds(gateReleaseArea.getX() + 20, gateReleaseArea.getY() + gateReleaseArea.getHeight() - 40, gateReleaseArea.getWidth() - 40, 20);
        gateReleaseSliderLabel.setText("Release", juce::dontSendNotification);
        gateReleaseSliderLabel.setJustificationType (juce::Justification::centred);
}

void ProcessorGraphTestAudioProcessorEditor::drawReverb()
{
    auto bounds = getLocalBounds();
       auto responseArea = bounds.removeFromBottom(bounds.getHeight() * 0.5);
       
       auto labelArea = bounds.removeFromTop(bounds.getHeight() * .33);
       effectTitleLabel.setBounds(labelArea.getX() + 10, labelArea.getY() + 10, labelArea.getWidth() - 20, labelArea.getHeight() - 20);
       effectTitleLabel.setFont(juce::Font(32.f, juce::Font::bold));
       effectTitleLabel.setText("OUR REVERB", juce::dontSendNotification);
       effectTitleLabel.setColour (juce::Label::textColourId, juce::Colours::white);
       effectTitleLabel.setJustificationType (juce::Justification::left);
       
       auto rvbBlendArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
       auto rvbRoomSizeArea = bounds.removeFromLeft(bounds.getWidth() * 0.63);
       auto rvbDampingArea = bounds.removeFromLeft(bounds.getWidth());
       
       rvbBlendSlider.setBounds(rvbBlendArea.getX() + 20, rvbBlendArea.getY() + 20, rvbBlendArea.getWidth() - 40, rvbBlendArea.getHeight() - 40);
       rvbBlendSliderLabel.setBounds(rvbBlendArea.getX() + 20, rvbBlendArea.getY() + rvbBlendArea.getHeight() - 40, rvbBlendArea.getWidth() - 40, 20);
       rvbBlendSliderLabel.setText("Blend", juce::dontSendNotification);
       rvbBlendSliderLabel.setJustificationType (juce::Justification::centred);
       
       rvbRoomSizeSlider.setBounds(rvbRoomSizeArea.getX() + 20, rvbRoomSizeArea.getY() + 20, rvbRoomSizeArea.getWidth() - 40, rvbRoomSizeArea.getHeight() - 40);
       rvbRoomSizeSliderLabel.setBounds(rvbRoomSizeArea.getX() + 20, rvbRoomSizeArea.getY() + rvbRoomSizeArea.getHeight() - 40, rvbRoomSizeArea.getWidth() - 40, 20);
       rvbRoomSizeSliderLabel.setText("Room Size", juce::dontSendNotification);
       rvbRoomSizeSliderLabel.setJustificationType (juce::Justification::centred);
       
       rvbDampingSlider.setBounds(rvbDampingArea.getX() + 20, rvbDampingArea.getY() + 20, rvbDampingArea.getWidth() - 40, rvbDampingArea.getHeight() - 40);
       rvbDampingSliderLabel.setBounds(rvbDampingArea.getX() + 20, rvbDampingArea.getY() + rvbDampingArea.getHeight() - 40, rvbDampingArea.getWidth() - 40, 20);
       rvbDampingSliderLabel.setText("Damping", juce::dontSendNotification);
       rvbDampingSliderLabel.setJustificationType (juce::Justification::centred);
}

void ProcessorGraphTestAudioProcessorEditor::drawCompressor()
{
    auto bounds = getLocalBounds();
     auto responseArea = bounds.removeFromBottom(bounds.getHeight() * 0.5);
     
     auto labelArea = bounds.removeFromTop(bounds.getHeight() * .33);
     effectTitleLabel.setBounds(labelArea.getX() + 10, labelArea.getY() + 10, labelArea.getWidth() - 20, labelArea.getHeight() - 20);
     effectTitleLabel.setFont(juce::Font(32.f, juce::Font::bold));
     effectTitleLabel.setText("OUR COMPRESSOR", juce::dontSendNotification);
     effectTitleLabel.setColour (juce::Label::textColourId, juce::Colours::white);
     effectTitleLabel.setJustificationType (juce::Justification::left);
     
     auto compThresholdArea = bounds.removeFromLeft(bounds.getWidth() * 0.2);
     auto compRatioArea = bounds.removeFromLeft(bounds.getWidth() * 0.25);
     auto compAttackArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
     auto compReleaseArea = bounds.removeFromLeft(bounds.getWidth() * 0.5);
     auto compMakeUpGainArea = bounds.removeFromLeft(bounds.getWidth());
     
     compThresholdSlider.setBounds(compThresholdArea.getX() + 20, compThresholdArea.getY() + 20, compThresholdArea.getWidth() - 40, compThresholdArea.getHeight() - 40);
     compThresholdSliderLabel.setBounds(compThresholdArea.getX() + 20, compThresholdArea.getY() + compThresholdArea.getHeight() - 40, compThresholdArea.getWidth() - 40, 20);
     compThresholdSliderLabel.setText("Threshold", juce::dontSendNotification);
     compThresholdSliderLabel.setJustificationType (juce::Justification::centred);
     
     compRatioSlider.setBounds(compRatioArea.getX() + 20, compRatioArea.getY() + 20, compRatioArea.getWidth() - 40, compRatioArea.getHeight() - 40);
     compRatioSliderLabel.setBounds(compRatioArea.getX() + 20, compRatioArea.getY() + compRatioArea.getHeight() - 40, compRatioArea.getWidth() - 40, 20);
     compRatioSliderLabel.setText("Ratio", juce::dontSendNotification);
     compRatioSliderLabel.setJustificationType (juce::Justification::centred);
     
     compAttackSlider.setBounds(compAttackArea.getX() + 20, compAttackArea.getY() + 20, compAttackArea.getWidth() - 40, compAttackArea.getHeight() - 40);
     compAttackSliderLabel.setBounds(compAttackArea.getX() + 20, compAttackArea.getY() + compAttackArea.getHeight() - 40, compAttackArea.getWidth() - 40, 20);
     compAttackSliderLabel.setText("Attack", juce::dontSendNotification);
     compAttackSliderLabel.setJustificationType (juce::Justification::centred);
     
     compReleaseSlider.setBounds(compReleaseArea.getX() + 20, compReleaseArea.getY() + 20, compReleaseArea.getWidth() - 40, compReleaseArea.getHeight() - 40);
     compReleaseSliderLabel.setBounds(compReleaseArea.getX() + 20, compReleaseArea.getY() + compReleaseArea.getHeight() - 40, compReleaseArea.getWidth() - 40, 20);
     compReleaseSliderLabel.setText("Release", juce::dontSendNotification);
     compReleaseSliderLabel.setJustificationType (juce::Justification::centred);
     
     compMakeUpGainSlider.setBounds(compMakeUpGainArea.getX() + 20, compMakeUpGainArea.getY() + 20, compMakeUpGainArea.getWidth() - 40, compMakeUpGainArea.getHeight() - 40);
     compMakeUpGainSliderLabel.setBounds(compMakeUpGainArea.getX() + 20, compMakeUpGainArea.getY() + compMakeUpGainArea.getHeight() - 40, compMakeUpGainArea.getWidth() - 40, 20);
     compMakeUpGainSliderLabel.setText("Makeup Gain", juce::dontSendNotification);
     compMakeUpGainSliderLabel.setJustificationType (juce::Justification::centred);
}

void ProcessorGraphTestAudioProcessorEditor::drawPhaser()
{
    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromBottom(bounds.getHeight() * 0.5);
    
    auto labelArea = bounds.removeFromTop(bounds.getHeight() * .33);
    effectTitleLabel.setBounds(labelArea.getX() + 10, labelArea.getY() + 10, labelArea.getWidth() - 20, labelArea.getHeight() - 20);
    effectTitleLabel.setFont(juce::Font(32.f, juce::Font::bold));
    effectTitleLabel.setText("OUR PHASER", juce::dontSendNotification);
    effectTitleLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    effectTitleLabel.setJustificationType (juce::Justification::left);
    
    auto phaserRateArea = bounds.removeFromLeft(bounds.getWidth() * 0.2);
    auto phaserDepthArea = bounds.removeFromLeft(bounds.getWidth() * 0.25);
    auto phaserFcArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto phaserFeedbackArea = bounds.removeFromLeft(bounds.getWidth() * 0.5);
    auto phaserBlendArea = bounds.removeFromLeft(bounds.getWidth());
    
    phaserRateSlider.setBounds(phaserRateArea.getX() + 20, phaserRateArea.getY() + 20, phaserRateArea.getWidth() - 40, phaserRateArea.getHeight() - 40);
    phaserRateSliderLabel.setBounds(phaserRateArea.getX() + 20, phaserRateArea.getY() + phaserRateArea.getHeight() - 40, phaserRateArea.getWidth() - 40, 20);
    phaserRateSliderLabel.setText("Rate", juce::dontSendNotification);
    phaserRateSliderLabel.setJustificationType (juce::Justification::centred);
    
    phaserDepthSlider.setBounds(phaserDepthArea.getX() + 20, phaserDepthArea.getY() + 20, phaserDepthArea.getWidth() - 40, phaserDepthArea.getHeight() - 40);
    phaserDepthSliderLabel.setBounds(phaserDepthArea.getX() + 20, phaserDepthArea.getY() + phaserDepthArea.getHeight() - 40, phaserDepthArea.getWidth() - 40, 20);
    phaserDepthSliderLabel.setText("Depth", juce::dontSendNotification);
    phaserDepthSliderLabel.setJustificationType (juce::Justification::centred);
    
    phaserFcSlider.setBounds(phaserFcArea.getX() + 20, phaserFcArea.getY() + 20, phaserFcArea.getWidth() - 40, phaserFcArea.getHeight() - 40);
    phaserFcSliderLabel.setBounds(phaserFcArea.getX() + 20, phaserFcArea.getY() + phaserFcArea.getHeight() - 40, phaserFcArea.getWidth() - 40, 20);
    phaserFcSliderLabel.setText("Center Freq", juce::dontSendNotification);
    phaserFcSliderLabel.setJustificationType (juce::Justification::centred);
    
    phaserFeedbackSlider.setBounds(phaserFeedbackArea.getX() + 20, phaserFeedbackArea.getY() + 20, phaserFeedbackArea.getWidth() - 40, phaserFeedbackArea.getHeight() - 40);
    phaserFeedbackSliderLabel.setBounds(phaserFeedbackArea.getX() + 20, phaserFeedbackArea.getY() + phaserFeedbackArea.getHeight() - 40, phaserFeedbackArea.getWidth() - 40, 20);
    phaserFeedbackSliderLabel.setText("Feedback", juce::dontSendNotification);
    phaserFeedbackSliderLabel.setJustificationType (juce::Justification::centred);
    
    phaserBlendSlider.setBounds(phaserBlendArea.getX() + 20, phaserBlendArea.getY() + 20, phaserBlendArea.getWidth() - 40, phaserBlendArea.getHeight() - 40);
    phaserBlendSliderLabel.setBounds(phaserBlendArea.getX() + 20, phaserBlendArea.getY() + phaserBlendArea.getHeight() - 40, phaserBlendArea.getWidth() - 40, 20);
    phaserBlendSliderLabel.setText("Blend", juce::dontSendNotification);
    phaserBlendSliderLabel.setJustificationType (juce::Justification::centred);
}

void ProcessorGraphTestAudioProcessorEditor::updateToggleState(juce::Button* button)
{
    bool showGate = false;
    bool showAmp = false;
    bool showVerb = false;
    bool showComp = false;
    bool showEq = false;
    bool showPhaser = false;
    
    if (button == &gateButton)
    {
        showGate = true;
        for (auto* comp : getNoiseGateComps())
        {
            addAndMakeVisible(comp);
        }
        effectTitleLabel.setText("OUR NOISE GATE", juce::dontSendNotification);
    }
    if (button == &ampButton)
    {
        showAmp = true;
        for (auto* comp : getAmpComps())
        {
            addAndMakeVisible(comp);
        }
        effectTitleLabel.setText("OUR AMP", juce::dontSendNotification);
    }
    if (button == &verbButton)
    {
        showVerb = true;
        for (auto* comp : getReverbComps())
        {
            addAndMakeVisible(comp);
        }
        
        effectTitleLabel.setText("OUR REVERB", juce::dontSendNotification);
    }
    if (button == &compressorButton)
    {
        showComp = true;
        for (auto* comp : getCompressorComps())
        {
            addAndMakeVisible(comp);
        }
        
        effectTitleLabel.setText("OUR COMPRESSOR", juce::dontSendNotification);
    }
    if (button == &eqButton)
    {
        showEq = true;
        for (auto* comp : getEqComps())
        {
            addAndMakeVisible(comp);
        }
        
        effectTitleLabel.setText("OUR EQ", juce::dontSendNotification);
    }
    if (button == &phaserButton)
    {
        showPhaser = true;
        for (auto* comp : getPhaserComps())
        {
            addAndMakeVisible(comp);
        }
        
        effectTitleLabel.setText("OUR PHASER", juce::dontSendNotification);
    }
    
    
    for (auto* comp : getNoiseGateComps())
    {
        comp->setVisible(showGate);
    }
    for (auto* comp : getAmpComps())
    {
        comp->setVisible(showAmp);
    }
    for (auto* comp : getReverbComps())
    {
        comp->setVisible(showVerb);
    }
    for (auto* comp : getCompressorComps())
    {
        comp->setVisible(showComp);
    }
    for (auto* comp : getEqComps())
    {
        comp->setVisible(showEq);
    }
    for (auto* comp : getPhaserComps())
    {
        comp->setVisible(showPhaser);
    }
    
    effectTitleLabel.setVisible(true);
    
}

std::vector<juce::Component*> ProcessorGraphTestAudioProcessorEditor::getChainComps()
{
    return
    {
        &gateButton,
        &ampButton,
        &verbButton,
        &compressorButton,
        &eqButton,
        &phaserButton,
        &inputGainSlider,
        &outputGainSlider,
        &gateBypassToggle,
        &compBypassToggle,
        &verbBypassToggle,
        &eqBypassToggle,
        &phaserBypassToggle,
//        &ampBypassToggle
    };
}


std::vector<juce::Component*> ProcessorGraphTestAudioProcessorEditor::getNoiseGateComps()
{
    return
    {
        &gateThresholdSlider,
        &gateRatioSlider,
        &gateAttackSlider,
        &gateReleaseSlider,
        &effectTitleLabel,
        &gateThresholdSliderLabel,
        &gateRatioSliderLabel,
        &gateAttackSliderLabel,
        &gateReleaseSliderLabel
    };
}


std::vector<juce::Component*> ProcessorGraphTestAudioProcessorEditor::getAmpComps()
{
    return
    {
//        &ampDriveSlider,
//        &ampBlendSlider,
//        &ampPreLpfSlider,
//        &ampPreHpfSlider,
//        &ampEmphasisFreqSlider,
//        &ampEmphasisGainSlider,
//        &ampPostLpfSlider,
//        &ampPostHpfSlider,
//        &effectTitleLabel,
//        &ampDriveSliderLabel,
//        &ampBlendSliderLabel,
//        &ampPreLpfSliderLabel,
//        &ampPreHpfSliderLabel,
//        &ampEmphasisFreqSliderLabel,
//        &ampEmphasisGainSliderLabel,
//        &ampPostLpfSliderLabel,
//        &ampPostHpfSliderLabel,
//        &ampTypeComboBox
    };
}

std::vector<juce::Component*> ProcessorGraphTestAudioProcessorEditor::getReverbComps()
{
    return
    {
        &rvbBlendSlider,
        &rvbRoomSizeSlider,
        &rvbDampingSlider,
        &rvbBlendSliderLabel,
        &rvbRoomSizeSliderLabel,
        &rvbDampingSliderLabel,
        &effectTitleLabel
    };
}


std::vector<juce::Component*> ProcessorGraphTestAudioProcessorEditor::getCompressorComps()
{
    return
    {
        &compThresholdSlider,
        &compRatioSlider,
        &compAttackSlider,
        &compReleaseSlider,
        &compMakeUpGainSlider,
        &compThresholdSliderLabel,
        &compRatioSliderLabel,
        &compAttackSliderLabel,
        &compReleaseSliderLabel,
        &compMakeUpGainSliderLabel,
        &effectTitleLabel
    };
}

std::vector<juce::Component*> ProcessorGraphTestAudioProcessorEditor::getEqComps()
{
    return
    {
//        &eqLowCutFreqSlider,
//        &eqLowCutSlopeSlider,
//        &eqHighCutFreqSlider,
//        &eqHighCutSlopeSlider,
//        &eqPeakFreqSlider,
//        &eqPeakGainSlider,
//        &eqPeakQSlider,
//        &eqLowCutFreqSliderLabel,
//        &eqLowCutSlopeSliderLabel,
//        &eqHighCutFreqSliderLabel,
//        &eqHighCutSlopeSliderLabel,
//        &eqPeakFreqSliderLabel,
//        &eqPeakGainSliderLabel,
//        &eqPeakQSliderLabel
    };
}

std::vector<juce::Component*> ProcessorGraphTestAudioProcessorEditor::getPhaserComps()
{
    return
        {
            &phaserRateSlider,
            &phaserDepthSlider,
            &phaserFcSlider,
            &phaserFeedbackSlider,
            &phaserBlendSlider,
            &phaserRateSliderLabel,
            &phaserDepthSliderLabel,
            &phaserFcSliderLabel,
            &phaserFeedbackSliderLabel,
            &phaserBlendSliderLabel
        };
}


