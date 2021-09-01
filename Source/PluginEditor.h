/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"

//==============================================================================
/**
*/
class CompressorImplementationAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    CompressorImplementationAudioProcessorEditor (CompressorImplementationAudioProcessor&);
    ~CompressorImplementationAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CompressorImplementationAudioProcessor& audioProcessor;

    // Dials
    juce::Slider pregainDial;
    juce::Slider threshDial;
    juce::Slider kneeDial;
    juce::Slider ratioDial;
    juce::Slider attackDial;
    juce::Slider releaseDial;
    juce::Slider preDelayDial;
    juce::Slider postgainDial;
    juce::Slider wetDial;
    const int dialWidth = 100;
    
    // Labels
    juce::Label pregainLabel;
    juce::Label threshLabel;
    juce::Label kneeLabel;
    juce::Label ratioLabel;
    juce::Label attackLabel;
    juce::Label releaseLabel;
    juce::Label preDelayLabel;
    juce::Label postgainLabel;
    juce::Label wetLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CompressorImplementationAudioProcessorEditor)
};
