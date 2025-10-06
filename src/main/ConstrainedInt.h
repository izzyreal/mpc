#include <type_traits>
#include <stdexcept>

namespace mpc {
template<typename IntType, IntType Min, IntType Max>
class ConstrainedInt {
    static_assert(std::is_integral_v<IntType>);
    static_assert(Min <= Max);

    IntType value;

public:
    constexpr ConstrainedInt(IntType v) : value(v) {
        if (v < Min || v > Max)
            throw std::out_of_range("Value out of range");
    }

    constexpr IntType get() const { return value; }
    constexpr operator IntType() const { return value; }
};
}
