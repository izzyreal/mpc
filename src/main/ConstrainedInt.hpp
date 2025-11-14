#pragma once

#include <type_traits>
#include <stdexcept>
#include <cmath>
#include <algorithm>

namespace mpc
{
    template <typename IntType, IntType Min, IntType Max> class ConstrainedInt
    {
        static_assert(std::is_integral_v<IntType>);
        static_assert(Min <= Max);

        IntType value;

    public:
        template <typename OtherInt, OtherInt OtherMin, OtherInt OtherMax,
                  typename = std::enable_if_t<
                      // only allow conversion FROM another ConstrainedInt…
                      std::is_integral_v<OtherInt> &&
                      // …whose entire range fits inside ours
                      (OtherMin >= Min) && (OtherMax <= Max)>>
        constexpr ConstrainedInt(
            const ConstrainedInt<OtherInt, OtherMin, OtherMax> &other)
            : value(static_cast<IntType>(other.get()))
        {
        }

        explicit constexpr ConstrainedInt(IntType v) : value(v)
        {
            if (v < Min || v > Max)
            {
                throw std::out_of_range("Value out of range");
            }
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

        static constexpr ConstrainedInt fromUnchecked(IntType v)
        {
            return ConstrainedInt(v);
        }

        constexpr ConstrainedInt operator+(IntType rhs) const
        {
            return fromUnchecked(value + rhs);
        }

        constexpr ConstrainedInt operator-(IntType rhs) const
        {
            return fromUnchecked(value - rhs);
        }

        template <typename T,
                  typename = std::enable_if_t<std::is_integral_v<T>>>
        constexpr ConstrainedInt operator+(T rhs) const
        {
            IntType v = value + static_cast<IntType>(rhs);
            return ConstrainedInt(v);
        }

        template <typename T,
                  typename = std::enable_if_t<std::is_integral_v<T>>>
        constexpr ConstrainedInt operator-(T rhs) const
        {
            IntType v = value - static_cast<IntType>(rhs);
            return ConstrainedInt(v);
        }

        constexpr ConstrainedInt operator*(const ConstrainedInt &rhs) const
        {
            IntType v = value * rhs.value;
            return ConstrainedInt(v);
        }

        // 2. Floating point multiply — only enabled for true floating point
        // types
        template <typename T,
                  typename = std::enable_if_t<std::is_floating_point_v<T>>>
        constexpr ConstrainedInt operator*(T rhs) const
        {
            using Wide = double;

            Wide scaled = static_cast<Wide>(value) * static_cast<Wide>(rhs);
            Wide rounded = std::round(scaled);
            Wide clamped = std::clamp(rounded, static_cast<Wide>(Min),
                                      static_cast<Wide>(Max));

            return ConstrainedInt(static_cast<IntType>(clamped));
        }
    };
} // namespace mpc
