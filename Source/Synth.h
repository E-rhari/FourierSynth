#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "Voice.h"
#include "Debug.h"


class Synth
{
public:
    Synth();

    void allocateResources(double sampleRate, int samplesPerBlock, float frequency);
    void deallocateResources();
    void reset();
    void render(float* outputBuffers[], int sampleCount, float gain);
    void handleMidi(uint8_t data0, uint8_t data1, uint8_t data2);

private:
    double sampleRate;
    Voice voice;

    double currentSampleRate = 0;
    double currentAngle = 0;
    double deltaAngle = 0;

    void noteOn(int note, int velocity);
    void noteOff(int note);

    void virtual updateDeltaAngle(float frequency);
};