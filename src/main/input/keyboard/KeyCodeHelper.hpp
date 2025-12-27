#pragma once

#include "input/keyboard/VmpcKeyCode.hpp"

#include <string>
#include <map>
#include <optional>

namespace mpc::input::keyboard
{
    class KeyCodeHelper
    {
    public:
        static VmpcKeyCode getVmpcFromPlatformKeyCode(int platformKeyCode);

        static std::optional<char>
        getCharForTypableVmpcKeyCode(VmpcKeyCode vmpcKeyCode);

        static std::string
        guessCharactersPrintedOnKeyUnicode(VmpcKeyCode vmpcKeyCode);

        static std::string
        getAsciiCompatibleDisplayName(VmpcKeyCode vmpcKeyCode);

        static int getPlatformFromVmpcKeyCode(VmpcKeyCode vmpcKeyCode);

        static std::optional<char>
        getCharWithShiftModifier(VmpcKeyCode vmpcKeyCode);

    private:
        static const std::map<const int, const VmpcKeyCode>
            platformToVmpcKeyCodes;

        static const std::map<const VmpcKeyCode, const char>
            typableVmpcKeyCodes;

        static std::string getKeyCodeString(VmpcKeyCode keyCode);
    };
} // namespace mpc::input::keyboard
