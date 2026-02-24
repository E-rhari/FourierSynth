#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


#define PI juce::MathConstants<float>::pi

class Oscillator
{
public:
    float amplitude;
    float freq;
    float sampleRate;
    float phaseOffset;
    int sampleIndex;

    float amplitudes[10];


    void reset()
    {
        sampleIndex = 0;
    }
    
    
    float nextSample()
    {
        sampleIndex += 1;
        float sampleValue = 0;

        for(int i=0; i<10; i++)
            sampleValue += std::sin(2*PI * sampleIndex * ((i+1)*freq / sampleRate) + phaseOffset) * amplitudes[i];

        return amplitude * sampleValue;
    }
};