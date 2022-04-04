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
        
        
        text = "test";
        strWidth = g.getCurrentFont().getStringWidth(text);
        
        r.setSize(strWidth + 4, customSlider->getTextHeight() + 2);
        r.setCentre(bounds.getCentreX(), bounds.getY() + bounds.getHeight() + 30);
        
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
gateReleaseSliderAttachment(audioProcessor.apTreeState, "gateRelease", gateReleaseSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    for (auto* comp : getComps())
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
//    g.setColour(juce::Colours::darkgrey);
    g.fillAll ();
    g.setColour(juce::Colour(109u, 103u, 95u));
//    g.setColour(juce::Colours::lightgrey);
    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromBottom(bounds.getHeight() * 0.5);
    g.fillRect(bounds.getX() + 10, bounds.getY() + 10, bounds.getWidth() - 20, bounds.getHeight() - 20);
    
}

void GuitarAmpGUIAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    drawCompressor();
    
}

void GuitarAmpGUIAudioProcessorEditor::drawCompressor()
{
    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromBottom(bounds.getHeight() * 0.5);
    
    auto labelArea = bounds.removeFromTop(bounds.getHeight() * .33);
    auto gateThresholdArea = bounds.removeFromLeft(bounds.getWidth() * 0.25);
    auto gateRatioArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto gateAttackArea = bounds.removeFromLeft(bounds.getWidth() * 0.5);
    auto gateReleaseArea = bounds.removeFromLeft(bounds.getWidth());
    
    gateThresholdSlider.setBounds(gateThresholdArea.getX() + 20, gateThresholdArea.getY() + 20, gateThresholdArea.getWidth() - 40, gateThresholdArea.getHeight() - 40);
    gateRatioSlider.setBounds(gateRatioArea.getX() + 20, gateRatioArea.getY() + 20, gateRatioArea.getWidth() - 40, gateRatioArea.getHeight() - 40);
    gateAttackSlider.setBounds(gateAttackArea.getX() + 20, gateAttackArea.getY() + 20, gateAttackArea.getWidth() - 40, gateAttackArea.getHeight() - 40);
    gateReleaseSlider.setBounds(gateReleaseArea.getX() + 20, gateReleaseArea.getY() + 20, gateReleaseArea.getWidth() - 40, gateReleaseArea.getHeight() - 40);
    
    
}

std::vector<juce::Component*> GuitarAmpGUIAudioProcessorEditor::getComps()
{
    return
    {
        &gateThresholdSlider,
        &gateRatioSlider,
        &gateAttackSlider,
        &gateReleaseSlider
    };
}
