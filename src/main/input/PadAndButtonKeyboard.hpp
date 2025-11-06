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
    private:
        static std::unordered_map<mpc::hardware::ComponentId, std::string>
        charMap();

        mpc::Mpc &mpc;

        bool upperCase = true;

        std::unordered_map<mpc::hardware::ComponentId, bool> pressedZeroTimes;

        mpc::hardware::ComponentId previousPad =
            mpc::hardware::ComponentId::NONE;

    public:
        PadAndButtonKeyboard(mpc::Mpc &);
        void pressHardwareComponent(const mpc::hardware::ComponentId);
        void resetPreviousPad();
        void resetPressedZeroTimes();
        void resetUpperCase();

        const bool isUpperCase() const
        {
            return upperCase;
        }
    };

} // namespace mpc::input
