#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Common.h"

// Amount of Parameters
const long unsigned int NUM_PARAMS = 2;


FourierSynthProcessor::FourierSynthProcessor()
    : AudioProcessor (BusesProperties()
        // Define plugin as stereo
        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      keyboardComponent (keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Configure Parameters
    gain_ = 1.0f;
    frequency_ = 440.0f;

    castParameter(apvts, ParamID::gain, gainParam);
    castParameter(apvts, ParamID::frequency, frequencyParam);
    apvts.state.addListener(this);
    
    // Configure presets
    createPrograms();
    setCurrentProgram(0);

    // MIDI
    keyboardState.addListener(this);
}

FourierSynthProcessor::~FourierSynthProcessor() 
{
    apvts.state.removeListener(this);
}


// * Audio Processing *

// Executes right before processing
void FourierSynthProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
     juce::ignoreUnused(samplesPerBlock); 
     
     currentSampleRate = sampleRate;
     updateDeltaAngle();
}

// Processes the audio - AUDIO THREAD!!!
void FourierSynthProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    // Define channel pointer to write audio data
    auto* channelDataLeft  = buffer.getWritePointer(0);
    auto* channelDataRight = buffer.getWritePointer(1);
    
    // loops over all the samples that should be written
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        double sampleValue = sin(currentAngle) * gain_;
        channelDataLeft[sample] = sampleValue;
        channelDataRight[sample] = sampleValue;

        currentAngle += deltaAngle;
    }

    // Detects change in any of the apvts parameters.
    // The variable parametersChanged changes trough the event valueTreePropertyChanged, executed in the GUI thread (located at Common.h)
    
    bool expected = true;
    if (parametersChanged.compare_exchange_strong(expected, false)) {
        update();
    }
}

// Executes right after processing
void FourierSynthProcessor::releaseResources() {}


// * Parameter Management *

// Updates parameters according to the apvts values - AUDIO THREAD!!!
void FourierSynthProcessor::update() {
    smoother.setCurrentAndTargetValue(gainParam->get());

    gain_ = gainParam->get();
    frequency_ = frequencyParam->get();

    updateDeltaAngle();
}

// Configures apvts parameter layout
juce::AudioProcessorValueTreeState::ParameterLayout FourierSynthProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParamID::gain,
        "Gain",
        juce::NormalisableRange(0.0f, 2.0f),
        1.0f
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParamID::frequency,
        "Frequency",
        juce::NormalisableRange(20.f, 7902.13f, 1.f, 0.5f, false), // Respectivamente: Inicio, Fim, Tamanho do passo, skew-factor, simetria
        440.0f
    ));

    return layout;
}


// * Preset management * 
// (Only plugin specific management. For generic perset management, see Preset.h and Common.h)

// Creates presets
void FourierSynthProcessor::createPrograms()
{
    presets.emplace_back(Preset("A4", {1.0f, 440}));
    presets.emplace_back(Preset("A5", {1.0f, 880}));
}

void FourierSynthProcessor::setCurrentProgram (int index)
{
    currentProgram = index;
    
    juce::RangedAudioParameter* params[NUM_PARAMS] = {
        gainParam,
        frequencyParam
    };

    const Preset& preset = presets[(unsigned int)index];

    for (long unsigned int i = 0; i < NUM_PARAMS; ++i)
    {
        params[i]->setValueNotifyingHost(params[i]->convertTo0to1(preset.param[i]));
    }
    
    reset();
}


// * Mathy Stuff *

void FourierSynthProcessor::updateDeltaAngle(){
    deltaAngle = (frequency_/currentSampleRate) * 2.0 * juce::MathConstants<double>::pi;
}


// * MIDI *

void FourierSynthProcessor::handleNoteOn (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
        auto m = juce::MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity);
        m.setTimeStamp (juce::Time::getMillisecondCounterHiRes() * 0.001);
        logMessage("Note on!");
}

void FourierSynthProcessor::handleNoteOff (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*velocity*/)
{
        auto m = juce::MidiMessage::noteOff (midiChannel, midiNoteNumber);
        m.setTimeStamp (juce::Time::getMillisecondCounterHiRes() * 0.001);
        logMessage("Note off!");
}


// * Debug *
void FourierSynthProcessor::logMessage(const juce::String& string, bool showTime /*=true*/)
{
    juce::String message = "";

    if(showTime){
        time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now), "%Y-%m-%d %X");
        message += juce::String(ss.str()) + ": ";
    }

    message += string;
    debugBox.moveCaretToEnd();
    debugBox.insertTextAtCaret(message + "\n");
}