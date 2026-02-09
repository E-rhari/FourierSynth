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
    
    // Define o tamanho do editor
    setSize (400, 300);
}

FourierSynthEditor::~FourierSynthEditor() {}

void FourierSynthEditor::paint (juce::Graphics& g)
{
    // Preenche a tela com uma cor solida
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);

    // TODO: Define nome do plugin na tela. Aqui usamos "Plugin" apenas
    g.drawFittedText ("Plugin", getLocalBounds(), juce::Justification::centred, 1);
}

// Funcao em que sao definidas posicoes customizadas dos elementos
void FourierSynthEditor::resized() 
{
    audioProcessor.keyboardComponent.setBounds (getLocalBounds().removeFromTop(80).reduced(8));
    auto sliderLeft = 80;
    gainSlider.setBounds (sliderLeft, 200, getWidth() - sliderLeft - 10, 20);
}