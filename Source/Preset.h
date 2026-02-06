#pragma once

#include <string>
#include <initializer_list>
#include <vector>

//==============================================================================
// Preset.h: estrutura padrao dos presets
//==============================================================================

struct Preset
{
    Preset(const char* pname,  std::initializer_list<float> param_list) : name(pname, 40)
    {
        param = param_list;
    }

    std::string name;
    std::vector<float> param;
};
