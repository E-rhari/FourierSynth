/** @brief Common code for most plugins */

#pragma once



// * General *

// Creates plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new FourierSynthProcessor(); }

// Plugin Name
const juce::String FourierSynthProcessor::getName() const { return "FourierSynth"; }

double FourierSynthProcessor::getTailLengthSeconds() const { return 0.0; }

bool FourierSynthProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* FourierSynthProcessor::createEditor() 
{ 
    return new FourierSynthEditor(*this, apvts);
}


// * Preset Menagement *

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


// * Parameter Management *

void FourierSynthProcessor::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) {
        parametersChanged.store(true);
}

template<typename T>
void FourierSynthProcessor::castParameter(juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& id, T& destination)
{
    destination = dynamic_cast<T>(apvts.getParameter(id.getParamID())); // T is infered
    jassert(destination); // Debug only
}

// * MIDI *
bool FourierSynthProcessor::acceptsMidi() const { return true; }
bool FourierSynthProcessor::producesMidi() const { return true; }
bool FourierSynthProcessor::isMidiEffect() const { return false; }
bool FourierSynthProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}
