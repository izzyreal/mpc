#pragma once
#include <string>
#include <typeinfo>
#include <variant>

#if defined(__APPLE__)
#include <cxxabi.h>
#endif

namespace mpc::utils {

    inline std::string demangle(const char* name) {
#if defined(__APPLE__)
        int st = 0;
        char* dem = abi::__cxa_demangle(name, nullptr, nullptr, &st);
        if (st == 0 && dem) {
            std::string out = dem;
            free(dem);
            return out;
        }
#endif
        return name;
    }

    // --- trait to detect variant ---
    template<typename T>
    struct is_variant : std::false_type {};

    template<typename... Ts>
    struct is_variant<std::variant<Ts...>> : std::true_type {};

    // --- primary typeAsString ---
    template<typename T>
    std::string typeAsString(const T& v);

    // --- variant overload (recurses on nested variants) ---
    template<typename... Ts>
    std::string typeAsString(const std::variant<Ts...>& v) {
        return std::visit(
            [](auto&& x) -> std::string {
                using X = std::decay_t<decltype(x)>;
                if constexpr (is_variant<X>::value) {
                    return typeAsString(x);       // recurse
                } else {
                    return demangle(typeid(x).name());
                }
            },
            v
        );
    }

    // --- fallback for plain types ---
    template<typename T>
    std::string typeAsString(const T& v) {
        return demangle(typeid(v).name());
    }

} // namespace mpc::utils
