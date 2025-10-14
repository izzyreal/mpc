#pragma once

#include <unordered_map>
#include <string>

namespace mpc::controller
{
class ButtonLockTracker {
    private:
        std::unordered_map<std::string, bool> states;

    public:
        void lock(const std::string& label)
        {
            states[label] = true;
        }

        bool isLocked(const std::string& label) const noexcept
        {
            if (auto it = states.find(label); it != states.end())
            {
                return it->second;
            }
            return false;
        }

        void unlock(const std::string& label) noexcept
        {
            states[label] = false;
        }

        void toggle(const std::string &label)
        {
            if (isLocked(label))
            {
                unlock(label);
            }
            else
            {
                lock(label);
            }
        }
    };
} // namespace mpc::controller
