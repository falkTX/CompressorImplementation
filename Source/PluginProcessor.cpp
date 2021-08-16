/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CompressorImplementationAudioProcessor::CompressorImplementationAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

CompressorImplementationAudioProcessor::~CompressorImplementationAudioProcessor()
{
}

//==============================================================================
const juce::String CompressorImplementationAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CompressorImplementationAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CompressorImplementationAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CompressorImplementationAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CompressorImplementationAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CompressorImplementationAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CompressorImplementationAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CompressorImplementationAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CompressorImplementationAudioProcessor::getProgramName (int index)
{
    return {};
}

void CompressorImplementationAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CompressorImplementationAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    comp.setSampleRate(sampleRate);
}

void CompressorImplementationAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CompressorImplementationAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CompressorImplementationAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...

    comp.processBuffer(buffer);
}

//==============================================================================
bool CompressorImplementationAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CompressorImplementationAudioProcessor::createEditor()
{
    return new CompressorImplementationAudioProcessorEditor (*this);
}

//==============================================================================
void CompressorImplementationAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CompressorImplementationAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CompressorImplementationAudioProcessor();
}

void CompressorImplementationAudioProcessor::updatePregain(float v) {
    comp.set_linearpregain(v);
}

void CompressorImplementationAudioProcessor::updateThresh(float v) {
    comp.set_linearthreshold(v);
}

void CompressorImplementationAudioProcessor::updatePostgain(float v) {
    comp.set_postgain(v);
}

void CompressorImplementationAudioProcessor::updateWet(float v) {
    comp.set_wetlevel(v);
}

void CompressorImplementationAudioProcessor::updatePreDelay(float v) {
    comp.set_delaybufsize(comp.getSampleRate(), v);
}

void CompressorImplementationAudioProcessor::updateRatio(float v) {
    comp.set_slope(1.0/v);
}

void CompressorImplementationAudioProcessor::updateKnee(float v) {
    comp.calculate_knee(v);
}

void CompressorImplementationAudioProcessor::updateAttack(float v) {
    comp.set_attack(comp.getSampleRate(), v);
}

void CompressorImplementationAudioProcessor::updateRelease(float v) {
    comp.set_release(comp.getSampleRate(), v);
}