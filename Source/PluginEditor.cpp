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
    updateButton.setVisible(false);

    // Setting up warnings
    addAndMakeVisible(harmonicMaxWarning);
    harmonicMaxWarning.setText("You've exceeded amount of harmonics this plugin expects. To set the value for higher harmonics, press the update button. You may also experience performance issues.");
    harmonicMaxWarning.setColour(juce::Colour(255, 255, 255));
    harmonicMaxWarning.setJustification(juce::Justification::centredTop);

    // Window
    setSize (400, 350);
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
    float windowWidth = getWidth();

    gainSlider.setBounds (border, 220, windowWidth - 2*border, 20);

    float sliderWidth = (windowWidth - 2*border)/harmonicGainSliders.size();
    for(size_t i=0; i<harmonicGainSliders.size(); i++)
        harmonicGainSliders.at(i)->setBounds(border + sliderWidth*i, 75, sliderWidth, 100);

    addHarmonicButton.setBounds(windowWidth - border - 30, 175, 30, 30);
    removeHarmonicButton.setBounds(windowWidth - border - 2*30, 175, 30, 30);
    updateButton.setBounds(windowWidth/2 - 30, 250, 60, 30);

    harmonicMaxWarning.setBounds(border, 280, windowWidth - border*2, 170);
    harmonicMaxWarning.setBoundingBox(juce::Parallelogram<float>(juce::Rectangle<float>(border, 290, windowWidth - border*2, 150)));

    updateButton.setVisible(audioProcessor.hasReachedMaxHarmonics);
    harmonicMaxWarning.setVisible(audioProcessor.hasReachedMaxHarmonics);
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
    if(harmonicGainAttachments.size() == 1)
        return;
    harmonicGainAttachments.pop_back();
    harmonicGainSliders.pop_back();

    resized();
}