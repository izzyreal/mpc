#pragma once

#include "IntTypes.hpp"

#include <type_traits>

namespace mpc::controller
{
    enum class Bank
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
        return static_cast<Bank>(programPadIndex / 16);
    }

    constexpr ProgramPadIndex
    physicalPadAndBankToProgramPadIndex(const PhysicalPadIndex physicalPadIndex,
                                        Bank bank) noexcept
    {
        using T = std::underlying_type_t<Bank>;
        return static_cast<T>(bank) * 16 + physicalPadIndex;
    }
} // namespace mpc::controller
