/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HelReverbAudioProcessorEditor::HelReverbAudioProcessorEditor (HelReverbAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts)
{
    bypassButtonAttachment.reset(new ButtonAttachment(valueTreeState, "bypass", bypassButton));
    addAndMakeVisible(bypassButton);
    bypassButton.setButtonText("Bypass Off");
    bypassButton.setColour(juce::TextButton::buttonColourId, juce::Colour::Colour(0, 0, 0));
    bypassButton.onClick = [this]() {
        bool BypassValue = !(*valueTreeState.getRawParameterValue("bypass"));
        valueTreeState.getParameter("bypass")->setValueNotifyingHost(BypassValue);
        bypassButton.setButtonText(BypassValue ? "Bypass On" : "Bypass Off");
        bypassButton.setColour(juce::TextButton::buttonOnColourId, BypassValue ? juce::Colour::Colour(169, 169, 169) : juce::Colour::Colour(0, 0, 0));
    };

    gainSliderAttachment.reset(new SliderAttachment(valueTreeState, "gain", gainSlider));
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical); // これでスライダーを縦向きにできるらしい(すごすぎ)
    gainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, GainAria.getWidth() * 2 / 3, PannerAria.getHeight() / 6);
    gainSlider.setTextValueSuffix(" dB");
    // ↑Gainの値が書いてあるテキストボックスについて(場所, 読み込み専用か否か, 幅, 高さ)を指定
    addAndMakeVisible(gainSlider);

    panAngleSliderAttachment.reset(new SliderAttachment(valueTreeState, "panangle", panAngleSlider));
    panAngleSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag); // スライダーを回転ノブの形にした
    panAngleSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, PannerAria.getWidth() * 2 / 3, PannerAria.getHeight() / 6);
    // ↑Panの値が書いてあるテキストボックスについて(場所, 読み込み専用か否か, 幅, 高さ)を指定
    addAndMakeVisible(panAngleSlider);

    panRuleBox.addItemList(
        juce::StringArray("linear", "balanced", "sin3dB", "sin4.5dB", "sin6dB", "sqrt3dB", "sqrt4.5dB"),
        1);


    panRuleBoxAttachment.reset(new ComboBoxAttachment(valueTreeState, "panrule", panRuleBox));
    panAngleSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, PannerAria.getWidth() * 2 / 3, PannerAria.getHeight() / 6);
    addAndMakeVisible(panRuleBox);

    reverbRoomLabel.setText("Room Size", juce::dontSendNotification);
    reverbRoomLabel.attachToComponent(&reverbRoomSlider, false);
    reverbRoomLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbRoomLabel);
    reverbRoomSliderAttachment.reset(new SliderAttachment(valueTreeState, "roomsize", reverbRoomSlider));
    reverbRoomSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    reverbRoomSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, ReverbRoomAria.getWidth() * 2 / 3, ReverbRoomAria.getHeight() / 10);
    addAndMakeVisible(reverbRoomSlider);

    reverbDampLabel.setText("Damping", juce::dontSendNotification);
    reverbDampLabel.attachToComponent(&reverbDampSlider, false);
    reverbDampLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbDampLabel);
    reverbDampSliderAttachment.reset(new SliderAttachment(valueTreeState, "damping", reverbDampSlider));
    reverbDampSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    reverbDampSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, ReverbDampAria.getWidth() * 2 / 3, ReverbDampAria.getHeight() / 10);
    addAndMakeVisible(reverbDampSlider);

    reverbWetLabel.setText("Wet", juce::dontSendNotification);
    reverbWetLabel.attachToComponent(&reverbWetSlider, false);
    reverbWetLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbWetLabel);
    reverbWetSliderAttachment.reset(new SliderAttachment(valueTreeState, "wetlevel", reverbWetSlider));
    reverbWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    reverbWetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, ReverbWetAria.getWidth() * 2 / 3, ReverbWetAria.getHeight() / 10);
    addAndMakeVisible(reverbWetSlider);

    reverbWidthLabel.setText("Width", juce::dontSendNotification);
    reverbWidthLabel.attachToComponent(&reverbWidthSlider, false);
    reverbWidthLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbWidthLabel);
    reverbWidthSliderAttachment.reset(new SliderAttachment(valueTreeState, "width", reverbWidthSlider));
    reverbWidthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    reverbWidthSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, ReverbWidthAria.getWidth() * 2 / 3, ReverbWidthAria.getHeight() / 10);
    addAndMakeVisible(reverbWidthSlider);

    distortionLabel.setText("Bomb", juce::dontSendNotification);
    distortionLabel.attachToComponent(&distortionSlider, false);
    distortionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(distortionLabel);
    distortionSliderAttachment.reset(new SliderAttachment(valueTreeState, "distortion", distortionSlider));
    distortionSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    distortionSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, ReverbDampAria.getWidth() * 2 / 3, DistortionAria.getHeight() / 10);
    addAndMakeVisible(distortionSlider);

    destroyLevel = *valueTreeState.getRawParameterValue("distortion") / 100;
    DtouImages.setImage(dtou, juce::RectanglePlacement::Flags::centred);
    DtouDestroyedImages.setImage(dtou_destroyed, juce::RectanglePlacement::Flags::centred);
    DtouImages.setAlpha(1 - destroyLevel);
    DtouDestroyedImages.setAlpha(destroyLevel);
    distortionSlider.onValueChange = [this]() {
        destroyLevel = *valueTreeState.getRawParameterValue("distortion") / 100; // D棟画像切り替え用
        DtouImages.setAlpha(1 - destroyLevel);
        DtouDestroyedImages.setAlpha(destroyLevel);
    };
    addAndMakeVisible(DtouImages);
    addAndMakeVisible(DtouDestroyedImages);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(width, height);
}

HelReverbAudioProcessorEditor::~HelReverbAudioProcessorEditor()
{
}

//==============================================================================
void HelReverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colour::Colour(46, 52, 64)); // おそらくプラグインの背景色だと思われるが...
    g.fillRect(GainAria.toFloat()); // よくわからんが、Gainのエリアだけ色を塗ったらしい

    //dtou_destroyed.multiplyAllAlphas(destroyLevel);
    //g.drawImage(dtou, DtouAria.toFloat(), juce::RectanglePlacement::Flags::centred);
    //g.drawImage(dtou_destroyed, DtouAria.toFloat(), juce::RectanglePlacement::Flags::centred, false);
}

void HelReverbAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    gainSlider.setBounds(GainAria.reduced(5));
    panAngleSlider.setBounds(PannerAria.reduced(5));
    panRuleBox.setBounds(PannerRuleAria.reduced(PannerAria.getWidth() / 6, 4));
    bypassButton.setBounds(BypassAria.reduced(GainAria.getWidth() / 6, 4));
    reverbRoomSlider.setBounds(ReverbRoomAria.reduced(40));
    reverbDampSlider.setBounds(ReverbDampAria.reduced(40));
    distortionSlider.setBounds(DistortionAria.reduced(20));
    reverbWetSlider.setBounds(ReverbWetAria.reduced(40));
    reverbWidthSlider.setBounds(ReverbWidthAria.reduced(40));
    DtouImages.setBounds(DtouAria.reduced(5));
    DtouDestroyedImages.setBounds(DtouAria.reduced(5));
}
