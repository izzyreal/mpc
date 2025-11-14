#pragma once

#include <type_traits>
#include <cmath>
#include <algorithm>

namespace mpc
{
    template <typename IntType, IntType Min, IntType Max> class ConstrainedInt
    {
        static_assert(std::is_integral_v<IntType>);
        static_assert(Min <= Max);

        IntType value;

        static constexpr IntType constrainedIntClamp(IntType v)
        {
            return std::clamp<IntType>(v, Min, Max);
        }

    public:
        template <
            typename OtherInt, OtherInt OtherMin, OtherInt OtherMax,
            typename = std::enable_if_t<std::is_integral_v<OtherInt> &&
                                        (OtherMin >= Min) && (OtherMax <= Max)>>
        constexpr ConstrainedInt(
            const ConstrainedInt<OtherInt, OtherMin, OtherMax> &other)
            : value(static_cast<IntType>(other.get()))
        {
        }

        explicit constexpr ConstrainedInt(IntType v)
            : value(constrainedIntClamp(v))
        {
        }

        constexpr ConstrainedInt() : value(Min) {}

        constexpr IntType get() const
        {
            return value;
        }
        constexpr operator IntType() const
        {
            return value;
        }

        constexpr ConstrainedInt operator+(IntType rhs) const
        {
            return ConstrainedInt(constrainedIntClamp(value + rhs));
        }

        constexpr ConstrainedInt operator-(IntType rhs) const
        {
            return ConstrainedInt(constrainedIntClamp(value - rhs));
        }

        template <typename T,
                  typename = std::enable_if_t<std::is_integral_v<T>>>
        constexpr ConstrainedInt operator+(T rhs) const
        {
            return ConstrainedInt(
                constrainedIntClamp(value + static_cast<IntType>(rhs)));
        }

        template <typename T,
                  typename = std::enable_if_t<std::is_integral_v<T>>>
        constexpr ConstrainedInt operator-(T rhs) const
        {
            return ConstrainedInt(
                constrainedIntClamp(value - static_cast<IntType>(rhs)));
        }

        constexpr ConstrainedInt operator*(const ConstrainedInt &rhs) const
        {
            return ConstrainedInt(constrainedIntClamp(value * rhs.value));
        }

        template <typename T,
                  typename = std::enable_if_t<std::is_floating_point_v<T>>>
        constexpr ConstrainedInt operator*(T rhs) const
        {
            using Wide = double;
            const Wide scaled =
                static_cast<Wide>(value) * static_cast<Wide>(rhs);
            const Wide rounded = std::round(scaled);
            Wide clamped = std::clamp(rounded, static_cast<Wide>(Min),
                                      static_cast<Wide>(Max));

            return ConstrainedInt(static_cast<IntType>(clamped));
        }
    };
} // namespace mpc
