#pragma once

#include "Oscillator.h"


struct Voice
{
    int note;
    int velocity;
    Oscillator osc;


    void reset()
    {
        note = -1;
        velocity = 0;
        osc.reset();
    }

    
    float render()
    {
        return osc.nextSample();
    }
};