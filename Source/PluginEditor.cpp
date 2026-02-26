#include "PluginProcessor.h"
#include "PluginEditor.h"


FourierSynthEditor::FourierSynthEditor (FourierSynthProcessor& _audioProcessor, juce::AudioProcessorValueTreeState& _apvts)
    : AudioProcessorEditor (&_audioProcessor), audioProcessor (_audioProcessor), apvts(_apvts)
{
    setResizable(true, false);

    addAndMakeVisible (gainSlider);
    gainSlider.setRange (50, 5000.0);
    gainAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, "gain", gainSlider));

    for(size_t i=0; i<audioProcessor.harmonicGains_.size(); i++){
        harmonicGainSliders.push_back(std::make_unique<juce::Slider>());
        addAndMakeVisible(*harmonicGainSliders.at(i));
        harmonicGainSliders.at(i)->setRange(0.f, 1.f);
        harmonicGainSliders.at(i)->setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);

        harmonicGainAttachments.emplace_back();
        harmonicGainAttachments.at(i).reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, std::format("harmonicGain{}", i), *harmonicGainSliders.at(i)));
    }

    setSize (400, 300);
}

FourierSynthEditor::~FourierSynthEditor() {}


void FourierSynthEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

// Place components on the window
void FourierSynthEditor::resized() 
{
    float border = 30;

    gainSlider.setBounds (border, 200, getWidth() - 2*border, 20);

    float sliderWidth = (getWidth() - 2*border)/harmonicGainSliders.size();
    for(int i=0; i<harmonicGainSliders.size(); i++)
        harmonicGainSliders.at(i)->setBounds(border + sliderWidth*i, 75, sliderWidth, 100);
}