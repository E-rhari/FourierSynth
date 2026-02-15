#pragma once

struct Voice
{
    int note;
    int velocity;

    void reset()
    {
        note = -1;
        velocity = 0;
    }
};