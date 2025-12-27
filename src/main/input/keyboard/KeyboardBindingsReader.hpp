#pragma once

#include "input/keyboard/KeyboardBindings.hpp"
#include "Logger.hpp"

#include <nlohmann/json.hpp>

namespace mpc::input::keyboard
{
    class KeyboardBindingsReader
    {
    public:
        static KeyboardBindingsData fromJson(const nlohmann::json &j)
        {
            if (!j.contains("version"))
            {
                MLOG(
                    "KeyboardBindingsReader: Missing 'version' field in JSON.");
            }

            if (!j.at("version").is_number_integer())
            {
                MLOG(
                    "KeyboardBindingsReader: 'version' field in JSON is not an "
                    "integer number.");
            }

            const auto versionInJson = j.at("version").get<int>();

            if (versionInJson != CURRENT_KEYBOARD_BINDINGS_VERSION)
            {
                MLOG("KeyboardBindingsReader: Expected 'version' " +
                     std::to_string(CURRENT_KEYBOARD_BINDINGS_VERSION) +
                     " in JSON, but instead we got " +
                     std::to_string(versionInJson));
            }

            if (!j.contains("bindings"))
            {
                MLOG(
                    "KeyboardBindingsReader: Missing 'bindings' field in "
                    "JSON.");
                return {};
            }

            KeyboardBindingsData bindingsData;

            const auto bindingsArray = j.at("bindings");

            if (!bindingsArray.is_array())
            {
                MLOG(
                    "KeyboardBindingsReader: 'bindings' field in JSON is not "
                    "an array.");
                return {};
            }

            for (auto &el : bindingsArray)
            {
                if (!el.contains("keyCode") || !el.contains("componentLabel") ||
                    !el.contains("direction"))
                {
                    continue;
                }

                const auto keyCode =
                    static_cast<VmpcKeyCode>(el.at("keyCode").get<int>());

                const auto componentLabel =
                    el.at("componentLabel").get<std::string>();

                const auto direction =
                    static_cast<Direction>(el.at("direction").get<int>());

                bindingsData.push_back(
                    KeyBinding{keyCode, componentLabel, direction});
            }

            return bindingsData;
        }
    };
} // namespace mpc::input::keyboard
