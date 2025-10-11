#pragma once

#include <unordered_map>
#include <string>

/**
 * This class handles typing a name in the NameScreen via the pads (old mobile phone style).
 */

namespace mpc { class Mpc; }

namespace mpc::inputlogic {

    class PadAndButtonKeyboard
    {
    private:
        static std::unordered_map<std::string, std::string> charMap();

        mpc::Mpc& mpc;

        bool upperCase = true;

        std::unordered_map<std::string, bool> pressedZeroTimes;

        std::string previousPad;

    public:
        PadAndButtonKeyboard(mpc::Mpc&);
        void pressHardwareComponent(const std::string& label);
        void resetPreviousPad();
        void resetPressedZeroTimes();
        void resetUpperCase();

        const bool isUpperCase() { return upperCase; }

    };

}
