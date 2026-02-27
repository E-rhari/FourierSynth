#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <stdlib.h>


#define PI juce::MathConstants<float>::pi

class Oscillator
{
public:
    float amplitude;
    float freq;
    float sampleRate;
    float phaseOffset;
    int sampleIndex;

    std::vector<float> harmonics;


    void reset()
    {
        sampleIndex = 0;
    }
    
    
    float nextSample()
    {
        sampleIndex += 1;
        float sampleValue = 0;

        for(size_t i=0; i<10; i++)
            sampleValue += std::sin(2*PI * sampleIndex * ((i+1)*freq / sampleRate) + phaseOffset) * harmonics.at(i);

        return amplitude * sampleValue;
    }

    void setAmplitudes(std::vector<float> harmonics_){
        // amplitudes.resize(harmonics_.size());
        // for(size_t i=0; i<harmonics_.size(); i++)
        //     amplitudes.at(i) = harmonics_.at(i);
        harmonics = harmonics_;
    }
};