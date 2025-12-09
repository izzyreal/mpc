#pragma once

#include "MpcSpecs.hpp"

#include <cstdint>
#include <string>
#include <cassert>
#include <algorithm>

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

    inline std::string busTypeToString(const BusType busType) noexcept
    {
        switch (busType)
        {
            case BusType::MIDI:
                return "MIDI";
            case BusType::DRUM1:
                return "DRUM1";
            case BusType::DRUM2:
                return "DRUM2";
            case BusType::DRUM3:
                return "DRUM3";
            case BusType::DRUM4:
                return "DRUM4";
            default:
                return "Unknown";
        }
    }

    inline bool isDrumBusType(const BusType busType) noexcept
    {
        return busType == BusType::DRUM1 || busType == BusType::DRUM2 ||
               busType == BusType::DRUM3 || busType == BusType::DRUM4;
    }

    inline bool isMidiBusType(const BusType busType) noexcept
    {
        return busType == BusType::MIDI;
    }

    inline size_t busTypeToIndex(const BusType busType) noexcept
    {
        const auto result = static_cast<size_t>(busType);
        assert(result <= mpc::Mpc2000XlSpecs::LAST_BUS_INDEX);
        return result;
    }

    inline size_t drumBusTypeToDrumIndex(const BusType busType) noexcept
    {
        assert(isDrumBusType(busType));
        return busTypeToIndex(busType) - 1;
    }

    inline BusType busIndexToBusType(const size_t index) noexcept
    {
        assert(index <= mpc::Mpc2000XlSpecs::LAST_BUS_INDEX);
        return static_cast<BusType>(index);
    }

    inline BusType drumBusIndexToDrumBusType(const size_t index) noexcept
    {
        assert(index <= mpc::Mpc2000XlSpecs::DRUM_BUS_COUNT - 1);
        return static_cast<BusType>(index + 1);
    }

    constexpr BusType operator+(BusType lhs, int rhs) noexcept
    {
        using S = int;
        constexpr S MIN = static_cast<S>(BusType::MIDI);
        constexpr S MAX = static_cast<S>(BusType::DRUM4);

        S v = static_cast<S>(lhs) + rhs;
        v = std::clamp(v, MIN, MAX);

        return static_cast<BusType>(v);
    }

    constexpr BusType operator-(BusType lhs, int rhs) noexcept
    {
        using S = int;
        constexpr S MIN = static_cast<S>(BusType::MIDI);
        constexpr S MAX = static_cast<S>(BusType::DRUM4);

        S v = static_cast<S>(lhs) - rhs;
        v = std::clamp(v, MIN, MAX);

        return static_cast<BusType>(v);
    }

    constexpr BusType operator+(int lhs, BusType rhs) noexcept
    {
        using S = int;

        constexpr S MIN = static_cast<S>(BusType::MIDI);
        constexpr S MAX = static_cast<S>(BusType::DRUM4);

        S v = lhs + static_cast<S>(rhs);
        v = std::clamp(v, MIN, MAX);

        return static_cast<BusType>(v);
    }
} // namespace mpc::sequencer
