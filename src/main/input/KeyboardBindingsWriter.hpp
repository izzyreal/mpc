#pragma once

#include "input/KeyboardBindings.hpp"

#include <nlohmann/json.hpp>

using namespace mpc::input;

namespace mpc::input
{
class KeyboardBindingsWriter
{
public:
    static nlohmann::json toJson(const KeyboardBindings& kb)
    {
        nlohmann::json j;
        for (auto& [k,v] : kb.getKeyboardBindingsData())
        {
            j[std::to_string((int)k)] = {
                {"componentLabel", v.componentLabel},
                {"direction", (int)v.direction}
            };
        }
        return j;
    }
};
}
