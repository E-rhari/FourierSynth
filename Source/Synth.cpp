#include "Synth.h"
#include <cstdlib>
#include <math.h>


Synth::Synth()
{
    sampleRate = 44100.f;
}


void Synth::allocateResources(double sampleRate_, int /*samplesPerBlock*/)
{
    sampleRate = sampleRate_;
}


void Synth::deallocateResources()
{
    // do nothing for now
}


void Synth::reset()
{
    voice.reset();
}


void Synth::render(float* outputBuffers[], int sampleCount, float gain)
{
    float* leftBuffer  = outputBuffers[0];
    float* rightBuffer = outputBuffers[1];
    
    for(int sample = 0; sample < sampleCount; sample++){
        if(voice.note == -1)
            continue;

        // double sampleValue = sin(currentAngle) * gain * (voice.velocity/127.f);
        // double sampleValue = ((rand() % 3) - 1) * gain * (voice.velocity/127.f);
        double sampleValue = voice.render();
        
        leftBuffer[sample] = sampleValue;
        if(rightBuffer != nullptr)
            rightBuffer[sample] = sampleValue;
    }
}


void Synth::handleMidi(uint8_t data0, uint8_t data1, uint8_t data2)
{
    switch(data0 & 0xF0){   // Reads the command, that is, the 4 most significant bits of the status byte
        case 0x80:          // Note Off
            noteOff(data1 & 0x7F);
            break;

        case 0x90:          // Note On
            uint8_t note = data1 & 0x7F;
            uint8_t velo = data2 & 0x7F;

            // For optimization, MIDI will at times send a Note On command with velocity 0 meaning Note Off
            if(velo > 0)
                noteOn(note, velo);
            else
                noteOff(note);
            break;
    }
}


float Synth::midiNoteToFrequency(int note)
{
    const float refFreq = 440.f;
    const int   refNote = 69;

    return refFreq * std::pow(2, (note-refNote)/12.f);   // refFreq * (¹²√2)^(note-refNote). 12TET equal temperament frequency conversion
}


void Synth::noteOn(int note, int velocity){
    voice.note = note;
    voice.velocity = velocity;

    voice.osc.amplitude = (velocity / 127.f) * 0.5f;
    voice.osc.freq = midiNoteToFrequency(note);
    voice.osc.sampleRate = sampleRate;
    voice.osc.phaseOffset = 0.f;
    voice.osc.reset();
}

void Synth::noteOff(int note)
{
    if(voice.note == note){
        voice.note = -1;
        voice.velocity = 0;
    }
}