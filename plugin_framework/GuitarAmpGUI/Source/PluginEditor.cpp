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

void LookAndFeel::drawToggleButton (juce::Graphics &g,
                       juce::ToggleButton &toggle,
                       bool shouldDrawButtonAsHighlighted,
                       bool shouldDrawButtonAsDown)
{
    using namespace juce;
    
    auto bounds = toggle.getLocalBounds();
    bounds.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth() - bounds.getWidth() / 4, bounds.getHeight());
    
    bool toggleState = toggle.getToggleState();
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
GuitarAmpGUIAudioProcessorEditor::GuitarAmpGUIAudioProcessorEditor (GuitarAmpGUIAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
gateThresholdSlider(*audioProcessor.apTreeState.getParameter("gateThreshold"), "dB"),
gateRatioSlider(*audioProcessor.apTreeState.getParameter("gateRatio"), ""),
gateAttackSlider(*audioProcessor.apTreeState.getParameter("gateAttack"), "s"),
gateReleaseSlider(*audioProcessor.apTreeState.getParameter("gateRelease"), "s"),

gateThresholdSliderAttachment(audioProcessor.apTreeState, "gateThreshold", gateThresholdSlider),
gateRatioSliderAttachment(audioProcessor.apTreeState, "gateRatio", gateRatioSlider),
gateAttackSliderAttachment(audioProcessor.apTreeState, "gateAttack", gateAttackSlider),
gateReleaseSliderAttachment(audioProcessor.apTreeState, "gateRelease", gateReleaseSlider),

ampDriveSlider(*audioProcessor.apTreeState.getParameter("ampDrive"), ""),
ampBlendSlider(*audioProcessor.apTreeState.getParameter("ampBlend"), "%"),
ampPreLpfSlider(*audioProcessor.apTreeState.getParameter("ampPreLPF"), "Hz"),
ampPreHpfSlider(*audioProcessor.apTreeState.getParameter("ampPreHPF"), "Hz"),
ampEmphasisFreqSlider(*audioProcessor.apTreeState.getParameter("ampEmphasis"), "Hz"),
ampEmphasisGainSlider(*audioProcessor.apTreeState.getParameter("ampEmphasisGain"), "dB"),
ampPostLpfSlider(*audioProcessor.apTreeState.getParameter("ampPostLPF"), "Hz"),
ampPostHpfSlider(*audioProcessor.apTreeState.getParameter("ampPostHPF"), "Hz"),

ampDriveSliderAttachment(audioProcessor.apTreeState, "ampDrive", ampDriveSlider),
ampBlendSliderAttachment(audioProcessor.apTreeState, "ampBlend", ampBlendSlider),
ampPreLpfSliderAttachment(audioProcessor.apTreeState, "ampPreLPF", ampPreLpfSlider),
ampPreHpfSliderAttachment(audioProcessor.apTreeState, "ampPreHPF", ampPreHpfSlider),
ampEmphasisFreqSliderAttachment(audioProcessor.apTreeState, "ampEmphasis", ampEmphasisFreqSlider),
ampEmphasisGainSliderAttachment(audioProcessor.apTreeState, "ampEmphasisGain", ampEmphasisGainSlider),
ampPostLpfSliderAttachment(audioProcessor.apTreeState, "ampPostLPF", ampPostLpfSlider),
ampPostHpfSliderAttachment(audioProcessor.apTreeState, "ampPostHPF", ampPostHpfSlider),

ampTypeComboBox("ampType"),
ampTypeComboBoxAttachment(audioProcessor.apTreeState, "ampType", ampTypeComboBox),

rvbBlendSlider(*audioProcessor.apTreeState.getParameter("rvbBlend"), "%"),
rvbRoomSizeSlider(*audioProcessor.apTreeState.getParameter("rvbRoomSize"), ""),
rvbDampingSlider(*audioProcessor.apTreeState.getParameter("rvbDamping"), ""),

rvbBlendSliderAttachment(audioProcessor.apTreeState, "rvbBlend", rvbBlendSlider),
rvbRoomSizeSliderAttachment(audioProcessor.apTreeState, "rvbRoomSize", rvbRoomSizeSlider),
rvbDampingSliderAttachment(audioProcessor.apTreeState, "rvbDamping", rvbDampingSlider),

compThresholdSlider(*audioProcessor.apTreeState.getParameter("compThreshold"), "dB"),
compRatioSlider(*audioProcessor.apTreeState.getParameter("compRatio"), ""),
compAttackSlider(*audioProcessor.apTreeState.getParameter("compAttack"), "ms"),
compReleaseSlider(*audioProcessor.apTreeState.getParameter("compRelease"), "ms"),
compMakeUpGainSlider(*audioProcessor.apTreeState.getParameter("compMakeupGain"), "dB"),

compThresholdSliderAttachment(audioProcessor.apTreeState, "compThreshold", compThresholdSlider),
compRatioSliderAttachment(audioProcessor.apTreeState, "compRatio", compRatioSlider),
compAttackSliderAttachment(audioProcessor.apTreeState, "compAttack", compAttackSlider),
compReleaseSliderAttachment(audioProcessor.apTreeState, "compRelease", compReleaseSlider),
compMakeUpGainSliderAttachment(audioProcessor.apTreeState, "compMakeupGain", compMakeUpGainSlider),

eqLowCutFreqSlider(*audioProcessor.apTreeState.getParameter("LowCutFreq"), "Hz"),
eqLowCutSlopeSlider(*audioProcessor.apTreeState.getParameter("LowCutSlope"), ""),
eqHighCutFreqSlider(*audioProcessor.apTreeState.getParameter("HighCutFreq"), "Hz"),
eqHighCutSlopeSlider(*audioProcessor.apTreeState.getParameter("HighCutSlope"), ""),
eqPeakFreqSlider(*audioProcessor.apTreeState.getParameter("PeakFreq"), "Hz"),
eqPeakGainSlider(*audioProcessor.apTreeState.getParameter("PeakGain"), "dB"),
eqPeakQSlider(*audioProcessor.apTreeState.getParameter("PeakQ"), ""),

eqLowCutFreqSliderAttachment(audioProcessor.apTreeState, "LowCutFreq", eqLowCutFreqSlider),
eqLowCutSlopeSliderAttachment(audioProcessor.apTreeState, "LowCutSlope", eqLowCutSlopeSlider),
eqHighCutFreqSliderAttachment(audioProcessor.apTreeState, "HighCutFreq", eqHighCutFreqSlider),
eqHighCutSlopeSliderAttachment(audioProcessor.apTreeState, "HighCutSlope", eqHighCutSlopeSlider),
eqPeakFreqSliderAttachment(audioProcessor.apTreeState, "PeakFreq", eqPeakFreqSlider),
eqPeakGainSliderAttachment(audioProcessor.apTreeState, "PeakGain", eqPeakGainSlider),
eqPeakQSliderAttachment(audioProcessor.apTreeState, "PeakQ", eqPeakQSlider),

phaserRateSlider(*audioProcessor.apTreeState.getParameter("phaserRate"), "Hz"),
phaserDepthSlider(*audioProcessor.apTreeState.getParameter("phaserDepth"), ""),
phaserFcSlider(*audioProcessor.apTreeState.getParameter("phaserFc"), "Hz"),
phaserFeedbackSlider(*audioProcessor.apTreeState.getParameter("phaserFeedback"), "%"),
phaserBlendSlider(*audioProcessor.apTreeState.getParameter("phaserBlend"), "%"),

phaserRateSliderAttachment(audioProcessor.apTreeState, "phaserRate", phaserRateSlider),
phaserDepthSliderAttachment(audioProcessor.apTreeState, "phaserDepth", phaserDepthSlider),
phaserFcSliderAttachment(audioProcessor.apTreeState, "phaserFc", phaserFcSlider),
phaserFeedbackSliderAttachment(audioProcessor.apTreeState, "phaserFeedback", phaserFeedbackSlider),
phaserBlendSliderAttachment(audioProcessor.apTreeState, "phaserBlend", phaserBlendSlider),

gateButton("GATE"),
ampButton("AMP"),
verbButton("REVERB"),
compressorButton("COMP"),
eqButton("EQ"),
phaserButton("PHASER")

{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    
    for (auto* comp : getAmpComps())
    {
        addAndMakeVisible(comp);
    }
    effectTitleLabel.setText("OUR AMP", juce::dontSendNotification);
    
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
    
    ampButton.setState(juce::Button::ButtonState::buttonDown);
    
    
//    addAndMakeVisible (switchEffectButton);
//    switchEffectButton.setButtonText ("Switch Effect");
//    switchEffectButton.addListener(this);
    
    setSize (700, 500);
}

GuitarAmpGUIAudioProcessorEditor::~GuitarAmpGUIAudioProcessorEditor()
{
    for (auto* comp : getChainComps())
    {
        comp -> setLookAndFeel(nullptr);
    }
}

//==============================================================================
void GuitarAmpGUIAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
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

void GuitarAmpGUIAudioProcessorEditor::resized()
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
    
    
    drawNoiseGate();
    drawReverb();
    drawCompressor();
    drawEQ();
    drawPhaser();
    drawAmp();
    
}

void GuitarAmpGUIAudioProcessorEditor::drawNoiseGate()
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

void GuitarAmpGUIAudioProcessorEditor::drawAmp()
{
    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromBottom(bounds.getHeight() * 0.5);
    
    auto labelArea = bounds.removeFromTop(bounds.getHeight() * .33);
    effectTitleLabel.setBounds(labelArea.getX() + 10, labelArea.getY() + 10, labelArea.getWidth() - 20, labelArea.getHeight() - 20);
    effectTitleLabel.setFont(juce::Font(32.f, juce::Font::bold));
    effectTitleLabel.setText("OUR AMP", juce::dontSendNotification);
    effectTitleLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    effectTitleLabel.setJustificationType (juce::Justification::left);
    
    auto ampDriveArea = bounds.removeFromLeft(bounds.getWidth() * 0.2);
    auto ampBlendArea = ampDriveArea.removeFromTop(bounds.getHeight() * 0.5);
    auto ampPreLpfArea = bounds.removeFromLeft(bounds.getWidth() * 0.25);
    auto ampPreHpfArea = ampPreLpfArea.removeFromTop(bounds.getHeight() * 0.5);
    auto ampEmphasisFreqArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto ampEmphasisGainArea = ampEmphasisFreqArea.removeFromTop(bounds.getHeight() * 0.5);
    auto ampPostLpfArea = bounds.removeFromLeft(bounds.getWidth() * 0.5);
    auto ampPostHpfArea = ampPostLpfArea.removeFromTop(bounds.getHeight() * 0.5);
    
    
    ampDriveSlider.setBounds(ampDriveArea.getX(), ampDriveArea.getY() - 10, ampDriveArea.getWidth(), ampDriveArea.getHeight());
    ampDriveSliderLabel.setBounds(ampDriveArea.getX(), ampDriveArea.getY() + ampDriveArea.getHeight() - 25, ampDriveArea.getWidth(), 10);
    ampDriveSliderLabel.setText("Drive", juce::dontSendNotification);
    ampDriveSliderLabel.setJustificationType (juce::Justification::centred);
    
    ampBlendSlider.setBounds(ampBlendArea.getX(), ampBlendArea.getY() - 10, ampBlendArea.getWidth(), ampBlendArea.getHeight());
    ampBlendSliderLabel.setBounds(ampBlendArea.getX(), ampBlendArea.getY() + ampBlendArea.getHeight() - 25, ampBlendArea.getWidth(), 10);
    ampBlendSliderLabel.setText("Blend", juce::dontSendNotification);
    ampBlendSliderLabel.setJustificationType (juce::Justification::centred);
    
    ampPreLpfSlider.setBounds(ampPreLpfArea.getX() - 20, ampPreLpfArea.getY() - 10, ampPreLpfArea.getWidth(), ampPreLpfArea.getHeight());
    ampPreLpfSliderLabel.setBounds(ampPreLpfArea.getX() - 20, ampPreLpfArea.getY() + ampPreLpfArea.getHeight() - 25, ampPreLpfArea.getWidth(), 10);
    ampPreLpfSliderLabel.setText("Pre-LPF", juce::dontSendNotification);
    ampPreLpfSliderLabel.setJustificationType (juce::Justification::centred);
    
    ampPreHpfSlider.setBounds(ampPreHpfArea.getX() - 20, ampPreHpfArea.getY() - 10, ampPreHpfArea.getWidth(), ampPreHpfArea.getHeight());
    ampPreHpfSliderLabel.setBounds(ampPreHpfArea.getX() - 20, ampPreHpfArea.getY() + ampPreHpfArea.getHeight() - 25, ampPreHpfArea.getWidth(), 10);
    ampPreHpfSliderLabel.setText("Pre-HPF", juce::dontSendNotification);
    ampPreHpfSliderLabel.setJustificationType (juce::Justification::centred);
    
    ampEmphasisFreqSlider.setBounds(ampEmphasisFreqArea.getX() - 20, ampEmphasisFreqArea.getY() - 10, ampEmphasisFreqArea.getWidth(), ampEmphasisFreqArea.getHeight());
    ampEmphasisFreqSliderLabel.setBounds(ampEmphasisFreqArea.getX() - 20, ampEmphasisFreqArea.getY() + ampEmphasisFreqArea.getHeight() - 25, ampEmphasisFreqArea.getWidth(), 10);
    ampEmphasisFreqSliderLabel.setText("Emphasis Freq", juce::dontSendNotification);
    ampEmphasisFreqSliderLabel.setJustificationType (juce::Justification::centred);
    
    ampEmphasisGainSlider.setBounds(ampEmphasisGainArea.getX() - 20, ampEmphasisGainArea.getY() - 10, ampEmphasisGainArea.getWidth(), ampEmphasisGainArea.getHeight());
    ampEmphasisGainSliderLabel.setBounds(ampEmphasisGainArea.getX() - 20, ampEmphasisGainArea.getY() + ampEmphasisGainArea.getHeight() - 25, ampEmphasisGainArea.getWidth(), 10);
    ampEmphasisGainSliderLabel.setText("Emphasis Gain", juce::dontSendNotification);
    ampEmphasisGainSliderLabel.setJustificationType (juce::Justification::centred);
    
    ampPostLpfSlider.setBounds(ampPostLpfArea.getX() - 20, ampPostLpfArea.getY() - 10, ampPostLpfArea.getWidth(), ampPostLpfArea.getHeight());
    ampPostLpfSliderLabel.setBounds(ampPostLpfArea.getX() - 20, ampPostLpfArea.getY() + ampPostLpfArea.getHeight() - 25, ampPostLpfArea.getWidth(), 10);
    ampPostLpfSliderLabel.setText("Post-LPF", juce::dontSendNotification);
    ampPostLpfSliderLabel.setJustificationType (juce::Justification::centred);
    
    ampPostHpfSlider.setBounds(ampPostHpfArea.getX() - 20, ampPostHpfArea.getY() - 10, ampPostHpfArea.getWidth(), ampPostHpfArea.getHeight());
    ampPostHpfSliderLabel.setBounds(ampPostHpfArea.getX() - 20, ampPostHpfArea.getY() + ampPostHpfArea.getHeight() - 25, ampPostHpfArea.getWidth(), 10);
    ampPostHpfSliderLabel.setText("Post-HPF", juce::dontSendNotification);
    ampPostHpfSliderLabel.setJustificationType (juce::Justification::centred);
    
    ampTypeComboBox.setBounds(bounds.getX() - 20, bounds.getY() + (bounds.getHeight() / 4), bounds.getWidth(), bounds.getHeight() / 4);
    juce::StringArray strArray ( {"None", "Tanh", "ATan", "Hard Clipper", "Rectifier", "Sine", "Tube"});
    for (int i = 0; i < strArray.size(); i++) {
        ampTypeComboBox.addItem(strArray[i], i+1);
    }

}

void GuitarAmpGUIAudioProcessorEditor::drawReverb()
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

void GuitarAmpGUIAudioProcessorEditor::drawCompressor()
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

void GuitarAmpGUIAudioProcessorEditor::drawEQ()
{
    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromBottom(bounds.getHeight() * 0.5);
    
    auto labelArea = bounds.removeFromTop(bounds.getHeight() * .33);
    effectTitleLabel.setBounds(labelArea.getX() + 10, labelArea.getY() + 10, labelArea.getWidth() - 20, labelArea.getHeight() - 20);
    effectTitleLabel.setFont(juce::Font(32.f, juce::Font::bold));
    effectTitleLabel.setText("OUR EQ", juce::dontSendNotification);
    effectTitleLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    effectTitleLabel.setJustificationType (juce::Justification::left);
    
    auto eqHighCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto eqHighCutSlopeArea = eqHighCutArea.removeFromTop(eqHighCutArea.getHeight() * 0.5);
    auto eqHighCutFreqArea = eqHighCutArea;
    auto eqPeakArea = bounds.removeFromLeft(bounds.getWidth() * 0.5);
    auto eqPeakFreqArea = eqPeakArea.removeFromTop(eqPeakArea.getHeight() * 0.5);
    auto eqPeakGainArea = eqPeakArea.removeFromLeft(eqPeakArea.getWidth() * 0.5);
    auto eqPeakQArea = eqPeakArea;
    auto eqLowCutArea = bounds.removeFromLeft(bounds.getWidth());
    auto eqLowCutSlopeArea = eqLowCutArea.removeFromTop(eqLowCutArea.getHeight() * 0.5);
    auto eqLowCutFreqArea = eqLowCutArea;
    
    eqHighCutSlopeSlider.setBounds(eqHighCutSlopeArea.getX() - 20, eqHighCutSlopeArea.getY() - 10, eqHighCutSlopeArea.getWidth(), eqHighCutSlopeArea.getHeight());
    eqHighCutSlopeSliderLabel.setBounds(eqHighCutSlopeArea.getX() - 20, eqHighCutSlopeArea.getY() + eqHighCutSlopeArea.getHeight() - 25, eqHighCutSlopeArea.getWidth(), 10);
    eqHighCutSlopeSliderLabel.setText("High Cut Slope", juce::dontSendNotification);
    eqHighCutSlopeSliderLabel.setJustificationType (juce::Justification::centred);
    
    eqHighCutFreqSlider.setBounds(eqHighCutFreqArea.getX() - 20, eqHighCutFreqArea.getY() - 10, eqHighCutFreqArea.getWidth(), eqHighCutFreqArea.getHeight());
    eqHighCutFreqSliderLabel.setBounds(eqHighCutFreqArea.getX() - 20, eqHighCutFreqArea.getY() + eqHighCutFreqArea.getHeight() - 25, eqHighCutFreqArea.getWidth(), 10);
    eqHighCutFreqSliderLabel.setText("High Cut Freq", juce::dontSendNotification);
    eqHighCutFreqSliderLabel.setJustificationType (juce::Justification::centred);
    
    eqPeakFreqSlider.setBounds(eqPeakFreqArea.getX() - 20, eqPeakFreqArea.getY() - 10, eqPeakFreqArea.getWidth(), eqPeakFreqArea.getHeight());
    eqPeakFreqSliderLabel.setBounds(eqPeakFreqArea.getX() - 20, eqPeakFreqArea.getY() + eqPeakFreqArea.getHeight() - 25, eqPeakFreqArea.getWidth(), 10);
    eqPeakFreqSliderLabel.setText("Peak Freq", juce::dontSendNotification);
    eqPeakFreqSliderLabel.setJustificationType (juce::Justification::centred);
    
    eqPeakGainSlider.setBounds(eqPeakGainArea.getX() - 20, eqPeakGainArea.getY() - 10, eqPeakGainArea.getWidth(), eqPeakGainArea.getHeight());
    eqPeakGainSliderLabel.setBounds(eqPeakGainArea.getX() - 20, eqPeakGainArea.getY() + eqPeakGainArea.getHeight() - 25, eqPeakGainArea.getWidth(), 10);
    eqPeakGainSliderLabel.setText("Peak Gain", juce::dontSendNotification);
    eqPeakGainSliderLabel.setJustificationType (juce::Justification::centred);
    
    eqPeakQSlider.setBounds(eqPeakQArea.getX() - 20, eqPeakQArea.getY() - 10, eqPeakQArea.getWidth(), eqPeakQArea.getHeight());
    eqPeakQSliderLabel.setBounds(eqPeakQArea.getX() - 20, eqPeakQArea.getY() + eqPeakQArea.getHeight() - 25, eqPeakQArea.getWidth(), 10);
    eqPeakQSliderLabel.setText("Peak Q", juce::dontSendNotification);
    eqPeakQSliderLabel.setJustificationType (juce::Justification::centred);
    
    eqLowCutSlopeSlider.setBounds(eqLowCutSlopeArea.getX() - 20, eqLowCutSlopeArea.getY() - 10, eqLowCutSlopeArea.getWidth(), eqLowCutSlopeArea.getHeight());
    eqLowCutSlopeSliderLabel.setBounds(eqLowCutSlopeArea.getX() - 20, eqLowCutSlopeArea.getY() + eqLowCutSlopeArea.getHeight() - 25, eqLowCutSlopeArea.getWidth(), 10);
    eqLowCutSlopeSliderLabel.setText("Low Cut Slope", juce::dontSendNotification);
    eqLowCutSlopeSliderLabel.setJustificationType (juce::Justification::centred);
    
    eqLowCutFreqSlider.setBounds(eqLowCutFreqArea.getX() - 20, eqLowCutFreqArea.getY() - 10, eqLowCutFreqArea.getWidth(), eqLowCutFreqArea.getHeight());
    eqLowCutFreqSliderLabel.setBounds(eqLowCutFreqArea.getX() - 20, eqLowCutFreqArea.getY() + eqLowCutFreqArea.getHeight() - 25, eqLowCutFreqArea.getWidth(), 10);
    eqLowCutFreqSliderLabel.setText("Low Cut Freq", juce::dontSendNotification);
    eqLowCutFreqSliderLabel.setJustificationType (juce::Justification::centred);

}

void GuitarAmpGUIAudioProcessorEditor::drawPhaser()
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

void GuitarAmpGUIAudioProcessorEditor::buttonClicked (juce::Button* button)
{
    if (button == &switchEffectButton)
    {
        for (auto* comp : getReverbComps())
        {
            drawReverb();
            addAndMakeVisible(comp);
        }
        for (auto* comp : getCompressorComps())
        {
            comp->setVisible(false);
        }
        effectTitleLabel.setVisible(true);
    }
}

void GuitarAmpGUIAudioProcessorEditor::updateToggleState(juce::Button* button)
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

std::vector<juce::Component*> GuitarAmpGUIAudioProcessorEditor::getChainComps()
{
    return
    {
        &gateButton,
        &ampButton,
        &verbButton,
        &compressorButton,
        &eqButton,
        &phaserButton
    };
}

std::vector<juce::Component*> GuitarAmpGUIAudioProcessorEditor::getNoiseGateComps()
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

std::vector<juce::Component*> GuitarAmpGUIAudioProcessorEditor::getAmpComps()
{
    return
    {
        &ampDriveSlider,
        &ampBlendSlider,
        &ampPreLpfSlider,
        &ampPreHpfSlider,
        &ampEmphasisFreqSlider,
        &ampEmphasisGainSlider,
        &ampPostLpfSlider,
        &ampPostHpfSlider,
        &effectTitleLabel,
        &ampDriveSliderLabel,
        &ampBlendSliderLabel,
        &ampPreLpfSliderLabel,
        &ampPreHpfSliderLabel,
        &ampEmphasisFreqSliderLabel,
        &ampEmphasisGainSliderLabel,
        &ampPostLpfSliderLabel,
        &ampPostHpfSliderLabel,
        &ampTypeComboBox
    };
}

std::vector<juce::Component*> GuitarAmpGUIAudioProcessorEditor::getReverbComps()
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

std::vector<juce::Component*> GuitarAmpGUIAudioProcessorEditor::getCompressorComps()
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

std::vector<juce::Component*> GuitarAmpGUIAudioProcessorEditor::getEqComps()
{
    return
    {
        &eqLowCutFreqSlider,
        &eqLowCutSlopeSlider,
        &eqHighCutFreqSlider,
        &eqHighCutSlopeSlider,
        &eqPeakFreqSlider,
        &eqPeakGainSlider,
        &eqPeakQSlider,
        &eqLowCutFreqSliderLabel,
        &eqLowCutSlopeSliderLabel,
        &eqHighCutFreqSliderLabel,
        &eqHighCutSlopeSliderLabel,
        &eqPeakFreqSliderLabel,
        &eqPeakGainSliderLabel,
        &eqPeakQSliderLabel
    };
}

std::vector<juce::Component*> GuitarAmpGUIAudioProcessorEditor::getPhaserComps()
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

