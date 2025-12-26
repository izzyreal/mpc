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
            KeyboardBindings kb;

            for (auto &el : j.items())
            {
                const auto k = static_cast<VmpcKeyCode>(std::stoi(el.key()));
                auto componentLabel =
                    el.value().at("componentLabel").get<std::string>();
                const auto direction = static_cast<Direction>(
                    el.value().at("direction").get<int>());
                kb.setBinding(componentLabel, direction, k);
            }

            return kb.getKeyboardBindingsData();
        }
    };
} // namespace mpc::input
