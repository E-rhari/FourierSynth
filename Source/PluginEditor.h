//==============================================================================
// PluginEditor.h: definicao do PluginEditor
//==============================================================================

#pragma once

#include "PluginProcessor.h"

class FourierSynthEditor : public juce::AudioProcessorEditor
{
public:
    FourierSynthEditor (FourierSynthProcessor&, juce::AudioProcessorValueTreeState&);
    ~FourierSynthEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // Referencia para o editor acessar o objeto MyAudioProcessor que o criou
    FourierSynthProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& apvts;

    juce::Slider gainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FourierSynthEditor)
};