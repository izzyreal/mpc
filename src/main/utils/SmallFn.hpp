#pragma once

#include <algorithm>
#include <cstdint>

namespace mpc::utils
{
    template <size_t MaxBytes, class Sig>
    struct SmallFn;

    template <size_t MaxBytes, class R, class... Args>
    struct SmallFn<MaxBytes, R(Args...)>
    {
        alignas(void*) unsigned char storage[MaxBytes];
        R (*invoke)(void*, Args...);

        SmallFn() = default;

        template<class F> explicit SmallFn(F f) { set(std::move(f)); }

        template<class F>
        void set(F f)
        {
            static_assert(sizeof(F) <= MaxBytes);
            new (storage) F(std::move(f));
            invoke = [](void* p, Args... as) -> R {
                return (*static_cast<F*>(p))(std::forward<Args>(as)...);
            };
        }

        R operator()(Args... as)
        {
            return invoke(storage, std::forward<Args>(as)...);
        }
    };
} // namespace mpc::utils