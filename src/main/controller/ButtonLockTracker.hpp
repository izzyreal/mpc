#pragma once

#include <unordered_map>

#include "hardware/ComponentId.hpp"

namespace mpc::controller
{
    class ButtonLockTracker
    {
    private:
        std::unordered_map<hardware::ComponentId, bool> states;

    public:
        void lock(const hardware::ComponentId id)
        {
            states[id] = true;
        }

        bool isLocked(const hardware::ComponentId id) const noexcept
        {
            if (auto it = states.find(id); it != states.end())
            {
                return it->second;
            }
            return false;
        }

        void unlock(const hardware::ComponentId id) noexcept
        {
            states[id] = false;
        }

        void toggle(const hardware::ComponentId id)
        {
            if (isLocked(id))
            {
                unlock(id);
            }
            else
            {
                lock(id);
            }
        }
    };
} // namespace mpc::controller
