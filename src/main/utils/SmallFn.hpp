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
        alignas(void*) unsigned char storage[MaxBytes];

        R   (*invoke)(const void*, Args...);
        void (*copyFn)(void* dst, const void* src);
        void (*moveFn)(void* dst, void* src);
        void (*destroyFn)(void*);

        SmallFn() :
            storage{},
            invoke(nullptr),
            copyFn(nullptr),
            moveFn(nullptr),
            destroyFn(nullptr)
        {}

        template <class F>
        explicit SmallFn(F f)
        {
            set(std::move(f));
        }

        template <class F>
        void set(F f)
        {
            static_assert(sizeof(F) <= MaxBytes);

            new (storage) F(std::move(f));

            invoke = [](const void* p, Args... as) -> R {
                return (*static_cast<const F*>(p))(std::forward<Args>(as)...);
            };

            static_assert(std::is_copy_constructible_v<F>,
                          "Stored functor is not copyable");

            copyFn = [](void* dst, const void* src) {
                new (dst) F(*static_cast<const F*>(src));
            };

            moveFn = [](void* dst, void* src) {
                F* srcF = static_cast<F*>(src);
                new (dst) F(std::move(*srcF));
                srcF->~F();
            };

            destroyFn = [](void* p) {
                static_cast<F*>(p)->~F();
            };
        }

        ~SmallFn()
        {
            reset();
        }

        void reset()
        {
            if (destroyFn)
                destroyFn(storage);

            invoke    = nullptr;
            copyFn    = nullptr;
            moveFn    = nullptr;
            destroyFn = nullptr;
        }

        SmallFn(const SmallFn& other)
        {
            invoke    = other.invoke;
            copyFn    = other.copyFn;
            moveFn    = other.moveFn;
            destroyFn = other.destroyFn;

            if (copyFn)
                copyFn(storage, other.storage);
        }

        SmallFn& operator=(const SmallFn& other)
        {
            if (this != &other)
            {
                reset();

                invoke    = other.invoke;
                copyFn    = other.copyFn;
                moveFn    = other.moveFn;
                destroyFn = other.destroyFn;

                if (copyFn)
                    copyFn(storage, other.storage);
            }
            return *this;
        }

        SmallFn(SmallFn&& other) noexcept
        {
            invoke    = other.invoke;
            copyFn    = other.copyFn;
            moveFn    = other.moveFn;
            destroyFn = other.destroyFn;

            if (moveFn)
                moveFn(storage, other.storage);

            other.invoke    = nullptr;
            other.copyFn    = nullptr;
            other.moveFn    = nullptr;
            other.destroyFn = nullptr;
        }

        SmallFn& operator=(SmallFn&& other) noexcept
        {
            if (this != &other)
            {
                reset();

                invoke    = other.invoke;
                copyFn    = other.copyFn;
                moveFn    = other.moveFn;
                destroyFn = other.destroyFn;

                if (moveFn)
                    moveFn(storage, other.storage);

                other.invoke    = nullptr;
                other.copyFn    = nullptr;
                other.moveFn    = nullptr;
                other.destroyFn = nullptr;
            }
            return *this;
        }

        R operator()(Args... as)
        {
            assert(invoke);
            return invoke(storage, std::forward<Args>(as)...);
        }

        R operator()(Args... as) const
        {
            assert(invoke);
            return invoke(storage, std::forward<Args>(as)...);
        }

        template <class T = R>
        std::enable_if_t<std::is_void_v<T>, void>
        operator()()
        {
            if (invoke)
                invoke(storage);
        }
    };

    using Task = SmallFn<96, void()>;
    using PostToUiThreadFn = SmallFn<16, void(Task&&)>;
    using PostToAudioThreadFn = SmallFn<16, void(Task&&)>;
} // namespace mpc::utils
