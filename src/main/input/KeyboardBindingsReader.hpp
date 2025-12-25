#pragma once
#include <nlohmann/json.hpp>

#include "KeyboardBindings.hpp"

using namespace mpc::input;

namespace mpc::input
{
    class KeyboardBindingsReader
    {
    public:
        static KeyboardBindingsData fromJson(const nlohmann::json &j)
        {
            if (!j.contains("version") ||
                j.at("version").get<int>() !=
                    CURRENT_KEYBOARD_BINDINGS_VERSION ||
                !j.contains("bindings"))
            {
                return {};
            }

            KeyboardBindingsData bindingsData;

            const auto bindingsArray = j.at("bindings");

            if (!bindingsArray.is_array())
            {
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
} // namespace mpc::input
