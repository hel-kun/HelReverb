/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class HelReverbAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    HelReverbAudioProcessor();
    ~HelReverbAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    //bool supportsDoublePrecisionProcessing() const override;

    template<typename T>
    void processBlock(juce::AudioBuffer<T>&, juce::MidiBuffer&);
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    //void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    juce::AudioProcessorValueTreeState parameters;

    std::atomic<float>* gain = nullptr;
    std::atomic<float>* panRule = nullptr;
    std::atomic<float>* panAngle = nullptr;
    std::atomic<float>* bypass = nullptr;
    std::atomic<float>* reverbSize = nullptr;
    std::atomic<float>* reverbDamping = nullptr;
    std::atomic<float>* reverbWet = nullptr;
    std::atomic<float>* reverbWidth = nullptr;

    // Distortion
    std::atomic<float>* distortion = nullptr;

    juce::dsp::Gain<float> gainDSP;
    juce::dsp::Panner<float> pannerDSP;
    juce::dsp::Reverb reverbDSP;
    juce::dsp::Reverb::Parameters reverbParams;

    juce::dsp::ProcessSpec spec;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HelReverbAudioProcessor)
};
