#include "PluginProcessor.h"
#include "PluginEditor.h"


FourierSynthEditor::FourierSynthEditor (FourierSynthProcessor& _audioProcessor, juce::AudioProcessorValueTreeState& _apvts)
    : AudioProcessorEditor (&_audioProcessor), audioProcessor (_audioProcessor), apvts(_apvts)
{
    setResizable(true, false);

    // Sliders
    addAndMakeVisible (gainSlider);
    gainSlider.setRange (50, 5000.0);
    gainAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, ParamID::gain.getParamID(), gainSlider));

    for(size_t i=0; i<audioProcessor.harmonicGains_.size(); i++)
        addHarmonic();

    // Setting up buttons
    addHarmonicButton.onClick    = [this]() { audioProcessor.addHarmonic(); addHarmonic(); }; 
    removeHarmonicButton.onClick = [this]() { audioProcessor.removeHarmonic(); removeHarmonic(); }; 
    updateButton.onClick         = [this]() { audioProcessor.update(); };

    addAndMakeVisible(addHarmonicButton);
    addHarmonicButton.setButtonText("+");
    addAndMakeVisible(removeHarmonicButton);
    removeHarmonicButton.setButtonText("-");
    addAndMakeVisible(updateButton);
    updateButton.setButtonText("Update");

    // Window
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

    gainSlider.setBounds (border, 220, getWidth() - 2*border, 20);

    float sliderWidth = (getWidth() - 2*border)/harmonicGainSliders.size();
    for(size_t i=0; i<harmonicGainSliders.size(); i++)
        harmonicGainSliders.at(i)->setBounds(border + sliderWidth*i, 75, sliderWidth, 100);

    addHarmonicButton.setBounds(getWidth() - border - 30, 175, 30, 30);
    removeHarmonicButton.setBounds(getWidth() - border - 2*30, 175, 30, 30);
    updateButton.setBounds(getWidth()/2 - 30, 250, 60, 30);
}


void FourierSynthEditor::addHarmonic(){
    size_t index = harmonicGainSliders.size();

    harmonicGainSliders.push_back(std::make_unique<juce::Slider>());
    addAndMakeVisible(*harmonicGainSliders.at(index));
    harmonicGainSliders.at(index)->setRange(0.f, 1.f);
    harmonicGainSliders.at(index)->setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);

    harmonicGainAttachments.emplace_back();
    harmonicGainAttachments.at(index).reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, ParamID::harmonicGains.at(index).getParamID(), *harmonicGainSliders.at(index)));
    
    resized();
}


void FourierSynthEditor::removeHarmonic(){
    harmonicGainAttachments.pop_back();
    harmonicGainSliders.pop_back();

    resized();
}