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

            KeyboardBindings kb;

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

                const auto k =
                    static_cast<VmpcKeyCode>(el.at("keyCode").get<int>());
                auto componentLabel =
                    el.at("componentLabel").get<std::string>();
                const auto direction =
                    static_cast<Direction>(el.at("direction").get<int>());
                kb.setBinding(componentLabel, direction, k);
            }

            return kb.getKeyboardBindingsData();
        }
    };
} // namespace mpc::input
