#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Debug.h"

// Amount of Parameters
const long unsigned int NUM_PARAMS = 2;



FourierSynthProcessor::FourierSynthProcessor() : AudioProcessor (BusesProperties()
                                                // Define plugin as stereo
                                                .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                                .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
    // Configure Parameters
    gain_ = 1.0f;
    frequency_ = 440.0f;

    // Populate the Harmonic Gains Parameters
    harmonicGainParams.resize(10);
    harmonicGains_.resize(10);
    harmonicGains_.at(0) = 1;
    for(float harmonicGain_ : harmonicGains_)
        harmonicGain_ = 0;
    ParamID::populateHarmonicGainsID(10);

    // Allocate the parameters to the pointers '[parameter]Param'
    castParameter(apvts, ParamID::gain, gainParam);
    castParameter(apvts, ParamID::frequency, frequencyParam);
    for(size_t i=0; i<ParamID::harmonicGains.size(); i++) 
        castParameter(apvts, ParamID::harmonicGains.at(i), harmonicGainParams.at(i));
    apvts.state.addListener(this);
    
    // Configure presets
    createPrograms();
    setCurrentProgram(0);

    Debug::log("-= Fourier Synth =-", false);
}

FourierSynthProcessor::~FourierSynthProcessor() 
{
    apvts.state.removeListener(this);
}


// * General *

// Creates plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new FourierSynthProcessor(); }

// Plugin Name
const juce::String FourierSynthProcessor::getName() const { return "FourierSynth"; }
double FourierSynthProcessor::getTailLengthSeconds() const { return 0.0; }
bool FourierSynthProcessor::hasEditor() const { return true; }

// Starts the editor and links it to the processor
juce::AudioProcessorEditor* FourierSynthProcessor::createEditor() 
{ 
    return new FourierSynthEditor(*this, apvts);
}


void FourierSynthProcessor::reset(){
    synth.reset();
}


// * Audio Processing *

// Segments the current block into smaller sections limited by the midi events.
// It then renders every segment individually according to the currently playing midi notes.
// Will also handle midi data as it comes in the midi buffer.
void FourierSynthProcessor::splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages){
    int bufferOffset = 0;

    for(const juce::MidiMessageMetadata metadata : midiMessages){
        // Render the audio that happens before this event (if any)
        int samplesThisSegment = metadata.samplePosition - bufferOffset;
        if(samplesThisSegment > 0){
            render(buffer, samplesThisSegment, bufferOffset);
            bufferOffset += samplesThisSegment;
        }

        // Handle the event. Ignore MIDI messages such as sysex.
        if(metadata.numBytes <= 3){
            uint8_t data1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
            uint8_t data2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;
            handleMidi(metadata.data[0], data1, data2);
        }
    }

    // Render the audio after the last MIDI event. 
    // If there were no MIDI events at all, this renders the entire buffer.
    int samplesLastSegment = buffer.getNumSamples() - bufferOffset;
    if(samplesLastSegment > 0)
        render(buffer, samplesLastSegment, bufferOffset);
    
    midiMessages.clear();
}

void FourierSynthProcessor::render(juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset)
{
    // Define channel pointer to write audio data
    float* outputBuffers[2] = {nullptr, nullptr};
    outputBuffers[0] = buffer.getWritePointer(0) + bufferOffset;
    if(getTotalNumOutputChannels() > 1)
        outputBuffers[1] = buffer.getWritePointer(1) + bufferOffset;

    synth.render(outputBuffers, sampleCount, gain_);
}

// Executes right before processing
void FourierSynthProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    synth.allocateResources(sampleRate, samplesPerBlock);
    reset();
}

// Processes the audio - AUDIO THREAD!!!
void FourierSynthProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals; // Blocks the CPU from turning normal floats into denormal float, which are slower

    int totalNumInputChannels = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that don't contain input data.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    splitBufferByEvents(buffer, midiMessages);

    // Detects change in any of the apvts parameters.
    // The variable parametersChanged changes trough the event valueTreePropertyChanged, executed in the GUI thread (located at Common.h)
    
    bool expected = true;
    if (parametersChanged.compare_exchange_strong(expected, false)) {
        update();
    }
}

// Executes right after processing
void FourierSynthProcessor::releaseResources()
{
    synth.deallocateResources();
}


// * Parameter Management *

// Updates parameters according to the apvts values - AUDIO THREAD!!!
void FourierSynthProcessor::update() {
    smoother.setCurrentAndTargetValue(gainParam->get());

    gain_ = gainParam->get();
    frequency_ = frequencyParam->get();

    for(size_t i=0; i<harmonicGainParams.size(); i++)
        harmonicGains_.at(i) = harmonicGainParams.at(i)->get();
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

    for(size_t i=0; i<ParamID::harmonicGains.size(); i++)
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            ParamID::harmonicGains.at(i),
            std::format("Harmonic Gain {}", i),
            juce::NormalisableRange(0.f, 1.f, 0.01f, 1.f, false),
            0.f
        ));

    return layout;
}

void FourierSynthProcessor::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) {
        parametersChanged.store(true);
}

template<typename T>
void FourierSynthProcessor::castParameter(juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& id, T& destination)
{
    destination = dynamic_cast<T>(apvts.getParameter(id.getParamID())); // T is infered
    jassert(destination); // Debug only
}

// * Preset management * 

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

// Amount of presets
int FourierSynthProcessor::getNumPrograms()
{
    return int(presets.size());
}

// Current preset index
int FourierSynthProcessor::getCurrentProgram()
{
    return currentProgram;
}

// Preset Name
const juce::String FourierSynthProcessor::getProgramName (int index)
{
    return {presets[(unsigned int)index].name};
}

// Changes Preset Name
void FourierSynthProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName); 
}

// Return current configuration, with presets, for a host capable of saving them, such as a DAW
void FourierSynthProcessor::getStateInformation (juce::MemoryBlock& destData) 
{
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

// Restores saved configs
void FourierSynthProcessor::setStateInformation (const void* data, int sizeInBytes) 
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

    if (xml.get() != nullptr && xml->hasTagName(apvts.state.getType())) {
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
        parametersChanged.store(true);
    }
}


// * MIDI *

bool FourierSynthProcessor::acceptsMidi() const { return true; }
bool FourierSynthProcessor::producesMidi() const { return false; }
bool FourierSynthProcessor::isMidiEffect() const { return false; }
bool FourierSynthProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void FourierSynthProcessor::handleMidi(uint8_t data0, uint8_t data1, uint8_t data2)
{
    char s[16];
    snprintf(s, 16, "%02hhX %02hhX %02hhX", data0, data1, data2);
    Debug::log(s);

    synth.handleMidi(data0, data1, data2);
}


// * ParamID *

void ParamID::populateHarmonicGainsID(size_t size){
    ParamID::harmonicGains.resize(10);
    for(size_t i=0; i<size; i++)
        ParamID::harmonicGains.at(i) = juce::ParameterID(std::format("harmonicGain{}", i), 1);
}

std::vector<juce::ParameterID> ParamID::harmonicGains;