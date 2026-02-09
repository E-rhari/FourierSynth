#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Common.h"

// TODO: Quantidade de parametros
const long unsigned int NUM_PARAMS = 2;

//==============================================================================
// Construtor e destrutor
//------------------------------------------------------------------------------
FourierSynthProcessor::FourierSynthProcessor()
    : AudioProcessor (BusesProperties()
        //TODO: Define se plugin mono ou stereo
        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      keyboardComponent (keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    //TODO: inicializacao dos parametros do plugin
    gain_ = 1.0f;
    frequency_ = 440.0f;

    castParameter(apvts, ParamID::gain, gainParam);
    castParameter(apvts, ParamID::frequency, frequencyParam);
    apvts.state.addListener(this);
    
    createPrograms();
    setCurrentProgram(0);

    // MIDI
    // addAndMakeVisible (keyboardComponent);
    keyboardState.addListener(this);
}

FourierSynthProcessor::~FourierSynthProcessor() 
{
    apvts.state.removeListener(this);
}
//==============================================================================

//==============================================================================
// Funcoes de processamento de audio
//------------------------------------------------------------------------------

// TODO: funcao que roda logo ANTES de começar a processar
void FourierSynthProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
     juce::ignoreUnused(samplesPerBlock); 
     
     currentSampleRate = sampleRate;
     updateDeltaAngle();
}

// TODO: funcao que processa audio em loop - AUDIO THREAD!!!
void FourierSynthProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    //ignora mensagens MIDI
    juce::ignoreUnused(midiMessages);

    //loop pelos canais (plugin stereo)

    //ponteiro para canal
    auto* channelDataLeft  = buffer.getWritePointer(0);
    auto* channelDataRight = buffer.getWritePointer(1);
    
    //loop pelas amostras de audio no buffer
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        double sampleValue = sin(currentAngle) * gain_;
        channelDataLeft[sample] = sampleValue;
        channelDataRight[sample] = sampleValue;

        currentAngle += deltaAngle;
    }

    //variavel parametersChanged muda pelo evento valueTreePropertyChanged que executa na thread de UI
    bool expected = true;
    if (parametersChanged.compare_exchange_strong(expected, false)) {
        update();
    }
}

// chamada logo DEPOIS de processar
void FourierSynthProcessor::releaseResources() {}

// TODO: atualiza parametros - AUDIO THREAD!!!
void FourierSynthProcessor::update() {
    smoother.setCurrentAndTargetValue(gainParam->get());

    gain_ = gainParam->get();
    frequency_ = frequencyParam->get();

    updateDeltaAngle();
}

//==============================================================================
// Gestao de parametros
//------------------------------------------------------------------------------
// TODO: Cria parametros e adiciona em layout
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

//==============================================================================
// Gestao de presets (somente funcoes que variam por plugin. Ver Common.h)
//------------------------------------------------------------------------------

// TODO: Cria presets iniciais
void FourierSynthProcessor::createPrograms()
{
    presets.emplace_back(Preset("A4", {1.0f, 440}));
    presets.emplace_back(Preset("A5", {1.0f, 880}));
}

// TODO: Define preset atual
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

//==============================================================================
// Cálculos matemáticos 
//------------------------------------------------------------------------------
void FourierSynthProcessor::updateDeltaAngle(){
    deltaAngle = (frequency_/currentSampleRate) * 2.0 * juce::MathConstants<double>::pi;
}
//==============================================================================


//==============================================================================
// MIDI
//------------------------------------------------------------------------------
void FourierSynthProcessor::handleNoteOn (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
        auto m = juce::MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity);
        m.setTimeStamp (juce::Time::getMillisecondCounterHiRes() * 0.001);
}

void FourierSynthProcessor::handleNoteOff (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*velocity*/)
{
        auto m = juce::MidiMessage::noteOff (midiChannel, midiNoteNumber);
        m.setTimeStamp (juce::Time::getMillisecondCounterHiRes() * 0.001);
}
//==============================================================================