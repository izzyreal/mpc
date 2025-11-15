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

        static constexpr IntType constrainedIntClampWide(long long v)
        {
            long long clamped = std::clamp(v, static_cast<long long>(Min),
                                           static_cast<long long>(Max));
            return static_cast<IntType>(clamped);
        }

    public:
        // Cross-type constrained conversion
        template <
            typename OtherInt, OtherInt OtherMin, OtherInt OtherMax,
            typename = std::enable_if_t<std::is_integral_v<OtherInt> &&
                                        (OtherMin >= Min) && (OtherMax <= Max)>>
        constexpr ConstrainedInt(
            const ConstrainedInt<OtherInt, OtherMin, OtherMax> &other)
            : value(static_cast<IntType>(other.get()))
        {
        }

        // Construction from raw IntType
        explicit constexpr ConstrainedInt(IntType v)
            : value(std::clamp(v, Min, Max))
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

        // *** Corrected arithmetic (no wrap-around) ***

        constexpr ConstrainedInt operator+(IntType rhs) const
        {
            long long raw =
                static_cast<long long>(value) + static_cast<long long>(rhs);
            return ConstrainedInt(constrainedIntClampWide(raw));
        }

        constexpr ConstrainedInt operator-(IntType rhs) const
        {
            long long raw =
                static_cast<long long>(value) - static_cast<long long>(rhs);
            return ConstrainedInt(constrainedIntClampWide(raw));
        }

        template <typename T,
                  typename = std::enable_if_t<std::is_integral_v<T>>>
        constexpr ConstrainedInt operator+(T rhs) const
        {
            long long raw =
                static_cast<long long>(value) + static_cast<long long>(rhs);
            return ConstrainedInt(constrainedIntClampWide(raw));
        }

        template <typename T,
                  typename = std::enable_if_t<std::is_integral_v<T>>>
        constexpr ConstrainedInt operator-(T rhs) const
        {
            long long raw =
                static_cast<long long>(value) - static_cast<long long>(rhs);
            return ConstrainedInt(constrainedIntClampWide(raw));
        }

        constexpr ConstrainedInt operator*(const ConstrainedInt &rhs) const
        {
            long long raw = static_cast<long long>(value) *
                            static_cast<long long>(rhs.value);
            return ConstrainedInt(constrainedIntClampWide(raw));
        }

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
