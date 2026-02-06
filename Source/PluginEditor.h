//==============================================================================
// PluginEditor.h: definicao do PluginEditor
//==============================================================================

#pragma once

#include "PluginProcessor.h"

class FourierSynthEditor : public juce::AudioProcessorEditor
{
public:
    FourierSynthEditor (FourierSynthProcessor&);
    ~FourierSynthEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // Referencia para o editor acessar o objeto MyAudioProcessor que o criou
    FourierSynthProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FourierSynthEditor)
};