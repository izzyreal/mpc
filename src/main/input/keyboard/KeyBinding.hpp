#pragma once

#include "input/keyboard/VmpcKeyCode.hpp"
#include "input/Direction.hpp"
#include "hardware/ComponentId.hpp"

namespace mpc::input::keyboard
{
    struct KeyBinding
    {
        VmpcKeyCode keyCode;

        std::string componentLabel;
        Direction direction = Direction::NoDirection;

        hardware::ComponentId getComponentId() const;
        int getPlatformKeyCode() const;

        bool operator==(const KeyBinding &other) const
        {
            return other.keyCode == keyCode &&
                   other.componentLabel == componentLabel &&
                   other.direction == direction;
        }

        bool operator!=(const KeyBinding &other) const
        {
            return other.keyCode != keyCode ||
                   other.componentLabel != componentLabel ||
                   other.direction != direction;
        }

        KeyBinding &operator=(const KeyBinding &other)
        {
            keyCode = other.keyCode;
            componentLabel = other.componentLabel;
            direction = other.direction;
            return *this;
        }
    };

    inline bool operator<(const KeyBinding &lhs, const KeyBinding &rhs)
    {
        return std::tie(lhs.keyCode, lhs.componentLabel, lhs.direction) <
               std::tie(rhs.keyCode, rhs.componentLabel, rhs.direction);
    }
} // namespace mpc::input::keyboard