#pragma once

#include <functional>
#include <utility>
#include <type_traits>

namespace mpc::lcdgui
{
    struct ReactiveBinding
    {
        std::function<void()> refreshFn;

        ReactiveBinding() = default;

        template <
            typename Getter,
            typename Updater,
            typename T = std::invoke_result_t<Getter>,
            std::enable_if_t<
                std::is_invocable_r_v<T, Getter> &&
                    std::is_invocable_v<Updater, const T &>,
                int> = 0>
        ReactiveBinding(Getter &&getter, Updater &&updater)
        {
            T lastValue{};

            refreshFn = [get = std::forward<Getter>(getter),
                         upd = std::forward<Updater>(updater),
                         prev = std::move(lastValue)]() mutable
            {
                const T current = get();
                if (current != prev)
                {
                    prev = current;
                    upd(current);
                }
            };
        }

        void refresh() const
        {
            if (refreshFn)
            {
                refreshFn();
            }
        }
    };
} // namespace mpc::lcdgui
