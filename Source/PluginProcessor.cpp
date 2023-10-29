/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HelReverbAudioProcessor::HelReverbAudioProcessor()
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
    , parameters(*this, nullptr, juce::Identifier("tutorial"), // Ç±ÇÃ1çsñ⁄ÇÊÇ≠ÇÌÇ©ÇÁÇÒ
        {
            // Gain
            std::make_unique<juce::AudioParameterFloat>(
                "gain",
                "Gain",
                juce::NormalisableRange<float>(-100.0f, 10.0f, 0.01),
                0.0f),
            // Pan
            std::make_unique<juce::AudioParameterChoice>(
                "panrule","Pan Rule",
                juce::StringArray("linear","balanced","sin3dB","sin4p5dB","sin6dB","squareRoot3dB","squareRoot4p5dB"),
                1),
            std::make_unique<juce::AudioParameterFloat>(
                "panangle","Pan Angle",
                juce::NormalisableRange<float>(-100.0f,100.0f,1),
                0.0f),
            // Bypass
            std::make_unique<juce::AudioParameterBool>(
                "bypass",
                "My Bypass",
                false),
            // Reverb
            std::make_unique<juce::AudioParameterFloat>(
                "roomsize",
                "Reverb Size",
                juce::NormalisableRange<float>(0.0f,100.0f,1),
                50.0f),
            std::make_unique<juce::AudioParameterFloat>(
                "damping",
                "Reverb Damping",
                juce::NormalisableRange<float>(0.0f,100.0f,1),
                50.0f),
            std::make_unique<juce::AudioParameterFloat>(
                "wetlevel",
                "Reverb Wet",
                juce::NormalisableRange<float>(0.0f,100.0f,1),
                33.0f),
            std::make_unique<juce::AudioParameterFloat>(
                "width",
                "Reverb Width",
                juce::NormalisableRange<float>(0.0f,100.0f,1),
            100.0f),
            std::make_unique<juce::AudioParameterFloat>(
                "distortion",
                "Distortion",juce::NormalisableRange<float>(0.0f,100.0f,1),
            0.0f),
        })
{
    //setProcessingPrecision(juce::AudioProcessor::doublePrecision);

    gain = parameters.getRawParameterValue("gain");
    panAngle = parameters.getRawParameterValue("panangle");
    panRule = parameters.getRawParameterValue("panrule");
    bypass = parameters.getRawParameterValue("bypass");
    reverbSize = parameters.getRawParameterValue("roomsize");
    reverbDamping = parameters.getRawParameterValue("damping");
    reverbWet = parameters.getRawParameterValue("wetlevel");
    reverbWidth = parameters.getRawParameterValue("width");
    distortion = parameters.getRawParameterValue("distortion");
}

HelReverbAudioProcessor::~HelReverbAudioProcessor()
{
}

//==============================================================================
const juce::String HelReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool HelReverbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool HelReverbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool HelReverbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double HelReverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int HelReverbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int HelReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void HelReverbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String HelReverbAudioProcessor::getProgramName (int index)
{
    return {};
}

void HelReverbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void HelReverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;
    spec.sampleRate = sampleRate;

    gainDSP.prepare(spec);
    pannerDSP.prepare(spec);
    reverbDSP.prepare(spec);
}

void HelReverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HelReverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

/*bool HrlReverbAudioProcessor::supportsDoublePrecisionProcessing() const
{
    return true;
}*/

void HelReverbAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    processBlock<float>(buffer, midiMessages);
}

//void GainPantutorialAudioProcessor::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
//{
//    processBlock<double>(buffer, midiMessages);
//}

template<typename T>
void HelReverbAudioProcessor::processBlock (juce::AudioBuffer<T>& buffer, juce::MidiBuffer& midiMessages)
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
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
        // DistorsionÇÃèàóù
        // DistortionÇÃèàóùÇ±Ç±Ç‹Ç≈
    }
    // êMçÜèàóùÇ±Ç±Ç©ÇÁ
    gainDSP.setGainDecibels(*gain);
    pannerDSP.setRule(static_cast<juce::dsp::PannerRule>((int)*panRule));
    pannerDSP.setPan(*panAngle / 100);
    reverbParams.roomSize = (*reverbSize / 100);
    reverbParams.damping = (*reverbDamping / 100);
    reverbParams.wetLevel = (*reverbWet / 100);
    reverbParams.width = *reverbWidth / 100;

    reverbDSP.setParameters(reverbParams);

    juce::dsp::AudioBlock<T> audioBlock(buffer);
    juce::dsp::ProcessContextReplacing<T> context(audioBlock);

    if (!bypass->operator float()) {
        gainDSP.process(context);
        pannerDSP.process(context);
        reverbDSP.process(context);
    }

    // êMçÜèàóùÇ±Ç±Ç‹Ç≈

}

//==============================================================================
bool HelReverbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* HelReverbAudioProcessor::createEditor()
{
    return new HelReverbAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void HelReverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void HelReverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName(parameters.state.getType())) {
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HelReverbAudioProcessor();
}
