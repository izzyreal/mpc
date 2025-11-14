#pragma once

#include "IntTypes.hpp"

#include <type_traits>
#include <cstdint>

namespace mpc::controller
{
    enum class Bank : int8_t
    {
        A = 0,
        B = 1,
        C = 2,
        D = 3
    };

    constexpr Bank operator+(Bank lhs, const int rhs) noexcept
    {
        using T = std::underlying_type_t<Bank>;
        return static_cast<Bank>(static_cast<T>(lhs) + rhs);
    }

    constexpr Bank operator+(const int lhs, Bank rhs) noexcept
    {
        using T = std::underlying_type_t<Bank>;
        return static_cast<Bank>(lhs + static_cast<T>(rhs));
    }

    constexpr Bank operator*(Bank lhs, const int rhs) noexcept
    {
        using T = std::underlying_type_t<Bank>;
        return static_cast<Bank>(static_cast<T>(lhs) * rhs);
    }

    constexpr Bank
    programPadIndexToBank(const ProgramPadIndex programPadIndex) noexcept
    {
        if (programPadIndex.get() < 16)
        {
            return Bank::A;
        }
        if (programPadIndex.get() < 32)
        {
            return Bank::B;
        }
        if (programPadIndex.get() < 48)
        {
            return Bank::C;
        }
        return Bank::D;
    }

    constexpr ProgramPadIndex
    physicalPadAndBankToProgramPadIndex(const PhysicalPadIndex physicalPadIndex,
                                        Bank bank) noexcept
    {
        using T = std::underlying_type_t<Bank>;
        return ProgramPadIndex(static_cast<T>(bank) *
                                   Mpc2000XlSpecs::PADS_PER_BANK_COUNT +
                               physicalPadIndex);
    }
} // namespace mpc::controller
