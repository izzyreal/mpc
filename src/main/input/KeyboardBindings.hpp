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
        NoDirection,
        Positive,
        Negative
    };

    constexpr int toSign(Direction d)
    {
        switch (d)
        {
            case Direction::Positive:
                return +1;
            case Direction::Negative:
                return -1;
            case Direction::NoDirection:
            default:
                return 0;
        }
    }

    struct KeyboardBinding
    {
        hardware::ComponentId componentId;
        Direction direction = Direction::NoDirection;
    };

    class KeyboardBindings
    {
    public:
        KeyboardBindings();

        std::vector<VmpcKeyCode>
        lookupComponent(hardware::ComponentId id) const;
        std::optional<KeyboardBinding> lookup(VmpcKeyCode key) const;

        void initializeDefaults();

    private:
        std::map<VmpcKeyCode, KeyboardBinding> bindings;
    };

} // namespace mpc::input
