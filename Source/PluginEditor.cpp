#include "PluginProcessor.h"
#include "PluginEditor.h"


FourierSynthEditor::FourierSynthEditor (FourierSynthProcessor& _audioProcessor, juce::AudioProcessorValueTreeState& _apvts)
    : AudioProcessorEditor (&_audioProcessor), audioProcessor (_audioProcessor), apvts(_apvts)
{
    setResizable(true, false);

    addAndMakeVisible(audioProcessor.keyboardComponent);

    addAndMakeVisible (gainSlider);
    gainSlider.setRange (50, 5000.0);
    gainAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, "gain", gainSlider));
    
    setSize (400, 300);
}

FourierSynthEditor::~FourierSynthEditor() {}


void FourierSynthEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);

    g.drawFittedText ("FourierSynth", getLocalBounds(), juce::Justification::centred, 1);
}

// Place components on the window
void FourierSynthEditor::resized() 
{
    audioProcessor.keyboardComponent.setBounds (getLocalBounds().removeFromTop(80).reduced(8));
    auto sliderLeft = 80;
    gainSlider.setBounds (sliderLeft, 200, getWidth() - sliderLeft - 10, 20);
}