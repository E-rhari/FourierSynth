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


    void reset()
    {
        sampleIndex = 0;
    }
    
    
    float nextSample()
    {
        sampleIndex += 1;
        return amplitude * std::sin(2*PI * sampleIndex * (freq / sampleRate) + phaseOffset);
    }
};