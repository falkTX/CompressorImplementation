/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CompressorImplementationAudioProcessorEditor::CompressorImplementationAudioProcessorEditor (CompressorImplementationAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 300);

    // make all visible
    addAndMakeVisible(pregainDial);
    addAndMakeVisible(threshDial);
    addAndMakeVisible(kneeDial);
    addAndMakeVisible(ratioDial);
    addAndMakeVisible(attackDial);
    addAndMakeVisible(releaseDial);
    addAndMakeVisible(preDelayDial);
    addAndMakeVisible(postgainDial);
    addAndMakeVisible(wetDial);

    addAndMakeVisible(pregainLabel);
    addAndMakeVisible(threshLabel);
    addAndMakeVisible(kneeLabel);
    addAndMakeVisible(ratioLabel);
    addAndMakeVisible(attackLabel);
    addAndMakeVisible(releaseLabel);
    addAndMakeVisible(preDelayLabel);
    addAndMakeVisible(postgainLabel);
    addAndMakeVisible(wetLabel);

    // label settings
    pregainLabel.setText("pre gain", juce::dontSendNotification);
    threshLabel.setText("threshold", juce::dontSendNotification);
    kneeLabel.setText("knee", juce::dontSendNotification);
    ratioLabel.setText("ratio", juce::dontSendNotification);
    attackLabel.setText("attack", juce::dontSendNotification);
    releaseLabel.setText("release", juce::dontSendNotification);
    preDelayLabel.setText("pre delay", juce::dontSendNotification);
    postgainLabel.setText("post gain", juce::dontSendNotification);
    wetLabel.setText("wet level", juce::dontSendNotification);

    pregainLabel.attachToComponent(&pregainDial, false);
    threshLabel.attachToComponent(&threshDial, false);
    kneeLabel.attachToComponent(&kneeDial, false);
    ratioLabel.attachToComponent(&ratioDial, false);
    attackLabel.attachToComponent(&attackDial, false);
    releaseLabel.attachToComponent(&releaseDial, false);
    preDelayLabel.attachToComponent(&preDelayDial, false);
    postgainLabel.attachToComponent(&postgainDial, false);
    wetLabel.attachToComponent(&wetDial, false);

    pregainLabel.setJustificationType(juce::Justification::centred);
    threshLabel.setJustificationType(juce::Justification::centred);
    kneeLabel.setJustificationType(juce::Justification::centred);
    ratioLabel.setJustificationType(juce::Justification::centred);
    attackLabel.setJustificationType(juce::Justification::centred);
    releaseLabel.setJustificationType(juce::Justification::centred);
    preDelayLabel.setJustificationType(juce::Justification::centred);
    postgainLabel.setJustificationType(juce::Justification::centred);
    wetLabel.setJustificationType(juce::Justification::centred);

    // dial settings
    pregainDial.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    threshDial.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    kneeDial.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    ratioDial.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    attackDial.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    releaseDial.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    preDelayDial.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    postgainDial.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    wetDial.setSliderStyle(juce::Slider::SliderStyle::Rotary);

    pregainDial.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 15);
    threshDial.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 15);
    kneeDial.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 15);
    ratioDial.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 15);
    attackDial.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 15);
    releaseDial.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 15);
    preDelayDial.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 15);
    postgainDial.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 15);
    wetDial.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 15);

    pregainDial.setRange(-60.0f, 10.0f, 0.01);
    threshDial.setRange(-60.0f, 0.0f, 0.01);
    kneeDial.setRange(0.0f, 60.0f, 0.01);
    ratioDial.setRange(2.0f, 20.0f, 0.001);
    attackDial.setRange(0.003f, 1.0f, 0.0001);
    releaseDial.setRange(0.05f, 3.0f, 0.001);
    preDelayDial.setRange(0.0f, 0.1f, 0.001);
    postgainDial.setRange(-60.0f, 10.0f, 0.01);
    wetDial.setRange(0.0f, 1.0f, 0.001);

    pregainDial.setValue(0.0f);
    threshDial.setValue(-12.0f);
    kneeDial.setValue(30.0f);
    ratioDial.setValue(12.0f);
    attackDial.setValue(0.003f);
    releaseDial.setValue(0.250f);
    preDelayDial.setValue(0.006f);
    postgainDial.setValue(0.0f);
    wetDial.setValue(1.0f);

    pregainDial.setSkewFactorFromMidPoint(0.0);
    postgainDial.setSkewFactorFromMidPoint(0.0);
    threshDial.setSkewFactorFromMidPoint(-24.0);
    ratioDial.setSkewFactorFromMidPoint(5.0);
    attackDial.setSkewFactorFromMidPoint(0.05);
    releaseDial.setSkewFactorFromMidPoint(0.5);

    pregainDial.addListener(this);
    threshDial.addListener(this);
    kneeDial.addListener(this);
    ratioDial.addListener(this);
    attackDial.addListener(this);
    releaseDial.addListener(this);
    preDelayDial.addListener(this);
    postgainDial.addListener(this);
    wetDial.addListener(this);
}

CompressorImplementationAudioProcessorEditor::~CompressorImplementationAudioProcessorEditor()
{
    pregainDial.removeListener(this);
    threshDial.removeListener(this);
    kneeDial.removeListener(this);
    ratioDial.removeListener(this);
    attackDial.removeListener(this);
    releaseDial.removeListener(this);
    preDelayDial.removeListener(this);
    postgainDial.removeListener(this);
    wetDial.removeListener(this);
}

//==============================================================================
void CompressorImplementationAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void CompressorImplementationAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto area = getLocalBounds();
    int aOffset = 10; int hOffset = 25;
    area.reduce(aOffset * 2, aOffset * 2);
    int widthSection = area.getWidth() / 5; int heightSection = area.getHeight() / 2;

    pregainDial.setBounds(widthSection * 0 + aOffset, heightSection * 0 + aOffset + hOffset, dialWidth, dialWidth);
    threshDial.setBounds(widthSection * 1 + aOffset, heightSection * 0 + aOffset + hOffset, dialWidth, dialWidth);
    postgainDial.setBounds(widthSection * 2 + aOffset, heightSection * 0 + aOffset + hOffset, dialWidth, dialWidth);
    wetDial.setBounds(widthSection * 4 + aOffset, heightSection * 0 + aOffset + hOffset, dialWidth, dialWidth);

    ratioDial.setBounds(widthSection * 0 + aOffset, heightSection * 1 + aOffset + hOffset, dialWidth, dialWidth);
    kneeDial.setBounds(widthSection * 1 + aOffset, heightSection * 1 + aOffset + hOffset, dialWidth, dialWidth);
    attackDial.setBounds(widthSection * 2 + aOffset, heightSection * 1 + aOffset + hOffset, dialWidth, dialWidth);
    releaseDial.setBounds(widthSection * 3 + aOffset, heightSection * 1 + aOffset + hOffset, dialWidth, dialWidth);
    preDelayDial.setBounds(widthSection * 4 + aOffset, heightSection * 1 + aOffset + hOffset, dialWidth, dialWidth);
}

void CompressorImplementationAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &pregainDial) {
        audioProcessor.updatePregain(pregainDial.getValue());
    }
    else if (slider == &threshDial) {
        audioProcessor.updateThresh(threshDial.getValue());
    }
    else if (slider == &postgainDial) {
        audioProcessor.updatePostgain(postgainDial.getValue());
    }
    else if (slider == &wetDial) {
        audioProcessor.updateWet(wetDial.getValue());
    }
    else if (slider == &preDelayDial) {
        audioProcessor.updatePreDelay(preDelayDial.getValue());
    }
    else if (slider == &ratioDial) {
        audioProcessor.updateRatio(ratioDial.getValue());
    }
    else if (slider == &kneeDial) {
        audioProcessor.updateKnee(kneeDial.getValue());
    }
    else if (slider == &attackDial) {
        audioProcessor.updateAttack(attackDial.getValue());
    }
    else if (slider == &releaseDial) {
        audioProcessor.updateRelease(releaseDial.getValue());
    }
}