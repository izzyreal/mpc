#include <string>
#include <vector>
#include <map>

#if __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#else
#include <Carbon/Carbon.h>
#endif
#elif defined (_WIN32)
#include <windows.h>
#elif defined (__linux__)
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#endif

namespace mpc::controls {

    class KeyboardLayout {
        
        struct KeyCodeInfo {
            const int platformSpecificVirtualKeycode;
            const std::string charWithoutModifiers;
            const std::string charWithShiftModifier;
        };

        public:
            static const std::map<const int, const KeyCodeInfo> getCurrentKeyboardLayout()
            {
                std::map<const int, const KeyCodeInfo> result;
                const std::vector<std::string> modifierNames { "", "Shift" };
#ifdef _WIN32
                HKL layout = GetKeyboardLayout(0);
                for (int vk = 0x20; vk <= 0xFF; ++vk)
                {
                    std::string charWithoutModifiers;
                    std::string charWithShiftModifier;

                    BYTE keyState[256] = {0};
                    WCHAR buffer[10] = {0};
                    std::wstring symbols;
                
                    if (ToUnicodeEx(vk, MapVirtualKeyEx(vk, MAPVK_VK_TO_VSC, layout), keyState, buffer, 10, 0, layout) > 0)
                    {
                        symbols = buffer;
                    }
                    
                    if (!symbols.empty())
                    {
                        charWithoutModifiers = std::string(symbols.begin(), symbols.end());
                    }

                    symbols.clear();

                    keyState[VK_SHIFT] = 0x80;

                    if (ToUnicodeEx(vk, MapVirtualKeyEx(vk, MAPVK_VK_TO_VSC, layout), keyState, buffer, 10, 0, layout) > 0)
                    {
                        symbols = buffer;
                    }

                    if (!symbols.empty())
                    {
                        charWithShiftModifier = std::string(symbols.begin(), symbols.end());
                    }

                    if (!charWithoutModifiers.empty() || !charWithShiftModifier.empty())
                    {
                        result.emplace(vk, KeyCodeInfo{ vk, charWithoutModifiers, charWithShiftModifier });
                    }
                }
#elif defined(__APPLE__) && (TARGET_OS_OSX == 1)
                const TISInputSourceRef inputSource = TISCopyCurrentKeyboardLayoutInputSource();

                if (!inputSource)
                {
                    printf("Failed to get input source\n");
                    return {};
                }

                const auto layoutData = static_cast<CFDataRef>(TISGetInputSourceProperty(inputSource, kTISPropertyUnicodeKeyLayoutData));
                const UCKeyboardLayout* keyboardLayout = reinterpret_cast<const UCKeyboardLayout*>(CFDataGetBytePtr(layoutData));

                if (!keyboardLayout)
                {
                    printf("Failed to get keyboard layout\n");
                    return {};
                }

                UInt32 keysDown = 0;

                for (int vk = 0; vk < 512; vk++)
                {
                    std::string charWithoutModifiers;
                    std::string charWithShiftModifier;

                    for (auto& modifierName : modifierNames)
                    {
                        UniChar chars[4];
                        std::string symbols;
                        UInt32 modifiers = modifierName.empty() ? 0 : shiftKey;
                        UniCharCount length = 0;

                        OSStatus status = UCKeyTranslate(
                                keyboardLayout,
                                vk,
                                kUCKeyActionDown,
                                modifiers >> 8,
                                LMGetKbdType(),
                                kUCKeyTranslateNoDeadKeysMask,
                                &keysDown,
                                sizeof(chars) / sizeof(chars[0]),
                                &length,
                                chars
                                );

                        if (status == noErr)
                        {
                            for (UniCharCount i = 0; i < length; ++i)
                            {
                                UniChar ch = chars[i];

                                if (ch >= 0x20 && ch != 0x7F)
                                {
                                    symbols += convertUniCharToUTF8(ch);
                                }
                            }
                        }

                        if (!symbols.empty())
                        {
                            if (modifierName.empty())
                            {
                                charWithoutModifiers = symbols;
                            }
                            else /* modifierName == "Shift" */
                            {
                                charWithShiftModifier = symbols;
                            }
                        }
                    }

                    if (charWithoutModifiers.empty() && charWithShiftModifier.empty())
                    {
                        continue;
                    }

                    result.emplace(vk, KeyCodeInfo { vk, charWithoutModifiers, charWithShiftModifier });
                }

#elif defined(__linux__)

                Display *display = XOpenDisplay(nullptr);
                if (display)
                {
                    XkbDescPtr xkb = XkbGetMap(display, XkbAllClientInfoMask, XkbUseCoreKbd);

                    if (xkb)
                    {
                        const int maxKeyCode = xkb->max_key_code;
                        XkbStateRec state;
                        XkbGetState(display, XkbUseCoreKbd, &state);

                        for (int vk = 0x20; vk <= maxKeyCode; ++vk)
                        {
                            std::string charWithoutModifiers;
                            std::string charWithShiftModifier;

                            for (const auto &modifierName : modifierNames)
                            {
                                char buffer[32];
                                std::string symbols;
                                XKeyEvent event;
                                event.type = KeyPress;
                                event.display = display;
                                event.keycode = vk;
                                event.state = modifierName.empty() ? 0 : ShiftMask;
                                event.window = DefaultRootWindow(display);
                                int len = XLookupString(&event, buffer, sizeof(buffer), nullptr, nullptr);

                                if (len > 0)
                                {
                                    symbols.append(buffer, len);
                                }

                                if (!symbols.empty())
                                {
                                    bool shouldAdd = true;
                                    for (auto &c : symbols)
                                    {
                                        if (!std::isprint(c))
                                        {
                                            shouldAdd = false;
                                            break;
                                        }
                                    }

                                    if (shouldAdd)
                                    {
                                        if (modifierName.empty()) charWithoutModifiers = symbols;
                                        else charWithShiftModifier = symbols;
                                    }
                                }
                            }

                            if (!charWithoutModifiers.empty() || !charWithShiftModifier.empty())
                            {
                                result.emplace(vk, KeyCodeInfo { vk, charWithoutModifiers, charWithShiftModifier });
                            }
                        }

                        XkbFreeKeyboard(xkb, XkbAllClientInfoMask, True);
                    }

                    XCloseDisplay(display);
                }
#endif
                return result;
            }

        private:
#if defined(__APPLE__) && (TARGET_OS_OSX == 1)
            static std::string convertUniCharToUTF8(UniChar ch)
            {
                CFStringRef cfString = CFStringCreateWithBytes(
                        kCFAllocatorDefault,
                        reinterpret_cast<const UInt8*>(&ch),
                        sizeof(ch),
                        kCFStringEncodingUTF16,
                        false
                        );

                if (cfString)
                {
                    CFIndex length = CFStringGetLength(cfString);
                    CFIndex maxLength = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
                    char* utf8String = new char[maxLength + 1];

                    if (CFStringGetCString(cfString, utf8String, maxLength + 1, kCFStringEncodingUTF8))
                    {
                        std::string result(utf8String);
                        delete[] utf8String;
                        CFRelease(cfString);
                        return result;
                    }

                    CFRelease(cfString);
                }

                return "";
            }
#endif
    };
} // namespace mpc::controls
