#include "KbMapping.hpp"

#include "Mpc.hpp"
#include <cctype>

#if __APPLE__
#include <TargetConditionals.h>
#endif

using namespace mpc::controls;

KbMapping::KbMapping(mpc::Mpc& mpcToUse) : mpc(mpcToUse)
{
    importMapping();
}

void KbMapping::exportMapping()
{
	const auto path = mpc.paths->configPath() / "keys.txt";

    const std::string version = "v1\n";

    std::vector<char> bytes;

    for (auto &c : magicHeader) bytes.push_back(c);
    bytes.push_back('\n');
    for (auto &c : version) bytes.push_back(c);
	
    for (auto& mapping : labelKeyMap)
    {
		for (char& c : mapping.first)
			bytes.push_back(c);
        
		bytes.push_back(' ');
		
        auto keyCode = std::to_string(static_cast<int>(mapping.second));
		
        for (char& c : keyCode)
			bytes.push_back(c);
		
        bytes.push_back('\n');
	}
    
	set_file_data(path, bytes);
}

void KbMapping::importMapping()
{
    labelKeyMap.clear();
    const auto path = mpc.paths->configPath() / "keys.txt";

    if (!fs::exists(path))
    {
        MLOG("Initializing default key mapping...");
        initializeDefaults();
        return;
    }
    
    const auto bytes = get_file_data(path);

    std::string header;
    std::string version;

    bool headerHasBeenParsed = false;
    bool versionHasBeenParsed = false;

    int byteIndex = 0;

    while ((!headerHasBeenParsed || !versionHasBeenParsed) && byteIndex < bytes.size())
    {
        if (!headerHasBeenParsed)
        {
            if (bytes[byteIndex] == '\n')
            {
                headerHasBeenParsed = true;
                byteIndex++;
            }
            else
            {
                header += bytes[byteIndex++];
            }
        }
        else if (!versionHasBeenParsed)
        {
            if (bytes[byteIndex] == '\n')
            {
                versionHasBeenParsed = true;
                byteIndex++;
                break;
            }
            else
            {
                version += bytes[byteIndex++];
            }
        }
    }

    bool shouldConvertPlatformKeycodesToVmpc = false;

    if (header != magicHeader)
    {
        // keys.txt is in an older format than the one introduced with VMPC2000XL 0.9.0.
        // In this format, we were still persisting platform-specific keycodes.
        //
        // In the new format, we are persisting VMPC2000XL keycodes. It also starts the
        // file with a magic header and a version.
        //
        // So, in this case, we'll convert the keycodes to VMPC2000XL ones as we go through
        // the file. We must also reset the parser's byte index.
        byteIndex = 0;
        shouldConvertPlatformKeycodesToVmpc = true;
    }

    std::string label;
    std::string keyCode;
    bool parsingLabel = true;

    for (int i = byteIndex; i < bytes.size(); i++)
    {
        if (bytes[i] != ' ' && parsingLabel)
        {
            label += bytes[i];
            continue;
        }
        
        if (bytes[i] == ' ')
        {
            if (i + 1 < bytes.size() && bytes[i+1] == '(')
            {
                // Here we do a fix for https://github.com/izzyreal/mpc/issues/258 in case
                // the user has already persisted a corrupt keys.txt
                label += '_';
                continue;
            }

            parsingLabel = false;
            continue;
        }
        
        if (bytes[i] != ' ' && bytes[i] != '\n' && !parsingLabel)
        {
            keyCode += bytes[i];
            continue;
        }
        
        if (bytes[i] == '\n' || i == bytes.size() - 1)
        {
            int parsedKeyCode = -1;
            
            try
            {
                parsedKeyCode = stoi(keyCode);

                if (shouldConvertPlatformKeycodesToVmpc && parsedKeyCode != -1)
                {
                    switch (parsedKeyCode)
                    {
#if TARGET_OS_OSX == 1
                        case -16:
                            parsedKeyCode = static_cast<int>(VmpcKeyCode::VMPC_KEY_Insert);
                            break;
                        case -17:
                            parsedKeyCode = static_cast<int>(VmpcKeyCode::VMPC_KEY_Delete);
                            break;
#endif
                        default:
                            parsedKeyCode = static_cast<int>(KeyCodeHelper::getVmpcFromPlatformKeyCode(parsedKeyCode));
                            break;
                    }
                }
            }
            catch (const std::exception& e)
            {
                MLOG("There is an issue with the stored keyboard mapping. Best reset your mapping and start from scratch. We'll try to make the best of what is stored though.");
            }
            
            if (parsedKeyCode != -1)
            {
                labelKeyMap.emplace_back(label, static_cast<VmpcKeyCode>(parsedKeyCode));
            }
            
            label.clear();
            keyCode.clear();
            parsingLabel = true;
        }
    }

    for (int i = static_cast<int>(VmpcKeyCode::VMPC_KEY_ANSI_Keypad0);
         i <= static_cast<int>(VmpcKeyCode::VMPC_KEY_ANSI_Keypad9);
         i++)
    {
        bool keypadKeyIsAlreadyAssigned = false;
        for (auto &mapping : labelKeyMap)
        {
            if (static_cast<int>(mapping.second) == i)
            {
                keypadKeyIsAlreadyAssigned = true;
                break;
            }
        }

        if (!keypadKeyIsAlreadyAssigned)
        {
            const auto label = std::to_string(i - static_cast<int>(VmpcKeyCode::VMPC_KEY_ANSI_Keypad0)) + "_(extra)";
            labelKeyMap.emplace_back(label, static_cast<VmpcKeyCode>(i));
        }
    }

    if (shouldConvertPlatformKeycodesToVmpc)
    {
        exportMapping();
    }
}

void KbMapping::initializeDefaults()
{
    labelKeyMap.clear();
    labelKeyMap.emplace_back("left", VmpcKeyCode::VMPC_KEY_LeftArrow);
    labelKeyMap.emplace_back("right", VmpcKeyCode::VMPC_KEY_RightArrow);
    labelKeyMap.emplace_back("up", VmpcKeyCode::VMPC_KEY_UpArrow);
    labelKeyMap.emplace_back("down", VmpcKeyCode::VMPC_KEY_DownArrow);
    labelKeyMap.emplace_back("rec", VmpcKeyCode::VMPC_KEY_ANSI_L);
    labelKeyMap.emplace_back("overdub", VmpcKeyCode::VMPC_KEY_ANSI_Semicolon);
    labelKeyMap.emplace_back("stop", VmpcKeyCode::VMPC_KEY_ANSI_Quote);
    labelKeyMap.emplace_back("play", VmpcKeyCode::VMPC_KEY_Space);
    labelKeyMap.emplace_back("play-start", VmpcKeyCode::VMPC_KEY_ANSI_Backslash);
    labelKeyMap.emplace_back("main-screen", VmpcKeyCode::VMPC_KEY_Escape);
    labelKeyMap.emplace_back("open-window", VmpcKeyCode::VMPC_KEY_ANSI_I);
    labelKeyMap.emplace_back("prev-step-event", VmpcKeyCode::VMPC_KEY_ANSI_Q);
    labelKeyMap.emplace_back("next-step-event", VmpcKeyCode::VMPC_KEY_ANSI_W);
    labelKeyMap.emplace_back("go-to", VmpcKeyCode::VMPC_KEY_ANSI_E);
    labelKeyMap.emplace_back("prev-bar-start", VmpcKeyCode::VMPC_KEY_ANSI_R);
    labelKeyMap.emplace_back("next-bar-end", VmpcKeyCode::VMPC_KEY_ANSI_T);
    labelKeyMap.emplace_back("tap", VmpcKeyCode::VMPC_KEY_ANSI_Y);
    labelKeyMap.emplace_back("next-seq", VmpcKeyCode::VMPC_KEY_ANSI_LeftBracket);
    labelKeyMap.emplace_back("track-mute", VmpcKeyCode::VMPC_KEY_ANSI_RightBracket);
    labelKeyMap.emplace_back("full-level", VmpcKeyCode::VMPC_KEY_ANSI_O);
    labelKeyMap.emplace_back("sixteen-levels", VmpcKeyCode::VMPC_KEY_ANSI_P);
    labelKeyMap.emplace_back("f1", VmpcKeyCode::VMPC_KEY_F1);
    labelKeyMap.emplace_back("f2", VmpcKeyCode::VMPC_KEY_F2);
    labelKeyMap.emplace_back("f3", VmpcKeyCode::VMPC_KEY_F3);
    labelKeyMap.emplace_back("f4", VmpcKeyCode::VMPC_KEY_F4);
    labelKeyMap.emplace_back("f5", VmpcKeyCode::VMPC_KEY_F5);
    labelKeyMap.emplace_back("f6", VmpcKeyCode::VMPC_KEY_F6);
    labelKeyMap.emplace_back("shift_#1", VmpcKeyCode::VMPC_KEY_Shift);
    labelKeyMap.emplace_back("shift_#2", VmpcKeyCode::VMPC_KEY_LeftShift);
    labelKeyMap.emplace_back("shift_#3", VmpcKeyCode::VMPC_KEY_RightShift);
    labelKeyMap.emplace_back("enter", VmpcKeyCode::VMPC_KEY_Return);
    labelKeyMap.emplace_back("undo-seq", VmpcKeyCode::VMPC_KEY_F10);
    labelKeyMap.emplace_back("erase", VmpcKeyCode::VMPC_KEY_F8);
    labelKeyMap.emplace_back("after", VmpcKeyCode::VMPC_KEY_F9);
    labelKeyMap.emplace_back("bank-a", VmpcKeyCode::VMPC_KEY_Home);
    labelKeyMap.emplace_back("bank-b", VmpcKeyCode::VMPC_KEY_End);
    labelKeyMap.emplace_back("bank-c", VmpcKeyCode::VMPC_KEY_Insert);
    labelKeyMap.emplace_back("bank-d", VmpcKeyCode::VMPC_KEY_Delete);
    labelKeyMap.emplace_back("0", VmpcKeyCode::VMPC_KEY_ANSI_0);
    labelKeyMap.emplace_back("1", VmpcKeyCode::VMPC_KEY_ANSI_1);
    labelKeyMap.emplace_back("2", VmpcKeyCode::VMPC_KEY_ANSI_2);
    labelKeyMap.emplace_back("3", VmpcKeyCode::VMPC_KEY_ANSI_3);
    labelKeyMap.emplace_back("4", VmpcKeyCode::VMPC_KEY_ANSI_4);
    labelKeyMap.emplace_back("5", VmpcKeyCode::VMPC_KEY_ANSI_5);
    labelKeyMap.emplace_back("6", VmpcKeyCode::VMPC_KEY_ANSI_6);
    labelKeyMap.emplace_back("7", VmpcKeyCode::VMPC_KEY_ANSI_7);
    labelKeyMap.emplace_back("8", VmpcKeyCode::VMPC_KEY_ANSI_8);
    labelKeyMap.emplace_back("9", VmpcKeyCode::VMPC_KEY_ANSI_9);
    labelKeyMap.emplace_back("0_(extra)", VmpcKeyCode::VMPC_KEY_ANSI_Keypad0);
    labelKeyMap.emplace_back("1_(extra)", VmpcKeyCode::VMPC_KEY_ANSI_Keypad1);
    labelKeyMap.emplace_back("2_(extra)", VmpcKeyCode::VMPC_KEY_ANSI_Keypad2);
    labelKeyMap.emplace_back("3_(extra)", VmpcKeyCode::VMPC_KEY_ANSI_Keypad3);
    labelKeyMap.emplace_back("4_(extra)", VmpcKeyCode::VMPC_KEY_ANSI_Keypad4);
    labelKeyMap.emplace_back("5_(extra)", VmpcKeyCode::VMPC_KEY_ANSI_Keypad5);
    labelKeyMap.emplace_back("6_(extra)", VmpcKeyCode::VMPC_KEY_ANSI_Keypad6);
    labelKeyMap.emplace_back("7_(extra)", VmpcKeyCode::VMPC_KEY_ANSI_Keypad7);
    labelKeyMap.emplace_back("8_(extra)", VmpcKeyCode::VMPC_KEY_ANSI_Keypad8);
    labelKeyMap.emplace_back("9_(extra)", VmpcKeyCode::VMPC_KEY_ANSI_Keypad9);
    labelKeyMap.emplace_back("pad-1", VmpcKeyCode::VMPC_KEY_ANSI_Z);
    labelKeyMap.emplace_back("pad-2", VmpcKeyCode::VMPC_KEY_ANSI_X);
    labelKeyMap.emplace_back("pad-3", VmpcKeyCode::VMPC_KEY_ANSI_C);
    labelKeyMap.emplace_back("pad-4", VmpcKeyCode::VMPC_KEY_ANSI_V);
    labelKeyMap.emplace_back("pad-5", VmpcKeyCode::VMPC_KEY_ANSI_A);
    labelKeyMap.emplace_back("pad-6", VmpcKeyCode::VMPC_KEY_ANSI_S);
    labelKeyMap.emplace_back("pad-7", VmpcKeyCode::VMPC_KEY_ANSI_D);
    labelKeyMap.emplace_back("pad-8", VmpcKeyCode::VMPC_KEY_ANSI_F);
    labelKeyMap.emplace_back("pad-9", VmpcKeyCode::VMPC_KEY_ANSI_B);
    labelKeyMap.emplace_back("pad-10", VmpcKeyCode::VMPC_KEY_ANSI_N);
    labelKeyMap.emplace_back("pad-11", VmpcKeyCode::VMPC_KEY_ANSI_M);
    labelKeyMap.emplace_back("pad-12", VmpcKeyCode::VMPC_KEY_ANSI_Comma);
    labelKeyMap.emplace_back("pad-13", VmpcKeyCode::VMPC_KEY_ANSI_G);
    labelKeyMap.emplace_back("pad-14", VmpcKeyCode::VMPC_KEY_ANSI_H);
    labelKeyMap.emplace_back("pad-15", VmpcKeyCode::VMPC_KEY_ANSI_J);
    labelKeyMap.emplace_back("pad-16", VmpcKeyCode::VMPC_KEY_ANSI_K);
    labelKeyMap.emplace_back("datawheel-down", VmpcKeyCode::VMPC_KEY_ANSI_Minus);
    labelKeyMap.emplace_back("datawheel-up", VmpcKeyCode::VMPC_KEY_ANSI_Equal);
    labelKeyMap.emplace_back("ctrl", VmpcKeyCode::VMPC_KEY_Control);
    labelKeyMap.emplace_back("alt", VmpcKeyCode::VMPC_KEY_OptionOrAlt);
}

const VmpcKeyCode KbMapping::getKeyCodeFromLabel(const std::string& label) {
	for (auto& kv : labelKeyMap)
    {
        if (kv.first == label)
        {
            return kv.second;
        }
    }

	return VmpcKeyCode::VMPC_KEY_UNKNOWN;
}

std::string KbMapping::getHardwareComponentLabelAssociatedWithKeycode(const VmpcKeyCode keyCode) {
	for (std::pair<std::string, VmpcKeyCode>& mapping : labelKeyMap) {
		std::string label = mapping.first;
		if (keyCode == mapping.second)
        {
            std::string sanitized;
            for (auto& c : label)
            {
                if (c == '_') break;
                sanitized.push_back(c);
            }
            return sanitized;
        }
	}
	return "";
}

void KbMapping::setKeyCodeForLabel(const VmpcKeyCode keyCode, const std::string& label)
{
    for (auto& kv : labelKeyMap)
    {
        if (kv.first == label) kv.second = keyCode;
    }
}

const VmpcKeyCode KbMapping::getPreviousKeyCode(const VmpcKeyCode keyCode)
{
    if (static_cast<int>(keyCode) - 1  >= -1)
    {
        return static_cast<VmpcKeyCode>(static_cast<int>(keyCode) - 1);
    }

    return static_cast<VmpcKeyCode>(-1);
}

const VmpcKeyCode KbMapping::getNextKeyCode(const VmpcKeyCode keyCode)
{
    if (static_cast<int>(keyCode) + 1 < static_cast<int>(VmpcKeyCode::VMPC_KEY_ENUMERATOR_SIZE))
    {
        return static_cast<VmpcKeyCode>(static_cast<int>(keyCode) + 1);
    }

    return static_cast<VmpcKeyCode>(static_cast<int>(VmpcKeyCode::VMPC_KEY_ENUMERATOR_SIZE) - 1);
}

std::vector<std::pair<std::string, VmpcKeyCode>>& KbMapping::getLabelKeyMap()
{
    return labelKeyMap;
}

