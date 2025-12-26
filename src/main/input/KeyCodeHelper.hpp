#pragma once

#include <string>
#include <map>
#include <optional>

namespace mpc::input
{
    /**
     * This class enumerates all keys that can be mapped to actions.
     *
     * It's an amalgamation of the most common virtual keycodes of
     * all supported platforms.
     *
     * Note that on Windows, there are 3 types of modifier keys:
     *
     * 1. Left shift/alt/ctrl.
     * 2. Right shift/alt/ctrl.
     * 3. Generic shift/alt/ctrl.
     *
     * All 3 actually have distinct keycodes.
     *
     * On the other supported platforms (Linux, macOS and iOS) there
     * exists no "generic" variety.
     *
     * The "meta" key (a.k.a. Windows or Command key) only comes in
     * left and right flavors on all platforms.
     *
     * Linux has left and right "super" keys that are typically mapped
     * to the same physical keys as left and right "meta", but they
     * don't have to be.
     *
     * Moreover, on macOS and iOS, the identifiers for "left"
     * varieties of modifier keys as well as the Command key, do not
     * explicitly have the word "left" in them. Keep this in mind
     * when reading __APPLE__ specific code that deals with virtual
     * keycodes.
     */
    enum class VmpcKeyCode
    {
        VMPC_KEY_UNKNOWN = -1,

        VMPC_KEY_ANSI_0,
        VMPC_KEY_ANSI_1,
        VMPC_KEY_ANSI_2,
        VMPC_KEY_ANSI_3,
        VMPC_KEY_ANSI_4,
        VMPC_KEY_ANSI_5,
        VMPC_KEY_ANSI_6,
        VMPC_KEY_ANSI_7,
        VMPC_KEY_ANSI_8,
        VMPC_KEY_ANSI_9,

        VMPC_KEY_ANSI_A,
        VMPC_KEY_ANSI_B,
        VMPC_KEY_ANSI_C,
        VMPC_KEY_ANSI_D,
        VMPC_KEY_ANSI_E,
        VMPC_KEY_ANSI_F,
        VMPC_KEY_ANSI_G,
        VMPC_KEY_ANSI_H,
        VMPC_KEY_ANSI_I,
        VMPC_KEY_ANSI_J,
        VMPC_KEY_ANSI_K,
        VMPC_KEY_ANSI_L,
        VMPC_KEY_ANSI_M,
        VMPC_KEY_ANSI_N,
        VMPC_KEY_ANSI_O,
        VMPC_KEY_ANSI_P,
        VMPC_KEY_ANSI_Q,
        VMPC_KEY_ANSI_R,
        VMPC_KEY_ANSI_S,
        VMPC_KEY_ANSI_T,
        VMPC_KEY_ANSI_U,
        VMPC_KEY_ANSI_V,
        VMPC_KEY_ANSI_W,
        VMPC_KEY_ANSI_X,
        VMPC_KEY_ANSI_Y,
        VMPC_KEY_ANSI_Z,

        VMPC_KEY_LeftArrow,
        VMPC_KEY_RightArrow,
        VMPC_KEY_UpArrow,
        VMPC_KEY_DownArrow,

        VMPC_KEY_F1,
        VMPC_KEY_F2,
        VMPC_KEY_F3,
        VMPC_KEY_F4,
        VMPC_KEY_F5,
        VMPC_KEY_F6,
        VMPC_KEY_F7,
        VMPC_KEY_F8,
        VMPC_KEY_F9,
        VMPC_KEY_F10,
        VMPC_KEY_F11,
        VMPC_KEY_F12,

        VMPC_KEY_Escape,
        VMPC_KEY_ANSI_Grave,
        VMPC_KEY_ISO_Section,
        VMPC_KEY_Tab,
        VMPC_KEY_CapsLock,
        VMPC_KEY_Shift,
        VMPC_KEY_LeftShift,
        VMPC_KEY_RightShift,
        VMPC_KEY_Control,
        VMPC_KEY_LeftControl,
        VMPC_KEY_RightControl,
        VMPC_KEY_OptionOrAlt,
        VMPC_KEY_LeftOptionOrAlt,
        VMPC_KEY_RightOptionOrAlt,
        VMPC_KEY_LeftMeta,
        VMPC_KEY_RightMeta,
        VMPC_KEY_Function,
        VMPC_KEY_ANSI_Minus,
        VMPC_KEY_ANSI_Equal,
        VMPC_KEY_Delete,
        VMPC_KEY_Insert,
        VMPC_KEY_Backspace,
        VMPC_KEY_ANSI_LeftBracket,
        VMPC_KEY_ANSI_RightBracket,
        VMPC_KEY_ANSI_Backslash,
        VMPC_KEY_ANSI_Semicolon,
        VMPC_KEY_ANSI_Quote,
        VMPC_KEY_Return,
        VMPC_KEY_ANSI_Comma,
        VMPC_KEY_ANSI_Period,
        VMPC_KEY_ANSI_Slash,
        VMPC_KEY_Space,

        VMPC_KEY_Home,
        VMPC_KEY_PageUp,
        VMPC_KEY_End,
        VMPC_KEY_PageDown,

        VMPC_KEY_ANSI_Keypad0,
        VMPC_KEY_ANSI_Keypad1,
        VMPC_KEY_ANSI_Keypad2,
        VMPC_KEY_ANSI_Keypad3,
        VMPC_KEY_ANSI_Keypad4,
        VMPC_KEY_ANSI_Keypad5,
        VMPC_KEY_ANSI_Keypad6,
        VMPC_KEY_ANSI_Keypad7,
        VMPC_KEY_ANSI_Keypad8,
        VMPC_KEY_ANSI_Keypad9,

        VMPC_KEY_ANSI_KeypadPeriodOrDelete,
        VMPC_KEY_ANSI_KeypadMultiply,
        VMPC_KEY_ANSI_KeypadPlus,
        VMPC_KEY_ANSI_KeypadNumLockOrClear,
        VMPC_KEY_ANSI_KeypadDivide,
        VMPC_KEY_ANSI_KeypadEnter,
        VMPC_KEY_ANSI_KeypadMinus,
        VMPC_KEY_ANSI_KeypadEquals,
        VMPC_KEY_ANSI_KeypadComma,

        VMPC_KEY_ENUMERATOR_SIZE
    };

    class KeyCodeHelper
    {
    public:
        static VmpcKeyCode
        getVmpcFromPlatformKeyCode(const int platformKeyCode);

        static std::optional<char>
        getCharForTypableVmpcKeyCode(const VmpcKeyCode vmpcKeyCode);

        static std::string
        guessCharactersPrintedOnKeyUnicode(const VmpcKeyCode vmpcKeyCode);

        static std::string
        getAsciiCompatibleDisplayName(const VmpcKeyCode vmpcKeyCode);

        static int getPlatformFromVmpcKeyCode(const VmpcKeyCode vmpcKeyCode);

        static std::optional<char>
        getCharWithShiftModifier(const VmpcKeyCode vmpcKeyCode);

    private:
        static const std::map<const int, const VmpcKeyCode>
            platformToVmpcKeyCodes;

        static const std::map<const VmpcKeyCode, const char>
            typableVmpcKeyCodes;

        static std::string getKeyCodeString(const VmpcKeyCode keyCode);
    };
} // namespace mpc::input
