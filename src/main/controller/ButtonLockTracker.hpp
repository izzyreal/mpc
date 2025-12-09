#pragma once

#include <unordered_map>
#include <vector>
#include <atomic>

#include "hardware/ComponentId.hpp"

namespace mpc::controller
{
    class ButtonLockTracker
    {
        std::unordered_map<hardware::ComponentId, std::atomic<bool>> states;

    public:
        ButtonLockTracker()
        {
            initializeEntries({hardware::REC, hardware::OVERDUB,
                               hardware::TAP_TEMPO_OR_NOTE_REPEAT});
        }

        void lock(const hardware::ComponentId id)
        {
            states.at(id).store(true);
        }

        bool isLocked(const hardware::ComponentId id) const noexcept
        {
            return states.at(id).load();
        }

        void unlock(const hardware::ComponentId id) noexcept
        {
            states.at(id).store(false);
        }

        void toggle(const hardware::ComponentId id)
        {
            auto &flag = states.at(id);
            bool expected = flag.load(std::memory_order_relaxed);
            for (;;)
            {
                if (const bool desired = !expected; flag.compare_exchange_weak(
                        expected, desired, std::memory_order_acq_rel,
                        std::memory_order_relaxed))
                {
                    return;
                }
            }
        }

    private:
        void initializeEntries(const std::vector<hardware::ComponentId> &&ids)
        {
            for (auto id : ids)
            {
                states.emplace(id, false);
            }
        }
    };
} // namespace mpc::controller
