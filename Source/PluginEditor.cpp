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
    
    addAndMakeVisible(audioProcessor.debugBox);
    audioProcessor.debugBox.setMultiLine (true);
    audioProcessor.debugBox.setReturnKeyStartsNewLine (true);
    audioProcessor.debugBox.setReadOnly (true);
    audioProcessor.debugBox.setScrollbarsShown (true);
    audioProcessor.debugBox.setCaretVisible (false);
    audioProcessor.debugBox.setPopupMenuEnabled (true);
    audioProcessor.debugBox.setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x32ffffff));
    audioProcessor.debugBox.setColour (juce::TextEditor::outlineColourId, juce::Colour (0x1c000000));
    audioProcessor.debugBox.setColour (juce::TextEditor::shadowColourId, juce::Colour (0x16000000));

    setSize (400, 300);
    audioProcessor.debugLog("-= Fourier Synth =-", false);
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
    float border = 30;

    audioProcessor.keyboardComponent.setBounds (getLocalBounds().removeFromTop(80).reduced(8));
    gainSlider.setBounds (border, 200, getWidth() - border - 10, 20);
    audioProcessor.debugBox.setBounds(0, getHeight() - 75, getWidth(), 75);
}