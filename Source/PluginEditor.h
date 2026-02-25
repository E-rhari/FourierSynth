/** @brief Definitions for the editor (GUI) classes */

#pragma once

#include "PluginProcessor.h"
#include "Debug.h"

class FourierSynthEditor : public juce::AudioProcessorEditor
{
public:
    FourierSynthEditor (FourierSynthProcessor&, juce::AudioProcessorValueTreeState&);
    ~FourierSynthEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    FourierSynthProcessor& audioProcessor;      // Reference for the processor that created this editor
    juce::AudioProcessorValueTreeState& apvts;  // Referece for the apvts of the audioprocessor. Shortcut for audioProcessor.apvts.

    juce::Slider gainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    std::vector<std::unique_ptr<juce::Slider>> harmonicGainSliders;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> harmonicGainAttachments;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FourierSynthEditor)
};