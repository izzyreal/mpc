#include "KeyCodeHelper.hpp"

#include "KeyboardLayout.hpp"

#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IOS
#include "controls/UIKeyConstants.hpp"
#else
#include <Carbon/Carbon.h>
#endif
#elif defined _WIN32
#include <windows.h>
#include <WinUser.h>
#elif defined __linux__
#include <X11/keysym.h>
#endif

using namespace mpc::controls;

const std::map<const int, const VmpcKeyCode> KeyCodeHelper::platformToVmpcKeyCodes {

#if defined(__APPLE__) && !TARGET_OS_IOS
    { kVK_Delete, VmpcKeyCode::VMPC_KEY_Backspace },
    { kVK_F1, VmpcKeyCode::VMPC_KEY_F1 },
    { kVK_F2, VmpcKeyCode::VMPC_KEY_F2 },
    { kVK_F3, VmpcKeyCode::VMPC_KEY_F3 },
    { kVK_F4, VmpcKeyCode::VMPC_KEY_F4 },
    { kVK_F5 , VmpcKeyCode::VMPC_KEY_F5 },
    { kVK_F6 , VmpcKeyCode::VMPC_KEY_F6 },
    { kVK_F7 , VmpcKeyCode::VMPC_KEY_F7 },
    { kVK_F8, VmpcKeyCode::VMPC_KEY_F8 },
    { kVK_F9, VmpcKeyCode::VMPC_KEY_F9 },
    { kVK_F10, VmpcKeyCode::VMPC_KEY_F10 },
    { kVK_F11, VmpcKeyCode::VMPC_KEY_F11 },
    { kVK_F12, VmpcKeyCode::VMPC_KEY_F12 },

    { kVK_Escape, VmpcKeyCode::VMPC_KEY_Escape },
    { kVK_ANSI_Grave, VmpcKeyCode::VMPC_KEY_ANSI_Grave },
    { kVK_ISO_Section, VmpcKeyCode::VMPC_KEY_ISO_Section },
    { kVK_Tab, VmpcKeyCode::VMPC_KEY_Tab },
    { kVK_CapsLock, VmpcKeyCode::VMPC_KEY_CapsLock },
    { kVK_Shift, VmpcKeyCode::VMPC_KEY_Shift },
    { kVK_Shift, VmpcKeyCode::VMPC_KEY_LeftShift },
    { kVK_RightShift, VmpcKeyCode::VMPC_KEY_RightShift },
    { kVK_Function, VmpcKeyCode::VMPC_KEY_Function },
    { kVK_Control, VmpcKeyCode::VMPC_KEY_Control },
    { kVK_Control, VmpcKeyCode::VMPC_KEY_LeftControl },
    { kVK_RightControl, VmpcKeyCode::VMPC_KEY_RightControl },
    { kVK_Option, VmpcKeyCode::VMPC_KEY_OptionOrAlt },
    { kVK_Command, VmpcKeyCode::VMPC_KEY_LeftMeta },
    { kVK_RightCommand, VmpcKeyCode::VMPC_KEY_RightMeta },
    { kVK_ANSI_Minus, VmpcKeyCode::VMPC_KEY_ANSI_Minus },
    { kVK_ANSI_Equal, VmpcKeyCode::VMPC_KEY_ANSI_Equal },
    { kVK_Delete, VmpcKeyCode::VMPC_KEY_Delete },
    { kVK_ANSI_LeftBracket, VmpcKeyCode::VMPC_KEY_ANSI_LeftBracket },
    { kVK_ANSI_RightBracket, VmpcKeyCode::VMPC_KEY_ANSI_RightBracket },
    { kVK_ANSI_Backslash, VmpcKeyCode::VMPC_KEY_ANSI_Backslash },
    { kVK_ANSI_Semicolon, VmpcKeyCode::VMPC_KEY_ANSI_Semicolon },
    { kVK_ANSI_Quote, VmpcKeyCode::VMPC_KEY_ANSI_Quote },
    { kVK_Return, VmpcKeyCode::VMPC_KEY_Return },
    { kVK_ANSI_KeypadEnter, VmpcKeyCode::VMPC_KEY_ANSI_KeypadEnter },
    { kVK_ANSI_Comma, VmpcKeyCode::VMPC_KEY_ANSI_Comma },
    { kVK_ANSI_Period, VmpcKeyCode::VMPC_KEY_ANSI_Period },
    { kVK_ANSI_Slash, VmpcKeyCode::VMPC_KEY_ANSI_Slash },
    { kVK_RightOption, VmpcKeyCode::VMPC_KEY_RightOptionOrAlt },
    { kVK_Space, VmpcKeyCode::VMPC_KEY_Space },

    { kVK_ANSI_Keypad0, VmpcKeyCode::VMPC_KEY_ANSI_Keypad0 },
    { kVK_ANSI_Keypad1, VmpcKeyCode::VMPC_KEY_ANSI_Keypad1 },
    { kVK_ANSI_Keypad2, VmpcKeyCode::VMPC_KEY_ANSI_Keypad2 },
    { kVK_ANSI_Keypad3, VmpcKeyCode::VMPC_KEY_ANSI_Keypad3 },
    { kVK_ANSI_Keypad4, VmpcKeyCode::VMPC_KEY_ANSI_Keypad4 },
    { kVK_ANSI_Keypad5, VmpcKeyCode::VMPC_KEY_ANSI_Keypad5 },
    { kVK_ANSI_Keypad6, VmpcKeyCode::VMPC_KEY_ANSI_Keypad6 },
    { kVK_ANSI_Keypad7, VmpcKeyCode::VMPC_KEY_ANSI_Keypad7 },
    { kVK_ANSI_Keypad8, VmpcKeyCode::VMPC_KEY_ANSI_Keypad8 },
    { kVK_ANSI_Keypad9, VmpcKeyCode::VMPC_KEY_ANSI_Keypad9 },

    { kVK_ANSI_0, VmpcKeyCode::VMPC_KEY_ANSI_0 },
    { kVK_ANSI_1, VmpcKeyCode::VMPC_KEY_ANSI_1 },
    { kVK_ANSI_2, VmpcKeyCode::VMPC_KEY_ANSI_2 },
    { kVK_ANSI_3, VmpcKeyCode::VMPC_KEY_ANSI_3 },
    { kVK_ANSI_4, VmpcKeyCode::VMPC_KEY_ANSI_4 },
    { kVK_ANSI_5, VmpcKeyCode::VMPC_KEY_ANSI_5 },
    { kVK_ANSI_6, VmpcKeyCode::VMPC_KEY_ANSI_6 },
    { kVK_ANSI_7, VmpcKeyCode::VMPC_KEY_ANSI_7 },
    { kVK_ANSI_8, VmpcKeyCode::VMPC_KEY_ANSI_8 },
    { kVK_ANSI_9, VmpcKeyCode::VMPC_KEY_ANSI_9 },

    { kVK_ANSI_A, VmpcKeyCode::VMPC_KEY_ANSI_A },
    { kVK_ANSI_B, VmpcKeyCode::VMPC_KEY_ANSI_B },
    { kVK_ANSI_C, VmpcKeyCode::VMPC_KEY_ANSI_C },
    { kVK_ANSI_D, VmpcKeyCode::VMPC_KEY_ANSI_D },
    { kVK_ANSI_E, VmpcKeyCode::VMPC_KEY_ANSI_E },
    { kVK_ANSI_F, VmpcKeyCode::VMPC_KEY_ANSI_F },
    { kVK_ANSI_G, VmpcKeyCode::VMPC_KEY_ANSI_G },
    { kVK_ANSI_H, VmpcKeyCode::VMPC_KEY_ANSI_H },
    { kVK_ANSI_I, VmpcKeyCode::VMPC_KEY_ANSI_I },
    { kVK_ANSI_J, VmpcKeyCode::VMPC_KEY_ANSI_J },
    { kVK_ANSI_K, VmpcKeyCode::VMPC_KEY_ANSI_K },
    { kVK_ANSI_L, VmpcKeyCode::VMPC_KEY_ANSI_L },
    { kVK_ANSI_M, VmpcKeyCode::VMPC_KEY_ANSI_M },
    { kVK_ANSI_N, VmpcKeyCode::VMPC_KEY_ANSI_N },
    { kVK_ANSI_O, VmpcKeyCode::VMPC_KEY_ANSI_O },
    { kVK_ANSI_P, VmpcKeyCode::VMPC_KEY_ANSI_P },
    { kVK_ANSI_Q, VmpcKeyCode::VMPC_KEY_ANSI_Q },
    { kVK_ANSI_R, VmpcKeyCode::VMPC_KEY_ANSI_R },
    { kVK_ANSI_S, VmpcKeyCode::VMPC_KEY_ANSI_S },
    { kVK_ANSI_T, VmpcKeyCode::VMPC_KEY_ANSI_T },
    { kVK_ANSI_U, VmpcKeyCode::VMPC_KEY_ANSI_U },
    { kVK_ANSI_V, VmpcKeyCode::VMPC_KEY_ANSI_V },
    { kVK_ANSI_W, VmpcKeyCode::VMPC_KEY_ANSI_W },
    { kVK_ANSI_X, VmpcKeyCode::VMPC_KEY_ANSI_X },
    { kVK_ANSI_Y, VmpcKeyCode::VMPC_KEY_ANSI_Y },
    { kVK_ANSI_Z, VmpcKeyCode::VMPC_KEY_ANSI_Z },

    { kVK_LeftArrow, VmpcKeyCode::VMPC_KEY_LeftArrow },
    { kVK_RightArrow, VmpcKeyCode::VMPC_KEY_RightArrow },
    { kVK_UpArrow, VmpcKeyCode::VMPC_KEY_UpArrow },
    { kVK_DownArrow, VmpcKeyCode::VMPC_KEY_DownArrow },

    { kVK_ANSI_KeypadDecimal, VmpcKeyCode::VMPC_KEY_ANSI_KeypadPeriodOrDelete },
    { kVK_ANSI_KeypadMultiply, VmpcKeyCode::VMPC_KEY_ANSI_KeypadMultiply },
    { kVK_ANSI_KeypadPlus, VmpcKeyCode::VMPC_KEY_ANSI_KeypadPlus },
    { kVK_ANSI_KeypadClear, VmpcKeyCode::VMPC_KEY_ANSI_KeypadNumLockOrClear },
    { kVK_ANSI_KeypadDivide, VmpcKeyCode::VMPC_KEY_ANSI_KeypadDivide },
    { kVK_ANSI_KeypadEnter, VmpcKeyCode::VMPC_KEY_ANSI_KeypadEnter },
    { kVK_ANSI_KeypadMinus, VmpcKeyCode::VMPC_KEY_ANSI_KeypadMinus },
    { kVK_ANSI_KeypadEquals, VmpcKeyCode::VMPC_KEY_ANSI_KeypadEquals },

    { kVK_Home, VmpcKeyCode::VMPC_KEY_Home },
    { kVK_End, VmpcKeyCode::VMPC_KEY_End },
    { kVK_PageUp, VmpcKeyCode::VMPC_KEY_PageUp },
    { kVK_PageDown, VmpcKeyCode::VMPC_KEY_PageDown }

#elif defined(__APPLE__) && TARGET_OS_IOS
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardF1, VmpcKeyCode::VMPC_KEY_F1 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardF2, VmpcKeyCode::VMPC_KEY_F2 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardF3, VmpcKeyCode::VMPC_KEY_F3 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardF4, VmpcKeyCode::VMPC_KEY_F4 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardF5, VmpcKeyCode::VMPC_KEY_F5 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardF6, VmpcKeyCode::VMPC_KEY_F6 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardF7, VmpcKeyCode::VMPC_KEY_F7 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardF8, VmpcKeyCode::VMPC_KEY_F8 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardF9, VmpcKeyCode::VMPC_KEY_F9 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardF10, VmpcKeyCode::VMPC_KEY_F10 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardF11, VmpcKeyCode::VMPC_KEY_F11 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardF12, VmpcKeyCode::VMPC_KEY_F12 },

        { UIKeyConstants::UIKeyboardHIDUsageKeyboardEscape, VmpcKeyCode::VMPC_KEY_Escape },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardGraveAccentAndTilde, VmpcKeyCode::VMPC_KEY_ANSI_Grave },
        { 0, VmpcKeyCode::VMPC_KEY_ISO_Section }, // No corresponding value
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardTab, VmpcKeyCode::VMPC_KEY_Tab },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardCapsLock, VmpcKeyCode::VMPC_KEY_CapsLock },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardLeftShift, VmpcKeyCode::VMPC_KEY_Shift },
        { 0, VmpcKeyCode::VMPC_KEY_Function }, // No corresponding value
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardLeftControl, VmpcKeyCode::VMPC_KEY_Control },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardLeftAlt, VmpcKeyCode::VMPC_KEY_OptionOrAlt },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardLeftAlt, VmpcKeyCode::VMPC_KEY_LeftOptionOrAlt },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardRightAlt, VmpcKeyCode::VMPC_KEY_RightOptionOrAlt },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardLeftGUI, VmpcKeyCode::VMPC_KEY_LeftMeta },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardRightGUI, VmpcKeyCode::VMPC_KEY_RightMeta },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardHyphen, VmpcKeyCode::VMPC_KEY_ANSI_Minus },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardEqualSign, VmpcKeyCode::VMPC_KEY_ANSI_Equal },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardDeleteOrBackspace, VmpcKeyCode::VMPC_KEY_Delete },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardOpenBracket, VmpcKeyCode::VMPC_KEY_ANSI_LeftBracket },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardCloseBracket, VmpcKeyCode::VMPC_KEY_ANSI_RightBracket },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardBackslash, VmpcKeyCode::VMPC_KEY_ANSI_Backslash },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardSemicolon, VmpcKeyCode::VMPC_KEY_ANSI_Semicolon },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardQuote, VmpcKeyCode::VMPC_KEY_ANSI_Quote },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardReturnOrEnter, VmpcKeyCode::VMPC_KEY_Return },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardComma, VmpcKeyCode::VMPC_KEY_ANSI_Comma },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardPeriod, VmpcKeyCode::VMPC_KEY_ANSI_Period },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardSlash, VmpcKeyCode::VMPC_KEY_ANSI_Slash },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardRightShift, VmpcKeyCode::VMPC_KEY_RightShift },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardRightAlt, VmpcKeyCode::VMPC_KEY_RightOptionOrAlt },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardSpacebar, VmpcKeyCode::VMPC_KEY_Space },

        { UIKeyConstants::UIKeyboardHIDUsageKeypad0, VmpcKeyCode::VMPC_KEY_ANSI_Keypad0 },
        { UIKeyConstants::UIKeyboardHIDUsageKeypad1, VmpcKeyCode::VMPC_KEY_ANSI_Keypad1 },
        { UIKeyConstants::UIKeyboardHIDUsageKeypad2, VmpcKeyCode::VMPC_KEY_ANSI_Keypad2 },
        { UIKeyConstants::UIKeyboardHIDUsageKeypad3, VmpcKeyCode::VMPC_KEY_ANSI_Keypad3 },
        { UIKeyConstants::UIKeyboardHIDUsageKeypad4, VmpcKeyCode::VMPC_KEY_ANSI_Keypad4 },
        { UIKeyConstants::UIKeyboardHIDUsageKeypad5, VmpcKeyCode::VMPC_KEY_ANSI_Keypad5 },
        { UIKeyConstants::UIKeyboardHIDUsageKeypad6, VmpcKeyCode::VMPC_KEY_ANSI_Keypad6 },
        { UIKeyConstants::UIKeyboardHIDUsageKeypad7, VmpcKeyCode::VMPC_KEY_ANSI_Keypad7 },
        { UIKeyConstants::UIKeyboardHIDUsageKeypad8, VmpcKeyCode::VMPC_KEY_ANSI_Keypad8 },
        { UIKeyConstants::UIKeyboardHIDUsageKeypad9, VmpcKeyCode::VMPC_KEY_ANSI_Keypad9 },
        { UIKeyConstants::UIKeyboardHIDUsageKeypadNumLock, VmpcKeyCode::VMPC_KEY_ANSI_KeypadNumLockOrClear },
        { UIKeyConstants::UIKeyboardHIDUsageKeypadSlash, VmpcKeyCode::VMPC_KEY_ANSI_KeypadDivide },
        { UIKeyConstants::UIKeyboardHIDUsageKeypadAsterisk, VmpcKeyCode::VMPC_KEY_ANSI_KeypadMultiply },
        { UIKeyConstants::UIKeyboardHIDUsageKeypadHyphen, VmpcKeyCode::VMPC_KEY_ANSI_KeypadMinus },
        { UIKeyConstants::UIKeyboardHIDUsageKeypadPlus, VmpcKeyCode::VMPC_KEY_ANSI_KeypadPlus },
        { UIKeyConstants::UIKeyboardHIDUsageKeypadEnter, VmpcKeyCode::VMPC_KEY_ANSI_KeypadEnter },
        { UIKeyConstants::UIKeyboardHIDUsageKeypadPeriod, VmpcKeyCode::VMPC_KEY_ANSI_KeypadPeriodOrDelete },
        { UIKeyConstants::UIKeyboardHIDUsageKeypadEqualSign, VmpcKeyCode::VMPC_KEY_ANSI_KeypadEquals },
        { UIKeyConstants::UIKeyboardHIDUsageKeypadComma, VmpcKeyCode::VMPC_KEY_ANSI_KeypadComma },

        { UIKeyConstants::UIKeyboardHIDUsageKeyboard0, VmpcKeyCode::VMPC_KEY_ANSI_0 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboard1, VmpcKeyCode::VMPC_KEY_ANSI_1 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboard2, VmpcKeyCode::VMPC_KEY_ANSI_2 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboard3, VmpcKeyCode::VMPC_KEY_ANSI_3 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboard4, VmpcKeyCode::VMPC_KEY_ANSI_4 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboard5, VmpcKeyCode::VMPC_KEY_ANSI_5 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboard6, VmpcKeyCode::VMPC_KEY_ANSI_6 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboard7, VmpcKeyCode::VMPC_KEY_ANSI_7 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboard8, VmpcKeyCode::VMPC_KEY_ANSI_8 },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboard9, VmpcKeyCode::VMPC_KEY_ANSI_9 },

        { UIKeyConstants::UIKeyboardHIDUsageKeyboardQ, VmpcKeyCode::VMPC_KEY_ANSI_Q },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardW, VmpcKeyCode::VMPC_KEY_ANSI_W },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardE, VmpcKeyCode::VMPC_KEY_ANSI_E },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardR, VmpcKeyCode::VMPC_KEY_ANSI_R },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardT, VmpcKeyCode::VMPC_KEY_ANSI_T },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardY, VmpcKeyCode::VMPC_KEY_ANSI_Y },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardU, VmpcKeyCode::VMPC_KEY_ANSI_U },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardI, VmpcKeyCode::VMPC_KEY_ANSI_I },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardO, VmpcKeyCode::VMPC_KEY_ANSI_O },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardP, VmpcKeyCode::VMPC_KEY_ANSI_P },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardA, VmpcKeyCode::VMPC_KEY_ANSI_A },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardS, VmpcKeyCode::VMPC_KEY_ANSI_S },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardD, VmpcKeyCode::VMPC_KEY_ANSI_D },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardF, VmpcKeyCode::VMPC_KEY_ANSI_F },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardG, VmpcKeyCode::VMPC_KEY_ANSI_G },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardH, VmpcKeyCode::VMPC_KEY_ANSI_H },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardJ, VmpcKeyCode::VMPC_KEY_ANSI_J },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardK, VmpcKeyCode::VMPC_KEY_ANSI_K },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardL, VmpcKeyCode::VMPC_KEY_ANSI_L },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardZ, VmpcKeyCode::VMPC_KEY_ANSI_Z },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardX, VmpcKeyCode::VMPC_KEY_ANSI_X },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardC, VmpcKeyCode::VMPC_KEY_ANSI_C },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardV, VmpcKeyCode::VMPC_KEY_ANSI_V },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardB, VmpcKeyCode::VMPC_KEY_ANSI_B },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardN, VmpcKeyCode::VMPC_KEY_ANSI_N },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardM, VmpcKeyCode::VMPC_KEY_ANSI_M },

        { UIKeyConstants::UIKeyboardHIDUsageKeyboardLeftArrow, VmpcKeyCode::VMPC_KEY_LeftArrow },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardRightArrow, VmpcKeyCode::VMPC_KEY_RightArrow },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardUpArrow, VmpcKeyCode::VMPC_KEY_UpArrow },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardDownArrow, VmpcKeyCode::VMPC_KEY_DownArrow },

        { UIKeyConstants::UIKeyboardHIDUsageKeyboardHome, VmpcKeyCode::VMPC_KEY_Home },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardPageUp, VmpcKeyCode::VMPC_KEY_PageUp },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardEnd, VmpcKeyCode::VMPC_KEY_End },
        { UIKeyConstants::UIKeyboardHIDUsageKeyboardPageDown, VmpcKeyCode::VMPC_KEY_PageDown }

#elif defined _WIN32

        { VK_F1, VmpcKeyCode::VMPC_KEY_F1 },
        { VK_F2, VmpcKeyCode::VMPC_KEY_F2 },
        { VK_F3, VmpcKeyCode::VMPC_KEY_F3 },
        { VK_F4, VmpcKeyCode::VMPC_KEY_F4 },
        { VK_F5, VmpcKeyCode::VMPC_KEY_F5 },
        { VK_F6, VmpcKeyCode::VMPC_KEY_F6 },
        { VK_F7, VmpcKeyCode::VMPC_KEY_F7 },
        { VK_F8, VmpcKeyCode::VMPC_KEY_F8 },
        { VK_F9, VmpcKeyCode::VMPC_KEY_F9 },
        { VK_F10, VmpcKeyCode::VMPC_KEY_F10 },
        { VK_F11, VmpcKeyCode::VMPC_KEY_F11 },
        { VK_F12, VmpcKeyCode::VMPC_KEY_F12 },

        { VK_ESCAPE, VmpcKeyCode::VMPC_KEY_Escape },
        { VK_TAB, VmpcKeyCode::VMPC_KEY_Tab },
        { VK_CAPITAL, VmpcKeyCode::VMPC_KEY_CapsLock },
        { VK_SHIFT, VmpcKeyCode::VMPC_KEY_Shift },
        { VK_CONTROL, VmpcKeyCode::VMPC_KEY_Control },
        { VK_LCONTROL, VmpcKeyCode::VMPC_KEY_LeftControl },
        { VK_RCONTROL, VmpcKeyCode::VMPC_KEY_RightControl },
        { VK_MENU, VmpcKeyCode::VMPC_KEY_OptionOrAlt },
        { VK_LMENU, VmpcKeyCode::VMPC_KEY_LeftOptionOrAlt },
        { VK_RMENU, VmpcKeyCode::VMPC_KEY_RightOptionOrAlt },
        { VK_OEM_MINUS, VmpcKeyCode::VMPC_KEY_ANSI_Minus },
        { VK_OEM_PLUS, VmpcKeyCode::VMPC_KEY_ANSI_Equal },
        { VK_INSERT, VmpcKeyCode::VMPC_KEY_Insert },
        { VK_DELETE, VmpcKeyCode::VMPC_KEY_Delete },
        { VK_RETURN, VmpcKeyCode::VMPC_KEY_Return },
        { VK_OEM_COMMA, VmpcKeyCode::VMPC_KEY_ANSI_Comma },
        { VK_OEM_PERIOD, VmpcKeyCode::VMPC_KEY_ANSI_Period },
        { VK_SPACE, VmpcKeyCode::VMPC_KEY_Space },

        { VK_OEM_1, VmpcKeyCode::VMPC_KEY_ANSI_Semicolon },
        { VK_OEM_2, VmpcKeyCode::VMPC_KEY_ANSI_Slash },
        { VK_OEM_3, VmpcKeyCode::VMPC_KEY_ANSI_Grave },
        { VK_OEM_4, VmpcKeyCode::VMPC_KEY_ANSI_LeftBracket },
        { VK_OEM_6, VmpcKeyCode::VMPC_KEY_ANSI_RightBracket },
        { VK_OEM_5, VmpcKeyCode::VMPC_KEY_ANSI_Backslash },
        { VK_OEM_7, VmpcKeyCode::VMPC_KEY_ANSI_Quote },

        { VK_NUMPAD0, VmpcKeyCode::VMPC_KEY_ANSI_Keypad0 },
        { VK_NUMPAD1, VmpcKeyCode::VMPC_KEY_ANSI_Keypad1 },
        { VK_NUMPAD2, VmpcKeyCode::VMPC_KEY_ANSI_Keypad2 },
        { VK_NUMPAD3, VmpcKeyCode::VMPC_KEY_ANSI_Keypad3 },
        { VK_NUMPAD4, VmpcKeyCode::VMPC_KEY_ANSI_Keypad4 },
        { VK_NUMPAD5, VmpcKeyCode::VMPC_KEY_ANSI_Keypad5 },
        { VK_NUMPAD6, VmpcKeyCode::VMPC_KEY_ANSI_Keypad6 },
        { VK_NUMPAD7, VmpcKeyCode::VMPC_KEY_ANSI_Keypad7 },
        { VK_NUMPAD8, VmpcKeyCode::VMPC_KEY_ANSI_Keypad8 },
        { VK_NUMPAD9, VmpcKeyCode::VMPC_KEY_ANSI_Keypad9 },

        { '0', VmpcKeyCode::VMPC_KEY_ANSI_0 },
        { '1', VmpcKeyCode::VMPC_KEY_ANSI_1 },
        { '2', VmpcKeyCode::VMPC_KEY_ANSI_2 },
        { '3', VmpcKeyCode::VMPC_KEY_ANSI_3 },
        { '4', VmpcKeyCode::VMPC_KEY_ANSI_4 },
        { '5', VmpcKeyCode::VMPC_KEY_ANSI_5 },
        { '6', VmpcKeyCode::VMPC_KEY_ANSI_6 },
        { '7', VmpcKeyCode::VMPC_KEY_ANSI_7 },
        { '8', VmpcKeyCode::VMPC_KEY_ANSI_8 },
        { '9', VmpcKeyCode::VMPC_KEY_ANSI_9 },

        { 'A', VmpcKeyCode::VMPC_KEY_ANSI_A },
        { 'B', VmpcKeyCode::VMPC_KEY_ANSI_B },
        { 'C', VmpcKeyCode::VMPC_KEY_ANSI_C },
        { 'D', VmpcKeyCode::VMPC_KEY_ANSI_D },
        { 'E', VmpcKeyCode::VMPC_KEY_ANSI_E },
        { 'F', VmpcKeyCode::VMPC_KEY_ANSI_F },
        { 'G', VmpcKeyCode::VMPC_KEY_ANSI_G },
        { 'H', VmpcKeyCode::VMPC_KEY_ANSI_H },
        { 'I', VmpcKeyCode::VMPC_KEY_ANSI_I },
        { 'J', VmpcKeyCode::VMPC_KEY_ANSI_J },
        { 'K', VmpcKeyCode::VMPC_KEY_ANSI_K },
        { 'L', VmpcKeyCode::VMPC_KEY_ANSI_L },
        { 'M', VmpcKeyCode::VMPC_KEY_ANSI_M },
        { 'N', VmpcKeyCode::VMPC_KEY_ANSI_N },
        { 'O', VmpcKeyCode::VMPC_KEY_ANSI_O },
        { 'P', VmpcKeyCode::VMPC_KEY_ANSI_P },
        { 'Q', VmpcKeyCode::VMPC_KEY_ANSI_Q },
        { 'R', VmpcKeyCode::VMPC_KEY_ANSI_R },
        { 'S', VmpcKeyCode::VMPC_KEY_ANSI_S },
        { 'T', VmpcKeyCode::VMPC_KEY_ANSI_T },
        { 'U', VmpcKeyCode::VMPC_KEY_ANSI_U },
        { 'V', VmpcKeyCode::VMPC_KEY_ANSI_V },
        { 'W', VmpcKeyCode::VMPC_KEY_ANSI_W },
        { 'X', VmpcKeyCode::VMPC_KEY_ANSI_X },
        { 'Y', VmpcKeyCode::VMPC_KEY_ANSI_Y },
        { 'Z', VmpcKeyCode::VMPC_KEY_ANSI_Z },

        { VK_LEFT, VmpcKeyCode::VMPC_KEY_LeftArrow },
        { VK_RIGHT, VmpcKeyCode::VMPC_KEY_RightArrow },
        { VK_UP, VmpcKeyCode::VMPC_KEY_UpArrow },
        { VK_DOWN, VmpcKeyCode::VMPC_KEY_DownArrow },

        { VK_DECIMAL, VmpcKeyCode::VMPC_KEY_ANSI_KeypadPeriodOrDelete },
        { VK_MULTIPLY, VmpcKeyCode::VMPC_KEY_ANSI_KeypadMultiply },
        { VK_ADD, VmpcKeyCode::VMPC_KEY_ANSI_KeypadPlus },
        { VK_CLEAR, VmpcKeyCode::VMPC_KEY_ANSI_KeypadNumLockOrClear },
        { VK_DIVIDE, VmpcKeyCode::VMPC_KEY_ANSI_KeypadDivide },
        { VK_SUBTRACT, VmpcKeyCode::VMPC_KEY_ANSI_KeypadMinus },

        { VK_HOME, VmpcKeyCode::VMPC_KEY_Home },
        { VK_PRIOR, VmpcKeyCode::VMPC_KEY_PageUp },
        { VK_END, VmpcKeyCode::VMPC_KEY_End },
        { VK_NEXT, VmpcKeyCode::VMPC_KEY_PageDown },
        { VK_BACK, VmpcKeyCode::VMPC_KEY_Backspace }

#elif defined __linux__

        { XK_F1, VmpcKeyCode::VMPC_KEY_F1 },
        { XK_F2, VmpcKeyCode::VMPC_KEY_F2 },
        { XK_F3, VmpcKeyCode::VMPC_KEY_F3 },
        { XK_F4, VmpcKeyCode::VMPC_KEY_F4 },
        { XK_F5, VmpcKeyCode::VMPC_KEY_F5 },
        { XK_F6, VmpcKeyCode::VMPC_KEY_F6 },
        { XK_F7, VmpcKeyCode::VMPC_KEY_F7 },
        { XK_F8, VmpcKeyCode::VMPC_KEY_F8 },
        { XK_F9, VmpcKeyCode::VMPC_KEY_F9 },
        { XK_F10, VmpcKeyCode::VMPC_KEY_F10 },
        { XK_F11, VmpcKeyCode::VMPC_KEY_F11 },
        { XK_F12, VmpcKeyCode::VMPC_KEY_F12 },

        { XK_BackSpace, VmpcKeyCode::VMPC_KEY_Backspace },
        { XK_Escape, VmpcKeyCode::VMPC_KEY_Escape },
        { XK_Tab, VmpcKeyCode::VMPC_KEY_Tab },
        { XK_Caps_Lock, VmpcKeyCode::VMPC_KEY_CapsLock },
        { XK_Shift_L, VmpcKeyCode::VMPC_KEY_Shift },
        { XK_Shift_L, VmpcKeyCode::VMPC_KEY_LeftShift },
        { XK_Shift_R, VmpcKeyCode::VMPC_KEY_RightShift },
        { XK_Control_L, VmpcKeyCode::VMPC_KEY_Control },
        { XK_Control_L, VmpcKeyCode::VMPC_KEY_LeftControl },
        { XK_Control_R, VmpcKeyCode::VMPC_KEY_LeftControl },
        { XK_Alt_L, VmpcKeyCode::VMPC_KEY_OptionOrAlt },
        { XK_Alt_L, VmpcKeyCode::VMPC_KEY_LeftOptionOrAlt },
        { XK_Alt_R, VmpcKeyCode::VMPC_KEY_RightOptionOrAlt },
        { XK_minus, VmpcKeyCode::VMPC_KEY_ANSI_Minus },
        { XK_equal, VmpcKeyCode::VMPC_KEY_ANSI_Equal },
        { XK_Insert, VmpcKeyCode::VMPC_KEY_Insert },
        { XK_Delete, VmpcKeyCode::VMPC_KEY_Delete },
        { XK_bracketleft, VmpcKeyCode::VMPC_KEY_ANSI_LeftBracket },
        { XK_bracketright, VmpcKeyCode::VMPC_KEY_ANSI_RightBracket },
        { XK_backslash, VmpcKeyCode::VMPC_KEY_ANSI_Backslash },
        { XK_semicolon, VmpcKeyCode::VMPC_KEY_ANSI_Semicolon },
        { XK_apostrophe, VmpcKeyCode::VMPC_KEY_ANSI_Quote },
        { XK_Return, VmpcKeyCode::VMPC_KEY_Return },
        { XK_comma, VmpcKeyCode::VMPC_KEY_ANSI_Comma },
        { XK_period, VmpcKeyCode::VMPC_KEY_ANSI_Period },
        { XK_slash, VmpcKeyCode::VMPC_KEY_ANSI_Slash },
        { XK_space, VmpcKeyCode::VMPC_KEY_Space },

        { XK_0, VmpcKeyCode::VMPC_KEY_ANSI_0 },
        { XK_1, VmpcKeyCode::VMPC_KEY_ANSI_1 },
        { XK_2, VmpcKeyCode::VMPC_KEY_ANSI_2 },
        { XK_3, VmpcKeyCode::VMPC_KEY_ANSI_3 },
        { XK_4, VmpcKeyCode::VMPC_KEY_ANSI_4 },
        { XK_5, VmpcKeyCode::VMPC_KEY_ANSI_5 },
        { XK_6, VmpcKeyCode::VMPC_KEY_ANSI_6 },
        { XK_7, VmpcKeyCode::VMPC_KEY_ANSI_7 },
        { XK_8, VmpcKeyCode::VMPC_KEY_ANSI_8 },
        { XK_9, VmpcKeyCode::VMPC_KEY_ANSI_9 },

        { XK_a, VmpcKeyCode::VMPC_KEY_ANSI_A },
        { XK_b, VmpcKeyCode::VMPC_KEY_ANSI_B },
        { XK_c, VmpcKeyCode::VMPC_KEY_ANSI_C },
        { XK_d, VmpcKeyCode::VMPC_KEY_ANSI_D },
        { XK_e, VmpcKeyCode::VMPC_KEY_ANSI_E },
        { XK_f, VmpcKeyCode::VMPC_KEY_ANSI_F },
        { XK_g, VmpcKeyCode::VMPC_KEY_ANSI_G },
        { XK_h, VmpcKeyCode::VMPC_KEY_ANSI_H },
        { XK_i, VmpcKeyCode::VMPC_KEY_ANSI_I },
        { XK_j, VmpcKeyCode::VMPC_KEY_ANSI_J },
        { XK_k, VmpcKeyCode::VMPC_KEY_ANSI_K },
        { XK_l, VmpcKeyCode::VMPC_KEY_ANSI_L },
        { XK_m, VmpcKeyCode::VMPC_KEY_ANSI_M },
        { XK_n, VmpcKeyCode::VMPC_KEY_ANSI_N },
        { XK_o, VmpcKeyCode::VMPC_KEY_ANSI_O },
        { XK_p, VmpcKeyCode::VMPC_KEY_ANSI_P },
        { XK_q, VmpcKeyCode::VMPC_KEY_ANSI_Q },
        { XK_r, VmpcKeyCode::VMPC_KEY_ANSI_R },
        { XK_s, VmpcKeyCode::VMPC_KEY_ANSI_S },
        { XK_t, VmpcKeyCode::VMPC_KEY_ANSI_T },
        { XK_u, VmpcKeyCode::VMPC_KEY_ANSI_U },
        { XK_v, VmpcKeyCode::VMPC_KEY_ANSI_V },
        { XK_w, VmpcKeyCode::VMPC_KEY_ANSI_W },
        { XK_x, VmpcKeyCode::VMPC_KEY_ANSI_X },
        { XK_y, VmpcKeyCode::VMPC_KEY_ANSI_Y },
        { XK_z, VmpcKeyCode::VMPC_KEY_ANSI_Z },

        { XK_Left, VmpcKeyCode::VMPC_KEY_LeftArrow },
        { XK_Right, VmpcKeyCode::VMPC_KEY_RightArrow },
        { XK_Up, VmpcKeyCode::VMPC_KEY_UpArrow },
        { XK_Down, VmpcKeyCode::VMPC_KEY_DownArrow },

        { XK_Home, VmpcKeyCode::VMPC_KEY_Home },
        { XK_Page_Up, VmpcKeyCode::VMPC_KEY_PageUp },
        { XK_End, VmpcKeyCode::VMPC_KEY_End },
        { XK_Page_Down, VmpcKeyCode::VMPC_KEY_PageDown }
#endif
};

const std::map<const VmpcKeyCode, const char> KeyCodeHelper::typableVmpcKeyCodes {
    { VmpcKeyCode::VMPC_KEY_ANSI_0, '0' },
    { VmpcKeyCode::VMPC_KEY_ANSI_1, '1' },
    { VmpcKeyCode::VMPC_KEY_ANSI_2, '2' },
    { VmpcKeyCode::VMPC_KEY_ANSI_3, '3' },
    { VmpcKeyCode::VMPC_KEY_ANSI_4, '4' },
    { VmpcKeyCode::VMPC_KEY_ANSI_5, '5' },
    { VmpcKeyCode::VMPC_KEY_ANSI_6, '6' },
    { VmpcKeyCode::VMPC_KEY_ANSI_7, '7' },
    { VmpcKeyCode::VMPC_KEY_ANSI_8, '8' },
    { VmpcKeyCode::VMPC_KEY_ANSI_9, '9' },
    { VmpcKeyCode::VMPC_KEY_ANSI_A, 'a' },
    { VmpcKeyCode::VMPC_KEY_ANSI_B, 'b' },
    { VmpcKeyCode::VMPC_KEY_ANSI_C, 'c' },
    { VmpcKeyCode::VMPC_KEY_ANSI_D, 'd' },
    { VmpcKeyCode::VMPC_KEY_ANSI_E, 'e' },
    { VmpcKeyCode::VMPC_KEY_ANSI_F, 'f' },
    { VmpcKeyCode::VMPC_KEY_ANSI_G, 'g' },
    { VmpcKeyCode::VMPC_KEY_ANSI_H, 'h' },
    { VmpcKeyCode::VMPC_KEY_ANSI_I, 'i' },
    { VmpcKeyCode::VMPC_KEY_ANSI_J, 'j' },
    { VmpcKeyCode::VMPC_KEY_ANSI_K, 'k' },
    { VmpcKeyCode::VMPC_KEY_ANSI_L, 'l' },
    { VmpcKeyCode::VMPC_KEY_ANSI_M, 'm' },
    { VmpcKeyCode::VMPC_KEY_ANSI_N, 'n' },
    { VmpcKeyCode::VMPC_KEY_ANSI_O, 'o' },
    { VmpcKeyCode::VMPC_KEY_ANSI_P, 'p' },
    { VmpcKeyCode::VMPC_KEY_ANSI_Q, 'q' },
    { VmpcKeyCode::VMPC_KEY_ANSI_R, 'r' },
    { VmpcKeyCode::VMPC_KEY_ANSI_S, 's' },
    { VmpcKeyCode::VMPC_KEY_ANSI_T, 't' },
    { VmpcKeyCode::VMPC_KEY_ANSI_U, 'u' },
    { VmpcKeyCode::VMPC_KEY_ANSI_V, 'v' },
    { VmpcKeyCode::VMPC_KEY_ANSI_W, 'w' },
    { VmpcKeyCode::VMPC_KEY_ANSI_X, 'x' },
    { VmpcKeyCode::VMPC_KEY_ANSI_Y, 'y' },
    { VmpcKeyCode::VMPC_KEY_ANSI_Z, 'z' },
    { VmpcKeyCode::VMPC_KEY_Space, ' ' },
    { VmpcKeyCode::VMPC_KEY_Backspace, ' ' }
};

const VmpcKeyCode KeyCodeHelper::getVmpcFromPlatformKeyCode(const int platformKeyCode)
{
    if (platformToVmpcKeyCodes.count(platformKeyCode) == 0)
    {
        return VmpcKeyCode::VMPC_KEY_UNKNOWN;
    }

    return platformToVmpcKeyCodes.at(platformKeyCode);
}

const char KeyCodeHelper::getCharForTypableVmpcKeyCode(const VmpcKeyCode vmpcKeyCode)
{
    if (typableVmpcKeyCodes.count(vmpcKeyCode) == 0)
    {
        return 0;
    }

    return typableVmpcKeyCodes.at(vmpcKeyCode);
}

const int KeyCodeHelper::getPlatformFromVmpcKeyCode(const VmpcKeyCode vmpcKeyCode)
{
    for (auto &kv : platformToVmpcKeyCodes)
    {
        if (kv.second == vmpcKeyCode)
        {
            return kv.first;
        }
    }

    return -1;
}

const std::string getDescriptorIfCommonForAsciiAndUnicode(const VmpcKeyCode vmpcKeyCode)
{
    if (vmpcKeyCode >= VmpcKeyCode::VMPC_KEY_F1 && vmpcKeyCode <= VmpcKeyCode::VMPC_KEY_F12)
    {
        const auto fNumber = ((int)vmpcKeyCode - (int)VmpcKeyCode::VMPC_KEY_F1) + 1;
        return "F" + std::to_string(fNumber);
    }

    std::string result;

    switch (vmpcKeyCode)
    {
        // Bespoke descriptors for keys that don't produce characters.
        // Below descriptors are used for simple ASCII-based display, like the
        // virtual MPC2000XL's LCD screen, as well as for Unicode-based text
        // rendering in a more graphically advanced UI.
        case VmpcKeyCode::VMPC_KEY_Space: result = "Space"; break;
        case VmpcKeyCode::VMPC_KEY_Shift: result = "Shift"; break;
        case VmpcKeyCode::VMPC_KEY_LeftShift: result = "Left shift"; break;
        case VmpcKeyCode::VMPC_KEY_RightShift: result = "Right shift"; break;
        case VmpcKeyCode::VMPC_KEY_OptionOrAlt: result = "Option/Alt"; break;
        case VmpcKeyCode::VMPC_KEY_LeftOptionOrAlt: result = "Left option/alt"; break;
        case VmpcKeyCode::VMPC_KEY_RightOptionOrAlt: result = "Right option/alt"; break;
        case VmpcKeyCode::VMPC_KEY_Control: result = "Control"; break;
        case VmpcKeyCode::VMPC_KEY_LeftControl: result = "Left control"; break;
        case VmpcKeyCode::VMPC_KEY_RightControl: result = "Right control"; break;
        case VmpcKeyCode::VMPC_KEY_Escape: result = "Escape"; break;
        case VmpcKeyCode::VMPC_KEY_Return: result = "Return"; break;
        case VmpcKeyCode::VMPC_KEY_PageUp: result = "Page up"; break;
        case VmpcKeyCode::VMPC_KEY_PageDown: result = "Page down"; break;
        case VmpcKeyCode::VMPC_KEY_Home: result = "Home"; break;
        case VmpcKeyCode::VMPC_KEY_End: result = "End"; break;
        case VmpcKeyCode::VMPC_KEY_Delete: result = "Delete"; break;
        case VmpcKeyCode::VMPC_KEY_Insert: result = "Insert"; break;
        case VmpcKeyCode::VMPC_KEY_Tab: result = "Tab"; break;
        case VmpcKeyCode::VMPC_KEY_CapsLock: result = "Caps lock"; break;
        case VmpcKeyCode::VMPC_KEY_Function: result = "Function"; break;
        case VmpcKeyCode::VMPC_KEY_LeftMeta: result = "Left meta"; break;
        case VmpcKeyCode::VMPC_KEY_RightMeta: result = "Right meta"; break;
        case VmpcKeyCode::VMPC_KEY_Backspace: result = "Backspace"; break;
        case VmpcKeyCode::VMPC_KEY_ANSI_KeypadNumLockOrClear: result = "Keypad num lock"; break;
        case VmpcKeyCode::VMPC_KEY_ANSI_KeypadEnter: result = "Keypad enter"; break;
        default: break;
    }

    return result;
}

const std::string KeyCodeHelper::getAsciiCompatibleDisplayName(const VmpcKeyCode vmpcKeyCode)
{
    const auto commonDescriptor = getDescriptorIfCommonForAsciiAndUnicode(vmpcKeyCode);

    if (!commonDescriptor.empty())
    {
        return commonDescriptor;
    }

    std::string result;

    switch (vmpcKeyCode)
    {
        // Bespoke descriptors for keys that don't produce characters.
        // Below descriptors are only used for simple ASCII-based display, like the
        // virtual MPC2000XL's LCD screen.
        case VmpcKeyCode::VMPC_KEY_LeftArrow: result = "Left arrow"; break;
        case VmpcKeyCode::VMPC_KEY_UpArrow: result = "Up arrow"; break;
        case VmpcKeyCode::VMPC_KEY_RightArrow: result = "Right arrow"; break;
        case VmpcKeyCode::VMPC_KEY_DownArrow: result = "Down arrow"; break;

        // Keys that produce characters, but a descriptor should be used as display name
        case VmpcKeyCode::VMPC_KEY_ANSI_Grave: result = "Grave accent"; break;
        case VmpcKeyCode::VMPC_KEY_ISO_Section: result = "Section"; break;

        default: result = getKeyCodeString(vmpcKeyCode);
    }

    return result;
}

const std::string KeyCodeHelper::guessCharactersPrintedOnKeyUnicode(const VmpcKeyCode vmpcKeyCode)
{
    const auto commonDescriptor = getDescriptorIfCommonForAsciiAndUnicode(vmpcKeyCode);

    if (!commonDescriptor.empty())
    {
        return commonDescriptor;
    }

    std::string result;

    switch (vmpcKeyCode)
    {
        // Bespoke descriptors for keys that don't produce characters.
        // Below descriptors are only used for Unicode-based text rendering.
        case VmpcKeyCode::VMPC_KEY_LeftArrow: result = "\u2190"; break;
        case VmpcKeyCode::VMPC_KEY_UpArrow: result = "\u2191"; break;
        case VmpcKeyCode::VMPC_KEY_RightArrow: result = "\u2192"; break;
        case VmpcKeyCode::VMPC_KEY_DownArrow: result = "\u2193"; break;

        default: result = getKeyCodeString(vmpcKeyCode);
    }

    return result;
}

const std::string KeyCodeHelper::getKeyCodeString(const VmpcKeyCode vmpcKeyCode)
{
    const auto platformKeyCode = getPlatformFromVmpcKeyCode(vmpcKeyCode);

    if (platformKeyCode == - 1)
    {
        return {};
    }

    const auto layout = KeyboardLayout::getCurrentKeyboardLayout();

    if (layout.count(platformKeyCode) == 0)
    {
        return {};
    }

    auto keyCodeInfo = layout.at(platformKeyCode);
    auto c1 = keyCodeInfo.charWithoutModifiers;
    const auto c2 = keyCodeInfo.charWithShiftModifier;
    
    if (c1.length() == 1 && c2.length() == 1)
    {
        if (std::toupper(c1.front()) == c2.front())
        {
            c1.clear();
        }
    }
    
    return c1 + c2;
}
