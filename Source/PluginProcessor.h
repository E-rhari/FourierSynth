/** @brief Definitions for audio data processing classes */

#pragma once
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Woverloaded-virtual=1"
#endif

#define _USE_MATH_DEFINES

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/gui/juce_KeyboardComponentBase.h>
#include <juce_audio_utils/gui/juce_MidiKeyboardComponent.h>

#include <atomic>
#include <vector>
#include <cmath>

#include <chrono>
#include <ctime>

#include "Preset.h"

/** @brief Namespace for plugin parameters IDs */
namespace ParamID {
    // Defines a model to instantiate ParaterIDs
    #define PARAMETER_ID(str) const juce::ParameterID str(#str, 1);

    // Initiates all parameters IDs
    PARAMETER_ID(gain)
    PARAMETER_ID(frequency);
    
    // Undefines model so it won't be used unproperly outside the namespace
    #undef PARAMETER_ID
}

class FourierSynthProcessor : public juce::AudioProcessor, private juce::ValueTree::Listener
{
public:
    FourierSynthProcessor();
    ~FourierSynthProcessor() override;

    // * Audio Processing *

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void releaseResources() override;
    double getTailLengthSeconds() const override;

    // * GUI *

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    // * Preset management *

    const juce::String getName() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // * MIDI *

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    // * Plugin specific parameters *

    float gain_;
    float frequency_;


    // * Mathy stuff *

    void virtual updateDeltaAngle();

    // * Debug *

    juce::TextEditor debugBox;

    void virtual debugLog(const juce::String& string, bool showTime=true);

private:
    // * Parameter Management *

    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", createParameterLayout() }; // Parameter Tree
    std::vector<Preset> presets;    // Preset List
    int currentProgram;             // Curent Preset
    std::atomic<bool> parametersChanged { false };    

    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void update();
    void createPrograms();

    template<typename T>
    inline static void castParameter(juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& id, T& destination);

    // * Plugin specific parameters and propreties *

    juce::LinearSmoothedValue<float> smoother;  // Parameter change smoother
    juce::AudioParameterFloat* gainParam;
    juce::AudioParameterFloat* frequencyParam;

    double currentSampleRate = 0;
    double currentAngle = 0;
    double deltaAngle = 0;

    void splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void handleMidi(uint8_t data0, uint8_t data1, uint8_t data2);
    void render(juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FourierSynthProcessor)
};
