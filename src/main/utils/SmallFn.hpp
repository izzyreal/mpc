#pragma once

#include <algorithm>
#include <cstdint>

namespace mpc::utils
{
    template <size_t MaxBytes, class Sig> struct SmallFn;

    template <size_t MaxBytes, class R, class... Args>
    struct SmallFn<MaxBytes, R(Args...)>
    {
        alignas(void *) unsigned char storage[MaxBytes];
        R (*invoke)(void *, Args...);

        SmallFn() : storage{}, invoke(nullptr) {}

        template <class F> explicit SmallFn(F f)
        {
            set(std::move(f));
        }

        template <class F> void set(F f)
        {
            static_assert(sizeof(F) <= MaxBytes);
            new (storage) F(std::move(f));
            invoke = [](void *p, Args... as) -> R
            {
                return (*static_cast<F *>(p))(std::forward<Args>(as)...);
            };
        }

        R operator()(Args... as)
        {
            if (!invoke)
            {
                return R();
            }
            return invoke(storage, std::forward<Args>(as)...);
        }
    };

    using Task = SmallFn<96, void()>;
    using PostToUiThreadFn = SmallFn<16, void(Task &&)>;
    using PostToAudioThreadFn = SmallFn<16, void(Task &&)>;
} // namespace mpc::utils