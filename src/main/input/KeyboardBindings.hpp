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
        std::string componentLabel;
        Direction direction = Direction::NoDirection;

        hardware::ComponentId getComponentId() const;
    };

    using KeyboardBindingsData = std::map<VmpcKeyCode, KeyboardBinding>;

    class KeyboardBindings
    {
    public:
        KeyboardBindings();

        std::vector<VmpcKeyCode>
        lookupComponent(hardware::ComponentId id) const;
        std::optional<KeyboardBinding> lookup(VmpcKeyCode key) const;

        void setBinding(hardware::ComponentId, Direction, VmpcKeyCode);
        void setBinding(std::string componentLabel, Direction, VmpcKeyCode);
        void setBindingsData(KeyboardBindingsData &);

        void initializeDefaults();

        KeyboardBindingsData getKeyboardBindingsData() const;

    private:
        KeyboardBindingsData bindings;
    };

} // namespace mpc::input
