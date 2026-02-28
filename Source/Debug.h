#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <stdlib.h>
#include <chrono>
#include <ctime>


class Debug {
public:

    static inline bool active=true;

    // Logs into the terminal that's running the host. This signature allows for some output costumization.
    static void log(const char* string, bool printOrigin=true, bool printTime=true, bool lineBreak=true){
        if(!active)
            return;
        
        if(printTime){
            time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::stringstream ss;
            ss << std::put_time(std::localtime(&now), "%X");
            std::cerr << "[" << ss.str() + "] ";
        }

        if(printOrigin)
            std::cerr << "FourierSynth: ";

        std::cerr << string;

        if(lineBreak)
            std::cerr << "\n";
    }

};