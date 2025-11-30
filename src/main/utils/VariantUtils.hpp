#pragma once

#include <variant>

namespace mpc
{
    template <class... Ts> struct Overload : Ts...
    {
        using Ts::operator()...;
    };

    template <class... Ts> Overload(Ts...) -> Overload<Ts...>;

    template <typename V, typename... Ts>
    bool isVariantAnyOf(const V &v, std::variant<Ts...> const &)
    {
        return std::visit(
            [](auto const &x)
            {
                using X = std::decay_t<decltype(x)>;
                return ((std::is_same_v<X, Ts>) || ...);
            },
            v);
    }
} // namespace mpc