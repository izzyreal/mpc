#pragma once

#include <map>
#include <vector>
#include <optional>

#include "controls/KeyCodeHelper.hpp"
#include "hardware/ComponentId.h"

namespace mpc::inputlogic
{

enum class Direction
{
    None,
    Positive,
    Negative
};

constexpr int toSign(Direction d)
{
    switch (d)
    {
        case Direction::Positive: return +1;
        case Direction::Negative: return -1;
        default:                  return 0;
    }
}

struct KeyboardBinding
{
    hardware::ComponentId componentId;
    Direction direction = Direction::None;
};

class KeyboardBindings
{
public:
    KeyboardBindings();

    std::vector<mpc::controls::VmpcKeyCode> lookupComponent(hardware::ComponentId id) const;
    std::optional<KeyboardBinding> lookup(controls::VmpcKeyCode key) const;

private:
    std::map<controls::VmpcKeyCode, KeyboardBinding> bindings;
    void initializeDefaults();
};

} // namespace mpc::inputlogic
