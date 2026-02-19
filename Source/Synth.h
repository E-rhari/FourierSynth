
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "Voice.h"
#include "Debug.h"


class Synth
{
public:
    Synth();

    void allocateResources(double sampleRate, int samplesPerBlock);
    void deallocateResources();
    void reset();
    void render(float* outputBuffers[], int sampleCount, float gain);
    void handleMidi(uint8_t data0, uint8_t data1, uint8_t data2);

private:
    double sampleRate;
    Voice voice;

    void noteOn(int note, int velocity);
    void noteOff(int note);
};