#pragma once

#include <cstdint>
#include <type_traits>

namespace mpc::sequencer
{
    enum class BusType : uint8_t
    {
        MIDI = 0,
        DRUM1 = 1,
        DRUM2 = 2,
        DRUM3 = 3,
        DRUM4 = 4
    };

    constexpr BusType operator+(BusType lhs, int rhs) noexcept
    {
        using T = std::underlying_type_t<BusType>;
        return static_cast<BusType>(static_cast<T>(lhs) + rhs);
    }

    constexpr BusType operator+(int lhs, BusType rhs) noexcept
    {
        using T = std::underlying_type_t<BusType>;
        return static_cast<BusType>(lhs + static_cast<T>(rhs));
    }
} // namespace mpc::sequencer
