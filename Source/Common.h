#pragma once

//==============================================================================
// Common.h: funcoes comuns a todos os plugins
//==============================================================================

//==============================================================================
// Funcoes gerais
//------------------------------------------------------------------------------
// Cria plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new FourierSynthProcessor(); }

// Nome do plugin
const juce::String FourierSynthProcessor::getName() const { return "FourierSynth"; }

// Tamanho da cauda gerada pelo processamento do plugin
double FourierSynthProcessor::getTailLengthSeconds() const { return 0.0; }

// Indica se plugin tem editor ou nao
bool FourierSynthProcessor::hasEditor() const { return true; }

// Cria editor generico
juce::AudioProcessorEditor* FourierSynthProcessor::createEditor() 
{ 
    return new FourierSynthEditor(*this, apvts);
}
//==============================================================================

//==============================================================================
// Gestao de presets
//------------------------------------------------------------------------------
// Quantidade de presets
int FourierSynthProcessor::getNumPrograms()
{
    return int(presets.size());
}

// Indice do preset atual
int FourierSynthProcessor::getCurrentProgram()
{
    return currentProgram;
}

// Nome do preset
const juce::String FourierSynthProcessor::getProgramName (int index)
{
    return {presets[(unsigned int)index].name};
}

// Altera nome do preset
void FourierSynthProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName); 
}

// Retorna configuracao atual, com presets, para host capaz de salvar configuracoes, como uma DAW
void FourierSynthProcessor::getStateInformation (juce::MemoryBlock& destData) 
{
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

// Restaura configuracoes salvas
void FourierSynthProcessor::setStateInformation (const void* data, int sizeInBytes) 
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

    if (xml.get() != nullptr && xml->hasTagName(apvts.state.getType())) {
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
        parametersChanged.store(true);
    }
}
//==============================================================================

//==============================================================================
// Gestao de parametros
//------------------------------------------------------------------------------
// Evento de mudanca de parametro
void FourierSynthProcessor::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) {
        parametersChanged.store(true);
}

// Converte tipo de dados do parametro para tipo apropriado
template<typename T>
void FourierSynthProcessor::castParameter(juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& id, T& destination)
{
    destination = dynamic_cast<T>(apvts.getParameter(id.getParamID())); //uma vez que T é inferido, nao precisa passar <T>
    jassert(destination); // Somente em debug: vai falhar se parametro nao existir ou for de tipo incorreto
}
//==============================================================================

//==============================================================================
// Configuracoes de MIDI e barramentos (nao utilizado)
//------------------------------------------------------------------------------
bool FourierSynthProcessor::acceptsMidi() const { return true; }
bool FourierSynthProcessor::producesMidi() const { return true; }
bool FourierSynthProcessor::isMidiEffect() const { return false; }
bool FourierSynthProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}
//==============================================================================
