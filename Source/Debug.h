#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


class Debug
{
public:
    Debug(){};
    ~Debug(){};

    inline static juce::TextEditor box = juce::TextEditor();

    static void log(const juce::String& string, bool showTime=true)
    {
        juce::String message = "";

        if(showTime){
            time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::stringstream ss;
            ss << std::put_time(std::localtime(&now), "%Y-%m-%d %X");
            message += juce::String(ss.str()) + ": ";
        }

        message += string;
        Debug::box.moveCaretToEnd();
        Debug::box.insertTextAtCaret(message + "\n");
    }
};