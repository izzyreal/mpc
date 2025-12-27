#pragma once

#include "input/keyboard/KeyboardBindings.hpp"

#include <nlohmann/json.hpp>

namespace mpc::input::keyboard
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
            j["version"] = CURRENT_KEYBOARD_BINDINGS_VERSION;
            j["bindings"] = nlohmann::json::array();
            for (auto &entry : kb)
            {
                j["bindings"].push_back(
                    {{"keyCode", static_cast<int>(entry.keyCode)},
                     {"componentLabel", entry.componentLabel},
                     {"direction", static_cast<int>(entry.direction)}});
            }
            return j;
        }
    };
} // namespace mpc::input::keyboard
