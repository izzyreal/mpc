#include "KbMapping.hpp"

#include "Logger.hpp"
#include "hardware/ComponentId.h"

#include <cctype>

#if __APPLE__
#include <TargetConditionals.h>
#endif

using namespace mpc::controls;

KbMapping::KbMapping(const fs::path configDirectoryToUse) : configDirectory(configDirectoryToUse)
{
    importMapping();
}

void KbMapping::exportMapping()
{
    /*
	const auto path = configDirectory / "keys.txt";

    const std::string version = "v1\n";

    std::vector<char> bytes;

    for (auto &c : magicHeader) bytes.push_back(c);
    bytes.push_back('\n');
    for (auto &c : version) bytes.push_back(c);
	
    for (auto& mapping : idKeyMap)
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
    */
}

void KbMapping::importMapping()
{
    idKeyMap.clear();
    const auto path = configDirectory / "keys.txt";

    //if (!fs::exists(path))
    {
        MLOG("Initializing default key mapping...");
        initializeDefaults();
        return;
    }
/*    
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
                idKeyMap.emplace_back(label, static_cast<VmpcKeyCode>(parsedKeyCode));
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
        for (auto &mapping : idKeyMap)
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
            idKeyMap.emplace_back(label, static_cast<VmpcKeyCode>(i));
        }
    }

    if (shouldConvertPlatformKeycodesToVmpc)
    {
        exportMapping();
    }
    */
}

void KbMapping::initializeDefaults()
{
    idKeyMap.clear();

    using Id = hardware::ComponentId;

    idKeyMap.emplace_back(Id::CURSOR_LEFT_OR_DIGIT, VmpcKeyCode::VMPC_KEY_LeftArrow);
    idKeyMap.emplace_back(Id::CURSOR_RIGHT_OR_DIGIT, VmpcKeyCode::VMPC_KEY_RightArrow);
    idKeyMap.emplace_back(Id::CURSOR_UP, VmpcKeyCode::VMPC_KEY_UpArrow);
    idKeyMap.emplace_back(Id::CURSOR_DOWN, VmpcKeyCode::VMPC_KEY_DownArrow);
    idKeyMap.emplace_back(Id::REC, VmpcKeyCode::VMPC_KEY_ANSI_L);
    idKeyMap.emplace_back(Id::OVERDUB, VmpcKeyCode::VMPC_KEY_ANSI_Semicolon);
    idKeyMap.emplace_back(Id::STOP, VmpcKeyCode::VMPC_KEY_ANSI_Quote);
    idKeyMap.emplace_back(Id::PLAY, VmpcKeyCode::VMPC_KEY_Space);
    idKeyMap.emplace_back(Id::PLAY_START, VmpcKeyCode::VMPC_KEY_ANSI_Backslash);
    idKeyMap.emplace_back(Id::MAIN_SCREEN, VmpcKeyCode::VMPC_KEY_Escape);
    idKeyMap.emplace_back(Id::OPEN_WINDOW, VmpcKeyCode::VMPC_KEY_ANSI_I);
    idKeyMap.emplace_back(Id::PREV_STEP_OR_EVENT, VmpcKeyCode::VMPC_KEY_ANSI_Q);
    idKeyMap.emplace_back(Id::NEXT_STEP_OR_EVENT, VmpcKeyCode::VMPC_KEY_ANSI_W);
    idKeyMap.emplace_back(Id::GO_TO, VmpcKeyCode::VMPC_KEY_ANSI_E);
    idKeyMap.emplace_back(Id::PREV_BAR_START, VmpcKeyCode::VMPC_KEY_ANSI_R);
    idKeyMap.emplace_back(Id::NEXT_BAR_END, VmpcKeyCode::VMPC_KEY_ANSI_T);
    idKeyMap.emplace_back(Id::TAP_TEMPO_OR_NOTE_REPEAT, VmpcKeyCode::VMPC_KEY_ANSI_Y);
    idKeyMap.emplace_back(Id::NEXT_SEQ, VmpcKeyCode::VMPC_KEY_ANSI_LeftBracket);
    idKeyMap.emplace_back(Id::TRACK_MUTE, VmpcKeyCode::VMPC_KEY_ANSI_RightBracket);
    idKeyMap.emplace_back(Id::FULL_LEVEL_OR_CASE_SWITCH, VmpcKeyCode::VMPC_KEY_ANSI_O);
    idKeyMap.emplace_back(Id::SIXTEEN_LEVELS_OR_SPACE, VmpcKeyCode::VMPC_KEY_ANSI_P);
    idKeyMap.emplace_back(Id::F1, VmpcKeyCode::VMPC_KEY_F1);
    idKeyMap.emplace_back(Id::F2, VmpcKeyCode::VMPC_KEY_F2);
    idKeyMap.emplace_back(Id::F3, VmpcKeyCode::VMPC_KEY_F3);
    idKeyMap.emplace_back(Id::F4, VmpcKeyCode::VMPC_KEY_F4);
    idKeyMap.emplace_back(Id::F5, VmpcKeyCode::VMPC_KEY_F5);
    idKeyMap.emplace_back(Id::F6, VmpcKeyCode::VMPC_KEY_F6);
    idKeyMap.emplace_back(Id::SHIFT, VmpcKeyCode::VMPC_KEY_Shift);
    idKeyMap.emplace_back(Id::SHIFT, VmpcKeyCode::VMPC_KEY_LeftShift);   // duplicate SHIFTs
    idKeyMap.emplace_back(Id::SHIFT, VmpcKeyCode::VMPC_KEY_RightShift);  // duplicate SHIFTs
    idKeyMap.emplace_back(Id::ENTER_OR_SAVE, VmpcKeyCode::VMPC_KEY_Return);
    idKeyMap.emplace_back(Id::UNDO_SEQ, VmpcKeyCode::VMPC_KEY_F10);
    idKeyMap.emplace_back(Id::ERASE, VmpcKeyCode::VMPC_KEY_F8);
    idKeyMap.emplace_back(Id::AFTER_OR_ASSIGN, VmpcKeyCode::VMPC_KEY_F9);
    idKeyMap.emplace_back(Id::BANK_A, VmpcKeyCode::VMPC_KEY_Home);
    idKeyMap.emplace_back(Id::BANK_B, VmpcKeyCode::VMPC_KEY_End);
    idKeyMap.emplace_back(Id::BANK_C, VmpcKeyCode::VMPC_KEY_Insert);
    idKeyMap.emplace_back(Id::BANK_D, VmpcKeyCode::VMPC_KEY_Delete);
    idKeyMap.emplace_back(Id::NUM_0_OR_VMPC, VmpcKeyCode::VMPC_KEY_ANSI_0);
    idKeyMap.emplace_back(Id::NUM_1_OR_SONG, VmpcKeyCode::VMPC_KEY_ANSI_1);
    idKeyMap.emplace_back(Id::NUM_2_OR_MISC, VmpcKeyCode::VMPC_KEY_ANSI_2);
    idKeyMap.emplace_back(Id::NUM_3_OR_LOAD, VmpcKeyCode::VMPC_KEY_ANSI_3);
    idKeyMap.emplace_back(Id::NUM_4_OR_SAMPLE, VmpcKeyCode::VMPC_KEY_ANSI_4);
    idKeyMap.emplace_back(Id::NUM_5_OR_TRIM, VmpcKeyCode::VMPC_KEY_ANSI_5);
    idKeyMap.emplace_back(Id::NUM_6_OR_PROGRAM, VmpcKeyCode::VMPC_KEY_ANSI_6);
    idKeyMap.emplace_back(Id::NUM_7_OR_MIXER, VmpcKeyCode::VMPC_KEY_ANSI_7);
    idKeyMap.emplace_back(Id::NUM_8_OR_OTHER, VmpcKeyCode::VMPC_KEY_ANSI_8);
    idKeyMap.emplace_back(Id::NUM_9_OR_MIDI_SYNC, VmpcKeyCode::VMPC_KEY_ANSI_9);
    idKeyMap.emplace_back(Id::NUM_0_OR_VMPC, VmpcKeyCode::VMPC_KEY_ANSI_Keypad0);
    idKeyMap.emplace_back(Id::NUM_1_OR_SONG, VmpcKeyCode::VMPC_KEY_ANSI_Keypad1);
    idKeyMap.emplace_back(Id::NUM_2_OR_MISC, VmpcKeyCode::VMPC_KEY_ANSI_Keypad2);
    idKeyMap.emplace_back(Id::NUM_3_OR_LOAD, VmpcKeyCode::VMPC_KEY_ANSI_Keypad3);
    idKeyMap.emplace_back(Id::NUM_4_OR_SAMPLE, VmpcKeyCode::VMPC_KEY_ANSI_Keypad4);
    idKeyMap.emplace_back(Id::NUM_5_OR_TRIM, VmpcKeyCode::VMPC_KEY_ANSI_Keypad5);
    idKeyMap.emplace_back(Id::NUM_6_OR_PROGRAM, VmpcKeyCode::VMPC_KEY_ANSI_Keypad6);
    idKeyMap.emplace_back(Id::NUM_7_OR_MIXER, VmpcKeyCode::VMPC_KEY_ANSI_Keypad7);
    idKeyMap.emplace_back(Id::NUM_8_OR_OTHER, VmpcKeyCode::VMPC_KEY_ANSI_Keypad8);
    idKeyMap.emplace_back(Id::NUM_9_OR_MIDI_SYNC, VmpcKeyCode::VMPC_KEY_ANSI_Keypad9);
    idKeyMap.emplace_back(Id::PAD_1_OR_AB, VmpcKeyCode::VMPC_KEY_ANSI_Z);
    idKeyMap.emplace_back(Id::PAD_2_OR_CD, VmpcKeyCode::VMPC_KEY_ANSI_X);
    idKeyMap.emplace_back(Id::PAD_3_OR_EF, VmpcKeyCode::VMPC_KEY_ANSI_C);
    idKeyMap.emplace_back(Id::PAD_4_OR_GH, VmpcKeyCode::VMPC_KEY_ANSI_V);
    idKeyMap.emplace_back(Id::PAD_5_OR_IJ, VmpcKeyCode::VMPC_KEY_ANSI_A);
    idKeyMap.emplace_back(Id::PAD_6_OR_KL, VmpcKeyCode::VMPC_KEY_ANSI_S);
    idKeyMap.emplace_back(Id::PAD_7_OR_MN, VmpcKeyCode::VMPC_KEY_ANSI_D);
    idKeyMap.emplace_back(Id::PAD_8_OR_OP, VmpcKeyCode::VMPC_KEY_ANSI_F);
    idKeyMap.emplace_back(Id::PAD_9_OR_QR, VmpcKeyCode::VMPC_KEY_ANSI_B);
    idKeyMap.emplace_back(Id::PAD_10_OR_ST, VmpcKeyCode::VMPC_KEY_ANSI_N);
    idKeyMap.emplace_back(Id::PAD_11_OR_UV, VmpcKeyCode::VMPC_KEY_ANSI_M);
    idKeyMap.emplace_back(Id::PAD_12_OR_WX, VmpcKeyCode::VMPC_KEY_ANSI_Comma);
    idKeyMap.emplace_back(Id::PAD_13_OR_YZ, VmpcKeyCode::VMPC_KEY_ANSI_G);
    idKeyMap.emplace_back(Id::PAD_14_OR_AMPERSAND_OCTOTHORPE, VmpcKeyCode::VMPC_KEY_ANSI_H);
    idKeyMap.emplace_back(Id::PAD_15_OR_HYPHEN_EXCLAMATION_MARK, VmpcKeyCode::VMPC_KEY_ANSI_J);
    idKeyMap.emplace_back(Id::PAD_16_OR_PARENTHESES, VmpcKeyCode::VMPC_KEY_ANSI_K);
    //idKeyMap.emplace_back(Id::TAP_TEMPO_OR_NOTE_REPEAT, VmpcKeyCode::VMPC_KEY_ANSI_Minus);  // datawheel-down?
    //idKeyMap.emplace_back(Id::TAP_TEMPO_OR_NOTE_REPEAT, VmpcKeyCode::VMPC_KEY_ANSI_Equal);  // datawheel-up?
    //idKeyMap.emplace_back(Id::SHIFT, VmpcKeyCode::VMPC_KEY_Control);          // guessing ctrl → shift fallback
    //idKeyMap.emplace_back(Id::SHIFT, VmpcKeyCode::VMPC_KEY_OptionOrAlt);     // guessing alt → shift fallback
}

const VmpcKeyCode KbMapping::getKeyCodeFromLabel(const std::string& label) {
	for (auto& kv : idKeyMap)
    {
        //if (kv.first == label)
        {
           // return kv.second;
        }
    }

	return VmpcKeyCode::VMPC_KEY_UNKNOWN;
}

mpc::hardware::ComponentId KbMapping::getHardwareComponentIdAssociatedWithKeycode(const VmpcKeyCode keyCode)
{
	for (auto &mapping : idKeyMap)
    {
		if (keyCode == mapping.second)
        {
            return mapping.first;
        }
	}

	return mpc::hardware::ComponentId::NONE; 
}

void KbMapping::setKeyCodeForLabel(const VmpcKeyCode keyCode, const std::string& label)
{
    for (auto& kv : idKeyMap)
    {
        //if (kv.first == label) kv.second = keyCode;
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

std::vector<std::pair<mpc::hardware::ComponentId, VmpcKeyCode>>& KbMapping::getLabelKeyMap()
{
    return idKeyMap;
}

