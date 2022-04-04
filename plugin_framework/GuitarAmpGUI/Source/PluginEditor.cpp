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
ampTypeComboBoxAttachment(audioProcessor.apTreeState, "ampType", ampTypeComboBox)

{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
//    for (auto* comp : getNoiseGateComps())
//    {
//        addAndMakeVisible(comp);
//    }
    
    for (auto* comp : getAmpComps())
    {
        addAndMakeVisible(comp);
    }
    
    setSize (700, 500);
}

GuitarAmpGUIAudioProcessorEditor::~GuitarAmpGUIAudioProcessorEditor()
{
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
    
}

void GuitarAmpGUIAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
//    drawCompressor();
    drawAmp();
    
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
    
//    auto ampDriveArea = bounds.removeFromLeft(bounds.getWidth() * 0.25);
//    auto ampBlendArea = ampDriveArea.removeFromTop(bounds.getHeight() * 0.5);
//    auto ampPreLpfArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
//    auto ampPreHpfArea = ampPreLpfArea.removeFromTop(bounds.getHeight() * 0.5);
//    auto ampEmphasisFreqArea = bounds.removeFromLeft(bounds.getWidth() * 0.5);
//    auto ampEmphasisGainArea = ampEmphasisFreqArea.removeFromTop(bounds.getHeight() * 0.5);
//    auto ampPostLpfArea = bounds.removeFromLeft(bounds.getWidth());
//    auto ampPostHpfArea = ampPostLpfArea.removeFromTop(bounds.getHeight() * 0.5);
    
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
