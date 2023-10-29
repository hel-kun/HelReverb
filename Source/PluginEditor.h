/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class HelReverbAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    HelReverbAudioProcessorEditor (HelReverbAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~HelReverbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    int width = 550;
    int height = 550;

    juce::Rectangle<int> BypassAria{ width - 150, 0, 150, 30 };
    juce::Rectangle<int> GainAria{ width - 150, BypassAria.getBottom(), 150, height * 4 / 5 - 60 };
    juce::Rectangle<int> PannerAria{ width - 150, GainAria.getBottom(), 150, height / 5 };
    juce::Rectangle<int> PannerRuleAria{ width - 150, PannerAria.getBottom(), 150, 30 };

    juce::Rectangle<int> ReverbRoomAria{ (width - 150) / 2, (height-150) / 2 };
    juce::Rectangle<int> ReverbDampAria{ (width - 150) / 2, 0, (width - 150) / 2, (height-150) / 2 };
    juce::Rectangle<int> ReverbWetAria{ 0, (height/2)+75, (width - 150) / 2, (height-150) / 2 };
    juce::Rectangle<int> ReverbWidthAria{ (width - 150) / 2, (height/2)+75, (width - 150) / 2, (height-150) / 2 };

    juce::Rectangle<int> DistortionAria{ width/ 2, ReverbRoomAria.getBottom(),(width/2)-150 , 150 };
    juce::Rectangle<int> DtouAria{0, ReverbRoomAria.getBottom(),width/2 , 150 };

    juce::Image dtou = juce::ImageCache::getFromMemory(BinaryData::dtou_png, BinaryData::dtou_pngSize);
    juce::Image dtou_destroyed = juce::ImageCache::getFromMemory(BinaryData::dtou_destroyed_png, BinaryData::dtou_destroyed_pngSize);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    HelReverbAudioProcessor& audioProcessor;

    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::Slider gainSlider;
    std::unique_ptr<SliderAttachment> gainSliderAttachment;
    juce::Slider panAngleSlider;
    std::unique_ptr<SliderAttachment> panAngleSliderAttachment;
    juce::ComboBox panRuleBox;
    std::unique_ptr<ComboBoxAttachment> panRuleBoxAttachment;

    juce::TextButton bypassButton;
    std::unique_ptr<ButtonAttachment> bypassButtonAttachment;

    juce::Slider reverbRoomSlider;
    std::unique_ptr<SliderAttachment> reverbRoomSliderAttachment;
    juce::Slider reverbDampSlider;
    std::unique_ptr<SliderAttachment> reverbDampSliderAttachment;
    juce::Slider reverbWetSlider;
    std::unique_ptr<SliderAttachment> reverbWetSliderAttachment;
    juce::Slider reverbWidthSlider;
    std::unique_ptr<SliderAttachment> reverbWidthSliderAttachment;

    juce::ImageComponent DtouImages;
    juce::ImageComponent DtouDestroyedImages;

    juce::Slider distortionSlider;
    std::unique_ptr<SliderAttachment> distortionSliderAttachment;

    juce::Label reverbRoomLabel;
    juce::Label reverbDampLabel;
    juce::Label reverbWetLabel;
    juce::Label reverbWidthLabel;
    juce::Label distortionLabel;

    float destroyLevel; // DìèÇÃé ê^ÇÃêÿÇËë÷Ç¶ÇÃÇΩÇﬂÇÃïœêî(ìßñæìx)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HelReverbAudioProcessorEditor)
};
