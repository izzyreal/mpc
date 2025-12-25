#pragma once
#include <nlohmann/json.hpp>

#include "KeyboardBindings.hpp"

using namespace mpc::input;

namespace mpc::input
{
class KeyboardBindingsReader
{
public:
    static KeyboardBindingsData fromJson(const nlohmann::json& j)
    {
        KeyboardBindings kb;

        for (auto& el : j.items())
        {
            VmpcKeyCode k = (VmpcKeyCode)std::stoi(el.key());
            auto componentLabel = el.value().at("componentLabel").get<std::string>();
            auto direction = (Direction)el.value().at("direction").get<int>();
            kb.setBinding(componentLabel, direction, k);
        }

        return kb.getKeyboardBindingsData();
    }
};
}
