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

    inline bool operator<(const KeyBinding& lhs, const KeyBinding& rhs)
    {
        return std::tie(lhs.keyCode, lhs.componentLabel, lhs.direction) <
               std::tie(rhs.keyCode, rhs.componentLabel, rhs.direction);
    }

    using KeyboardBindingsData = std::vector<KeyBinding>;

    class KeyboardBindings
    {
    public:
        KeyboardBindings();

        explicit KeyboardBindings(const KeyboardBindingsData &);

        bool isSameAs(const KeyboardBindingsData& other) const;

        bool hasNoDuplicateVmpcKeyCodes() const;

        bool hasNoDuplicateKeyBindings() const;

        bool hasNoDuplicates() const;

        std::vector<VmpcKeyCode>
            lookupComponentKeyCodes(hardware::ComponentId) const;

        std::vector<KeyBinding *> lookupKeyCodeBindings(VmpcKeyCode);
        KeyBinding *lookupFirstKeyCodeBinding(VmpcKeyCode);

        void setBinding(const std::string &componentLabel, Direction,
                        VmpcKeyCode);

        void initializeDefaults();

        const KeyboardBindingsData &getKeyboardBindingsData() const;

        KeyBinding *getByIndex(int);

        void setBindingsData(const KeyboardBindingsData &);

        int getBindingCount() const;

    private:
        KeyboardBindingsData bindings;

        std::vector<VmpcKeyCode>
        lookupComponentKeyCodes(const std::string &) const;
        std::vector<KeyBinding *> lookupComponentBindings(const std::string &);
    };

} // namespace mpc::input
