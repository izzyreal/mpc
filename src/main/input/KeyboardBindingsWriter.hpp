#pragma once

#include "input/KeyboardBindings.hpp"

#include <nlohmann/json.hpp>

using namespace mpc::input;

namespace mpc::input
{
    class KeyboardBindingsWriter
    {
    public:
        static nlohmann::json toJson(const KeyboardBindings &kb)
        {
            return toJson(kb.getKeyboardBindingsData());
        }

        static nlohmann::json toJson(const KeyboardBindingsData &kb)
        {
            nlohmann::json j;
            for (auto &entry : kb)
            {
                j[std::to_string(static_cast<int>(entry.keyCode))] = {
                    {"componentLabel", entry.componentLabel},
                    {"direction", static_cast<int>(entry.direction)}};
            }
            return j;
        }
    };
} // namespace mpc::input
