#pragma once

#include <unordered_map>
#include <string>

#include "hardware/ComponentId.hpp"

/**
 * This class handles typing a name in the NameScreen via the pads (old mobile
 * phone style).
 */

namespace mpc
{
    class Mpc;
}

namespace mpc::input
{

    class PadAndButtonKeyboard
    {
        static std::unordered_map<hardware::ComponentId, std::string> charMap();

        Mpc &mpc;

        bool upperCase = true;

        std::unordered_map<hardware::ComponentId, bool> pressedZeroTimes;

        hardware::ComponentId previousPad = hardware::ComponentId::NONE;

    public:
        PadAndButtonKeyboard(Mpc &);
        void pressHardwareComponent(const hardware::ComponentId);
        void resetPreviousPad();
        void resetPressedZeroTimes();
        void resetUpperCase();

        const bool isUpperCase() const
        {
            return upperCase;
        }
    };

} // namespace mpc::input
