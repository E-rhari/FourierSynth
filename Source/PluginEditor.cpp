#include "PluginProcessor.h"
#include "PluginEditor.h"


FourierSynthEditor::FourierSynthEditor (FourierSynthProcessor& _audioProcessor, juce::AudioProcessorValueTreeState& _apvts)
    : AudioProcessorEditor (&_audioProcessor), audioProcessor (_audioProcessor), apvts(_apvts)
{
    setResizable(true, false);

    addAndMakeVisible (gainSlider);
    gainSlider.setRange (50, 5000.0);
    gainAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, "gain", gainSlider));
    
    addAndMakeVisible(Debug::box);
    Debug::box.setMultiLine (true);
    Debug::box.setReturnKeyStartsNewLine (true);
    Debug::box.setReadOnly (true);
    Debug::box.setScrollbarsShown (true);
    Debug::box.setCaretVisible (false);
    Debug::box.setPopupMenuEnabled (true);
    Debug::box.setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x32ffffff));
    Debug::box.setColour (juce::TextEditor::outlineColourId, juce::Colour (0x1c000000));
    Debug::box.setColour (juce::TextEditor::shadowColourId, juce::Colour (0x16000000));

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
    float border = 30;

    gainSlider.setBounds (border, 200, getWidth() - border - 10, 20);
    Debug::box.setBounds(0, getHeight() - 75, getWidth(), 75);
}