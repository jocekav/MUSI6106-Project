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
    
    
    if (auto* customToggle = dynamic_cast<CustomToggle*>(&toggle))
    {
        auto color = toggleState ? juce::Colour(196u, 196u, 196u) : juce::Colour(66u, 245u, 72u);
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

void LookAndFeel::drawComboBox(juce::Graphics& g,
                  int     width,
                  int     height,
                  bool    isButtonDown,
                  int     ,
                  int     ,
                  int     ,
                  int     ,
                  juce::ComboBox &box)
{
    using namespace juce;
    
    auto cornerSize = 0.0f;
    Rectangle<int> boxBounds (0, 0, width, height);

    g.setColour (juce::Colour(134u, 130u, 124u));
    g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);

    g.setColour (Colours::white);
    g.drawRoundedRectangle (boxBounds.toFloat().reduced (0.5f, 0.5f), cornerSize, 1.0f);

    Rectangle<int> arrowZone (width - 30, 0, 20, height);
    Path path;
    path.startNewSubPath ((float) arrowZone.getX() + 3.0f, (float) arrowZone.getCentreY() - 2.0f);
    path.lineTo ((float) arrowZone.getCentreX(), (float) arrowZone.getCentreY() + 3.0f);
    path.lineTo ((float) arrowZone.getRight() - 3.0f, (float) arrowZone.getCentreY() - 2.0f);

    g.setColour (box.findColour (ComboBox::arrowColourId).withAlpha ((box.isEnabled() ? 0.9f : 0.2f)));
    g.strokePath (path, PathStrokeType (2.0f));
}

void CustomToggle::paint(juce::Graphics &g)
{
    getLookAndFeel().drawToggleButton(g, *this, false, false);
}

void CustomComboBox::paint(juce::Graphics &g)
{
    getLookAndFeel().drawComboBox(g,
                                  getLocalBounds().getWidth(),
                                  getLocalBounds().getHeight(),
                                  false, 1, 1, 1, 1,
                                  *this);
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
gateBypassAttachment(audioProcessor.apvts, "NoiseGateBypass_0", gateBypassToggle),

//Reverb Attachments
rvbBlendSlider(*audioProcessor.apvts.getParameter("ReverbBlend_0"), "%"),
rvbRoomSizeSlider(*audioProcessor.apvts.getParameter("ReverbRoomSize_0"), ""),
rvbDampingSlider(*audioProcessor.apvts.getParameter("ReverbDamping_0"), ""),
verbBypassToggle(*audioProcessor.apvts.getParameter("ReverbBypass_0"), ""),

rvbBlendSliderAttachment(audioProcessor.apvts, "ReverbBlend_0", rvbBlendSlider),
rvbRoomSizeSliderAttachment(audioProcessor.apvts, "ReverbRoomSize_0", rvbRoomSizeSlider),
rvbDampingSliderAttachment(audioProcessor.apvts, "ReverbDamping_0", rvbDampingSlider),
verbBypassAttachment(audioProcessor.apvts, "ReverbBypass_0", verbBypassToggle),

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
compBypassAttachment(audioProcessor.apvts, "CompressorBypass_0", compBypassToggle),

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
phaserBypassAttachment(audioProcessor.apvts, "PhaserBypass_0", phaserBypassToggle),

//Gain Attachments
inputGainSlider(*audioProcessor.apvts.getParameter("GainValue_0"), "dB"),
outputGainSlider(*audioProcessor.apvts.getParameter("GainValue_1"), "dB"),

inputGainSliderAttachment(audioProcessor.apvts, "GainValue_0", inputGainSlider),
outputGainSliderAttachment(audioProcessor.apvts, "GainValue_1", outputGainSlider),

//EQ Attachments
eqLowPassFreqSlider(*audioProcessor.apvts.getParameter("EqualizerLPF_0"), "Hz"),
eqLowPassQSlider(*audioProcessor.apvts.getParameter("EqualizerLPFQ_0"), ""),
eqHighPassFreqSlider(*audioProcessor.apvts.getParameter("EqualizerHPF_0"), "Hz"),
eqHighPassQSlider(*audioProcessor.apvts.getParameter("EqualizerHPFQ_0"), ""),
eqLowMidFreqSlider(*audioProcessor.apvts.getParameter("EqualizerLMF_0"), "Hz"),
eqLowMidQSlider(*audioProcessor.apvts.getParameter("EqualizerLMQ_0"), ""),
eqLowMidGainSlider(*audioProcessor.apvts.getParameter("EqualizerLMGain_0"), "dB"),
eqMidFreqSlider(*audioProcessor.apvts.getParameter("EqualizerMF_0"), "Hz"),
eqMidQSlider(*audioProcessor.apvts.getParameter("EqualizerMQ_0"), ""),
eqMidGainSlider(*audioProcessor.apvts.getParameter("EqualizerMGain_0"), "dB"),
eqHighMidFreqSlider(*audioProcessor.apvts.getParameter("EqualizerHMF_0"), "Hz"),
eqHighMidQSlider(*audioProcessor.apvts.getParameter("EqualizerHMQ_0"), ""),
eqHighMidGainSlider(*audioProcessor.apvts.getParameter("EqualizerHMGain_0"), "dB"),
eqBypassToggle(*audioProcessor.apvts.getParameter("EqualizerBypass_0"), ""),

eqLowPassFreqSliderAttachment(audioProcessor.apvts, "EqualizerLPF_0", eqLowPassFreqSlider),
eqLowPassQSliderAttachment(audioProcessor.apvts, "EqualizerLPFQ_0", eqLowPassQSlider),
eqHighPassFreqSliderAttachment(audioProcessor.apvts, "EqualizerHPF_0", eqHighPassFreqSlider),
eqHighPassQSliderAttachment(audioProcessor.apvts, "EqualizerHPFQ_0", eqHighPassQSlider),
eqLowMidFreqSliderAttachment(audioProcessor.apvts, "EqualizerLMF_0", eqLowMidFreqSlider),
eqLowMidQSliderAttachment(audioProcessor.apvts, "EqualizerLMQ_0", eqLowMidQSlider),
eqLowMidGainSliderAttachment(audioProcessor.apvts, "EqualizerLMGain_0", eqLowMidGainSlider),
eqMidFreqSliderAattachment(audioProcessor.apvts, "EqualizerMF_0", eqMidFreqSlider),
eqMidQSliderAttachment(audioProcessor.apvts, "EqualizerMQ_0", eqMidQSlider),
eqMidGainSliderAttachment(audioProcessor.apvts, "EqualizerMGain_0", eqMidGainSlider),
eqHighMidFreqSliderAttachment(audioProcessor.apvts, "EqualizerHMF_0", eqHighMidFreqSlider),
eqHighMidQSliderAttachment(audioProcessor.apvts, "EqualizerHMQ_0", eqHighMidQSlider),
eqHighMidGainSliderAttachment(audioProcessor.apvts, "EqualizerHMGain_0", eqHighMidGainSlider),
eqBypassAttachment(audioProcessor.apvts, "EqualizerBypass_0", eqBypassToggle),


//Amp Attachments
//ampBypassToggle(*audioProcessor.apvts.getParameter("ampBypass"), ""),
//ampBypassAttachment(audioProcessor.apvts, "AmpBypass", ampBypassToggle),
ampTypeComboBox(*audioProcessor.apvts.getParameter("Amp_0"), ""),
ampTypeComboBoxAttachment(audioProcessor.apvts, "Amp_0", ampTypeComboBox),

//Delay Attachments
delayTimeSlider(*audioProcessor.apvts.getParameter("DelayTime_0"), "%"),
delayBlendSlider(*audioProcessor.apvts.getParameter("DelayBlend_0"), "%"),
delayBypassToggle(*audioProcessor.apvts.getParameter("DelayBypass_0"), ""),

delayTimeSliderAttachment(audioProcessor.apvts, "DelayTime_0", delayTimeSlider),
delayBlendSliderAttachment(audioProcessor.apvts, "DelayBlend_0", delayBlendSlider),
delayBypassAttachment(audioProcessor.apvts, "DelayBypass_0", delayBypassToggle),

gateButton("GATE"),
ampButton("AMP"),
verbButton("REVERB"),
compressorButton("COMP"),
eqButton("EQ"),
phaserButton("PHASER"),
delayButton("DELAY")

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
     delayButton.setRadioGroupId(EffectShown);
     
     gateButton.onClick = [this] { updateToggleState (&gateButton); };
     ampButton.onClick = [this] { updateToggleState (&ampButton); };
     verbButton.onClick = [this] { updateToggleState (&verbButton); };
     compressorButton.onClick = [this] { updateToggleState (&compressorButton); };
     eqButton.onClick = [this] { updateToggleState (&eqButton); };
     phaserButton.onClick = [this] { updateToggleState (&phaserButton); };
     delayButton.onClick = [this] { updateToggleState (&delayButton); };
     
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
    chainBounds.setBounds(chainBounds.getX() + 40, chainBounds.getY() + chainBounds.getHeight() / 3 - 10, chainBounds.getWidth() - 60, chainBounds.getHeight() / 4);
    auto gateArea = chainBounds.removeFromLeft(chainBounds.getWidth() / 7);
    auto compArea = chainBounds.removeFromLeft(chainBounds.getWidth() / 6);
    auto eqArea = chainBounds.removeFromLeft(chainBounds.getWidth() / 5);
    auto ampArea = chainBounds.removeFromLeft(chainBounds.getWidth() / 4);
    auto phaserArea = chainBounds.removeFromLeft(chainBounds.getWidth() / 3);
    auto delayArea = chainBounds.removeFromLeft(chainBounds.getWidth() / 2);
    auto verbArea = chainBounds.removeFromLeft(chainBounds.getWidth());
    
    eqButton.setBounds(eqArea);
    ampButton.setBounds(ampArea);
    gateButton.setBounds(gateArea);
    compressorButton.setBounds(compArea);
    phaserButton.setBounds(phaserArea);
    delayButton.setBounds(delayArea);
    verbButton.setBounds(verbArea);
    
    gateBypassToggle.setBounds(gateArea.getX() + gateArea.getWidth() / 4, gateArea.getY() - 30, gateArea.getWidth() / 4, gateArea.getHeight() / 4);
//    ampBypassToggle.setBounds(ampArea.getX() + ampArea.getWidth() / 4, ampArea.getY() - 30, ampArea.getWidth() / 4, ampArea.getHeight() / 4);
    eqBypassToggle.setBounds(eqArea.getX() + eqArea.getWidth() / 4, eqArea.getY() - 30, eqArea.getWidth() / 4, eqArea.getHeight() / 4);
    compBypassToggle.setBounds(compArea.getX() + compArea.getWidth() / 4, compArea.getY() - 30, compArea.getWidth() / 4, compArea.getHeight() / 4);
    verbBypassToggle.setBounds(verbArea.getX() + verbArea.getWidth() / 4, verbArea.getY() - 30, verbArea.getWidth() / 4, verbArea.getHeight() / 4);
    delayBypassToggle.setBounds(delayArea.getX() + delayArea.getWidth() / 4, delayArea.getY() - 30, delayArea.getWidth() / 4, delayArea.getHeight() / 4);
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
    drawEQ();
    drawPhaser();
    drawDelay();
    drawAmp();
        
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

void ProcessorGraphTestAudioProcessorEditor::drawEQ()
{
    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromBottom(bounds.getHeight() * 0.5);
    
    auto labelArea = bounds.removeFromTop(bounds.getHeight() * .33);
    effectTitleLabel.setBounds(labelArea.getX() + 10, labelArea.getY() + 10, labelArea.getWidth() - 20, labelArea.getHeight() - 20);
    effectTitleLabel.setFont(juce::Font(32.f, juce::Font::bold));
    effectTitleLabel.setText("OUR EQ", juce::dontSendNotification);
    effectTitleLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    effectTitleLabel.setJustificationType (juce::Justification::left);
    
    auto eqLowArea = bounds.removeFromLeft(bounds.getWidth() * 0.2);
    auto eqLowFreqArea = eqLowArea.removeFromTop(bounds.getHeight() * 0.5);
    auto eqLowQArea = eqLowArea;
    
    auto eqLowMidArea = bounds.removeFromLeft(bounds.getWidth() * 0.25);
    auto eqLowMidFreqArea = eqLowMidArea.removeFromTop(eqLowMidArea.getHeight() * 0.33);
    auto eqLowMidQArea = eqLowMidArea.removeFromTop(eqLowMidArea.getHeight() * .5);
    auto eqLowMidGainArea = eqLowMidArea.removeFromTop(eqLowMidArea.getHeight());
    
    auto eqMidArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto eqMidFreqArea = eqMidArea.removeFromTop(eqMidArea.getHeight() * 0.33);
    auto eqMidQArea = eqMidArea.removeFromTop(eqMidArea.getHeight() * .5);
    auto eqMidGainArea = eqMidArea.removeFromTop(eqMidArea.getHeight());
    
    auto eqHighMidArea = bounds.removeFromLeft(bounds.getWidth() * 0.5);
    auto eqHighMidFreqArea = eqHighMidArea.removeFromTop(eqHighMidArea.getHeight() * 0.33);
    auto eqHighMidQArea = eqHighMidArea.removeFromTop(eqHighMidArea.getHeight() * .5);
    auto eqHighMidGainArea = eqHighMidArea.removeFromTop(eqHighMidArea.getHeight());
    
    auto eqHighArea = bounds.removeFromLeft(bounds.getWidth());
    auto eqHighFreqArea = eqHighArea.removeFromTop(bounds.getHeight() * 0.5);
    auto eqHighQArea = eqHighArea;
    
    eqLowPassFreqSlider.setBounds(eqLowFreqArea.getX() - 20, eqLowFreqArea.getY() - 10, eqLowFreqArea.getWidth(), eqLowFreqArea.getHeight());
    eqLowPassFreqSliderLabel.setBounds(eqLowFreqArea.getX() - 20, eqLowFreqArea.getY() + eqLowFreqArea.getHeight() - 25, eqLowFreqArea.getWidth(), 10);
    eqLowPassFreqSliderLabel.setText("Low Freq", juce::dontSendNotification);
    eqLowPassFreqSliderLabel.setJustificationType (juce::Justification::centred);
    
    eqLowPassQSlider.setBounds(eqLowQArea.getX() - 20, eqLowQArea.getY() - 10, eqLowQArea.getWidth(), eqLowQArea.getHeight());
    eqLowPassQSliderLabel.setBounds(eqLowQArea.getX() - 20, eqLowQArea.getY() + eqLowQArea.getHeight() - 25, eqLowQArea.getWidth(), 10);
    eqLowPassQSliderLabel.setText("Low Q", juce::dontSendNotification);
    eqLowPassQSliderLabel.setJustificationType (juce::Justification::centred);
    
    eqLowMidFreqSlider.setBounds(eqLowMidFreqArea.getX() - 20, eqLowMidFreqArea.getY() - 10, eqLowMidFreqArea.getWidth(), eqLowMidFreqArea.getHeight());
    eqLowMidFreqSliderLabel.setBounds(eqLowMidFreqArea.getX() - 20, eqLowMidFreqArea.getY() + eqLowMidFreqArea.getHeight() - 25, eqLowMidFreqArea.getWidth(), 10);
    eqLowMidFreqSliderLabel.setText("LowMid Freq", juce::dontSendNotification);
    eqLowMidFreqSliderLabel.setJustificationType (juce::Justification::centred);
    
    eqLowMidQSlider.setBounds(eqLowMidQArea.getX() - 20, eqLowMidQArea.getY() - 10, eqLowMidQArea.getWidth(), eqLowMidQArea.getHeight());
    eqLowMidQSliderLabel.setBounds(eqLowMidQArea.getX() - 20, eqLowMidQArea.getY() + eqLowMidQArea.getHeight() - 25, eqLowMidQArea.getWidth(), 10);
    eqLowMidQSliderLabel.setText("LowMid Q", juce::dontSendNotification);
    eqLowMidQSliderLabel.setJustificationType (juce::Justification::centred);
    
    eqLowMidGainSlider.setBounds(eqLowMidGainArea.getX() - 20, eqLowMidGainArea.getY() - 10, eqLowMidGainArea.getWidth(), eqLowMidGainArea.getHeight());
    eqLowMidGainSliderLabel.setBounds(eqLowMidGainArea.getX() - 20, eqLowMidGainArea.getY() + eqLowMidGainArea.getHeight() - 25, eqLowMidGainArea.getWidth(), 10);
    eqLowMidGainSliderLabel.setText("LowMid Gain", juce::dontSendNotification);
    eqLowMidGainSliderLabel.setJustificationType (juce::Justification::centred);
    
    eqMidFreqSlider.setBounds(eqMidFreqArea.getX() - 20, eqMidFreqArea.getY() - 10, eqMidFreqArea.getWidth(), eqMidFreqArea.getHeight());
    eqMidFreqSliderLabel.setBounds(eqMidFreqArea.getX() - 20, eqMidFreqArea.getY() + eqMidFreqArea.getHeight() - 25, eqMidFreqArea.getWidth(), 10);
    eqMidFreqSliderLabel.setText("Mid Freq", juce::dontSendNotification);
    eqMidFreqSliderLabel.setJustificationType (juce::Justification::centred);
    
    eqMidQSlider.setBounds(eqMidQArea.getX() - 20, eqMidQArea.getY() - 10, eqMidQArea.getWidth(), eqMidQArea.getHeight());
    eqMidQSliderLabel.setBounds(eqMidQArea.getX() - 20, eqMidQArea.getY() + eqMidQArea.getHeight() - 25, eqMidQArea.getWidth(), 10);
    eqMidQSliderLabel.setText("Mid Q", juce::dontSendNotification);
    eqMidQSliderLabel.setJustificationType (juce::Justification::centred);
    
    eqMidGainSlider.setBounds(eqMidGainArea.getX() - 20, eqMidGainArea.getY() - 10, eqMidGainArea.getWidth(), eqMidGainArea.getHeight());
    eqMidGainSliderLabel.setBounds(eqMidGainArea.getX() - 20, eqMidGainArea.getY() + eqMidGainArea.getHeight() - 25, eqMidGainArea.getWidth(), 10);
    eqMidGainSliderLabel.setText("Mid Gain", juce::dontSendNotification);
    eqMidGainSliderLabel.setJustificationType (juce::Justification::centred);
    
    eqHighMidFreqSlider.setBounds(eqHighMidFreqArea.getX() - 20, eqHighMidFreqArea.getY() - 10, eqHighMidFreqArea.getWidth(), eqHighMidFreqArea.getHeight());
    eqHighMidFreqSliderLabel.setBounds(eqHighMidFreqArea.getX() - 20, eqHighMidFreqArea.getY() + eqHighMidFreqArea.getHeight() - 25, eqHighMidFreqArea.getWidth(), 10);
    eqHighMidFreqSliderLabel.setText("HighMid Freq", juce::dontSendNotification);
    eqHighMidFreqSliderLabel.setJustificationType (juce::Justification::centred);
    
    eqHighMidQSlider.setBounds(eqHighMidQArea.getX() - 20, eqHighMidQArea.getY() - 10, eqHighMidQArea.getWidth(), eqHighMidQArea.getHeight());
    eqHighMidQSliderLabel.setBounds(eqHighMidQArea.getX() - 20, eqHighMidQArea.getY() + eqHighMidQArea.getHeight() - 25, eqHighMidQArea.getWidth(), 10);
    eqHighMidQSliderLabel.setText("HighMid Q", juce::dontSendNotification);
    eqHighMidQSliderLabel.setJustificationType (juce::Justification::centred);
    
    eqHighMidGainSlider.setBounds(eqHighMidGainArea.getX() - 20, eqHighMidGainArea.getY() - 10, eqHighMidGainArea.getWidth(), eqHighMidGainArea.getHeight());
    eqHighMidGainSliderLabel.setBounds(eqHighMidGainArea.getX() - 20, eqHighMidGainArea.getY() + eqHighMidGainArea.getHeight() - 25, eqHighMidGainArea.getWidth(), 10);
    eqHighMidGainSliderLabel.setText("HighMid Gain", juce::dontSendNotification);
    eqHighMidGainSliderLabel.setJustificationType (juce::Justification::centred);
    
    eqHighPassFreqSlider.setBounds(eqHighFreqArea.getX() - 20, eqHighFreqArea.getY() - 10, eqHighFreqArea.getWidth(), eqHighFreqArea.getHeight());
    eqHighPassFreqSliderLabel.setBounds(eqHighFreqArea.getX() - 20, eqHighFreqArea.getY() + eqHighFreqArea.getHeight() - 25, eqHighFreqArea.getWidth(), 10);
    eqHighPassFreqSliderLabel.setText("High Freq", juce::dontSendNotification);
    eqHighPassFreqSliderLabel.setJustificationType (juce::Justification::centred);
    
    eqHighPassQSlider.setBounds(eqHighQArea.getX() - 20, eqHighQArea.getY() - 10, eqHighQArea.getWidth(), eqHighQArea.getHeight());
    eqHighPassQSliderLabel.setBounds(eqHighQArea.getX() - 20, eqHighQArea.getY() + eqHighQArea.getHeight() - 25, eqHighQArea.getWidth(), 10);
    eqHighPassQSliderLabel.setText("High Q", juce::dontSendNotification);
    eqHighPassQSliderLabel.setJustificationType (juce::Justification::centred);
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

void ProcessorGraphTestAudioProcessorEditor::drawDelay()
{
    auto bounds = getLocalBounds();
   auto responseArea = bounds.removeFromBottom(bounds.getHeight() * 0.5);
   
   auto labelArea = bounds.removeFromTop(bounds.getHeight() * .33);
   effectTitleLabel.setBounds(labelArea.getX() + 10, labelArea.getY() + 10, labelArea.getWidth() - 20, labelArea.getHeight() - 20);
   effectTitleLabel.setFont(juce::Font(32.f, juce::Font::bold));
   effectTitleLabel.setText("OUR DELAY", juce::dontSendNotification);
   effectTitleLabel.setColour (juce::Label::textColourId, juce::Colours::white);
   effectTitleLabel.setJustificationType (juce::Justification::left);
       
   auto delayTimeArea = bounds.removeFromLeft(bounds.getWidth() * 0.5);
   auto delayBlendArea = bounds.removeFromLeft(bounds.getWidth());
   
   delayTimeSlider.setBounds(delayTimeArea.getX() + 20, delayTimeArea.getY() + 20, delayTimeArea.getWidth() - 40, delayTimeArea.getHeight() - 40);
   delayTimeSliderLabel.setBounds(delayTimeArea.getX() + 20, delayTimeArea.getY() + delayTimeArea.getHeight() - 40, delayTimeArea.getWidth() - 40, 20);
    delayTimeSliderLabel.setText("Time", juce::dontSendNotification);
    delayTimeSliderLabel.setJustificationType (juce::Justification::centred);
   
   delayBlendSlider.setBounds(delayBlendArea.getX() + 20, delayBlendArea.getY() + 20, delayBlendArea.getWidth() - 40, delayBlendArea.getHeight() - 40);
    delayBlendSliderLabel.setBounds(delayBlendArea.getX() + 20, delayBlendArea.getY() + delayBlendArea.getHeight() - 40, delayBlendArea.getWidth() - 40, 20);
    delayBlendSliderLabel.setText("Blend", juce::dontSendNotification);
    delayBlendSliderLabel.setJustificationType (juce::Justification::centred);
    
}

void ProcessorGraphTestAudioProcessorEditor::drawAmp()
{
    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromBottom(bounds.getHeight() * 0.5);

    auto labelArea = bounds.removeFromTop(bounds.getHeight() * .33);
    effectTitleLabel.setBounds(labelArea.getX() + 10, labelArea.getY() + 10, labelArea.getWidth() - 20, labelArea.getHeight() - 20);
    effectTitleLabel.setFont(juce::Font(32.f, juce::Font::bold));
    effectTitleLabel.setText("OUR AMP", juce::dontSendNotification);
    effectTitleLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    effectTitleLabel.setJustificationType (juce::Justification::left);
    
//    ampTypeComboBox.setBounds(bounds.getX() - 20, bounds.getY() + (bounds.getHeight() / 4), bounds.getWidth(), bounds.getHeight() / 4);
    ampTypeComboBox.setBounds(bounds.getX() + 50, bounds.getY() + (bounds.getHeight() / 4), bounds.getWidth() - 100, bounds.getHeight() / 4);
    juce::StringArray strArray ( {"TanhWaveshaping", "AnalogAmp", "SGAmp" });
    for (int i = 0; i < strArray.size(); i++) {
        ampTypeComboBox.addItem(strArray[i], i+1);
    }
    
    
}

void ProcessorGraphTestAudioProcessorEditor::updateToggleState(juce::Button* button)
{
    bool showGate = false;
    bool showAmp = false;
    bool showVerb = false;
    bool showComp = false;
    bool showEq = false;
    bool showPhaser = false;
    bool showDelay = false;
    
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
    if (button == &delayButton)
    {
        showDelay = true;
        for (auto* comp : getDelayComps())
        {
            addAndMakeVisible(comp);
        }
        
        effectTitleLabel.setText("OUR DELAY", juce::dontSendNotification);
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
    for (auto* comp : getDelayComps())
    {
        comp->setVisible(showDelay);
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
        &delayButton,
        &inputGainSlider,
        &outputGainSlider,
        &gateBypassToggle,
        &compBypassToggle,
        &verbBypassToggle,
        &eqBypassToggle,
        &phaserBypassToggle,
        &delayBypassToggle
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
        &ampTypeComboBox
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
        &eqLowPassFreqSlider,
        &eqLowPassQSlider,
        &eqHighPassFreqSlider,
        &eqHighPassQSlider,
        &eqLowMidFreqSlider,
        &eqLowMidQSlider,
        &eqLowMidGainSlider,
        &eqMidFreqSlider,
        &eqMidQSlider,
        &eqMidGainSlider,
        &eqHighMidFreqSlider,
        &eqHighMidQSlider,
        &eqHighMidGainSlider,
        &eqLowPassFreqSliderLabel,
        &eqLowPassQSliderLabel,
        &eqHighPassFreqSliderLabel,
        &eqHighPassQSliderLabel,
        &eqLowMidFreqSliderLabel,
        &eqLowMidQSliderLabel,
        &eqLowMidGainSliderLabel,
        &eqMidFreqSliderLabel,
        &eqMidQSliderLabel,
        &eqMidGainSliderLabel,
        &eqHighMidFreqSliderLabel,
        &eqHighMidQSliderLabel,
        &eqHighMidGainSliderLabel
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

std::vector<juce::Component*> ProcessorGraphTestAudioProcessorEditor::getDelayComps()
{
    return
    {
        &delayTimeSlider,
        &delayBlendSlider,
        &delayTimeSliderLabel,
        &delayBlendSliderLabel
    };
}


