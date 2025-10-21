#pragma once

#include <map>
#include <vector>
#include <optional>

#include "input/KeyCodeHelper.hpp"
#include "hardware/ComponentId.hpp"

namespace mpc::input
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
        case Direction::None:
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

    std::vector<mpc::input::VmpcKeyCode> lookupComponent(hardware::ComponentId id) const;
    std::optional<KeyboardBinding> lookup(VmpcKeyCode key) const;

private:
    std::map<VmpcKeyCode, KeyboardBinding> bindings;
    void initializeDefaults();
};

} // namespace mpc::input
