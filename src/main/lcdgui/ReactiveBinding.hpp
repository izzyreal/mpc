#pragma once

#include <functional>
#include <utility>
#include <type_traits>
#include <optional>

namespace mpc::lcdgui
{
    struct ReactiveBinding
    {
        std::function<void()> refreshFn;

        ReactiveBinding() = default;

        template <
            typename Getter, typename Updater,
            typename Comparator = std::equal_to<std::invoke_result_t<Getter>>,
            typename T = std::invoke_result_t<Getter>,
            std::enable_if_t<std::is_invocable_r_v<T, Getter> &&
                                 std::is_invocable_v<Updater, const T &> &&
                                 std::is_invocable_r_v<bool, Comparator,
                                                       const T &, const T &>,
                             int> = 0>
        ReactiveBinding(Getter &&getter, Updater &&updater,
                        Comparator comp = {})
        {
            refreshFn = [get = std::forward<Getter>(getter),
                         upd = std::forward<Updater>(updater),
                         cmp = std::move(comp),
                         prev = std::optional<T>()]() mutable
            {
                const T current = get();
                if (!prev.has_value() || !cmp(current, *prev))
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
