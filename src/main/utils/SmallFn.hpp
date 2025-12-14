#pragma once

#include <algorithm>
#include <cstddef>
#include <cassert>

namespace mpc::utils
{
    template <size_t MaxBytes, class Sig> struct SmallFn;

    template <size_t MaxBytes, class R, class... Args>
    struct SmallFn<MaxBytes, R(Args...)>
    {
        alignas(void *) unsigned char storage[MaxBytes];

        R (*invoke)(const void *, Args...);
        void (*moveFn)(void *dst, void *src);
        void (*destroyFn)(void *);

        SmallFn()
            : storage{}, invoke(nullptr), moveFn(nullptr), destroyFn(nullptr)
        {
        }

        template <class F> explicit SmallFn(F f)
        {
            set(std::move(f));
        }

        template <class F> void set(F f)
        {
            static_assert(sizeof(F) <= MaxBytes);

            new (storage) F(std::move(f));

            invoke = [](const void *p, Args... as) -> R
            {
                return (*static_cast<const F *>(p))(std::forward<Args>(as)...);
            };

            moveFn = [](void *dst, void *src)
            {
                F *srcF = static_cast<F *>(src);
                new (dst) F(std::move(*srcF));
                srcF->~F();
            };

            destroyFn = [](void *p)
            {
                static_cast<F *>(p)->~F();
            };
        }

        ~SmallFn()
        {
            reset();
        }

        void reset()
        {
            if (destroyFn)
            {
                destroyFn(storage);
            }

            invoke = nullptr;
            moveFn = nullptr;
            destroyFn = nullptr;
        }

        // -------- MOVE ONLY --------
        SmallFn(const SmallFn &) = delete;
        SmallFn &operator=(const SmallFn &) = delete;

        SmallFn(SmallFn &&other) noexcept
        {
            invoke = other.invoke;
            moveFn = other.moveFn;
            destroyFn = other.destroyFn;

            if (moveFn)
            {
                moveFn(storage, other.storage);
            }

            other.invoke = nullptr;
            other.moveFn = nullptr;
            other.destroyFn = nullptr;
        }

        SmallFn &operator=(SmallFn &&other) noexcept
        {
            if (this != &other)
            {
                reset();

                invoke = other.invoke;
                moveFn = other.moveFn;
                destroyFn = other.destroyFn;

                if (moveFn)
                {
                    moveFn(storage, other.storage);
                }

                other.invoke = nullptr;
                other.moveFn = nullptr;
                other.destroyFn = nullptr;
            }
            return *this;
        }
        // ----------------------------

        template <class T = R, class = std::enable_if_t<std::is_void_v<T> &&
                                                        sizeof...(Args) == 0>>
        void operator()()
        {
            if (invoke)
            {
                invoke(storage);
            }
        }

        template <class T = R, class = std::enable_if_t<!std::is_void_v<T> ||
                                                        sizeof...(Args) != 0>>
        R operator()(Args... as) const
        {
            assert(invoke);
            return invoke(storage, std::forward<Args>(as)...);
        }
    };

    using Task = SmallFn<128, void()>;
    using PostToUiThreadFn = SmallFn<16, void(Task &&)>;
    using PostToAudioThreadFn = SmallFn<16, void(Task &&)>;
} // namespace mpc::utils
