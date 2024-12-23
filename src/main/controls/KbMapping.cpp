#include "KbMapping.hpp"

#include "Mpc.hpp"

using namespace mpc::controls;

KbMapping::KbMapping(mpc::Mpc& mpcToUse) : mpc(mpcToUse)
{
    importMapping();
}

void KbMapping::exportMapping() {
	const auto path = mpc.paths->configPath() / "keys.txt";

    std::vector<char> bytes;
	
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
    
    auto bytes = get_file_data(path);

    std::string label;
    std::string keyCode;
    bool parsingLabel = true;

    for (int i = 0; i < bytes.size(); i++)
    {
        if (bytes[i] != ' ' && parsingLabel)
        {
            label += bytes[i];
            continue;
        }
        
        if (bytes[i] == ' ')
        {
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

    migrateV0_4_4MappingToV0_5();
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

const std::string KbMapping::getKeyCodeString(const VmpcKeyCode keyCode, const bool oneChar)
{
    return KeyCodeHelper::vmpcKeyCodeNames.at(keyCode);
}

void KbMapping::setKeyCodeForLabel(const const VmpcKeyCode keyCode, const std::string& label)
{
    for (auto& kv : labelKeyMap)
    {
        if (kv.first == label) kv.second = keyCode;
    }
}

const VmpcKeyCode KbMapping::getPreviousKeyCode(const VmpcKeyCode keyCode)
{
    if (static_cast<int>(keyCode) - 1  >= 0)
    {
        return static_cast<VmpcKeyCode>(static_cast<int>(keyCode) - 1);
    }

    return static_cast<VmpcKeyCode>(0);
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

void KbMapping::migrateV0_4_4MappingToV0_5()
{
    auto importedMap = labelKeyMap;

    initializeDefaults();

    for (auto& mapping : labelKeyMap)
    {
        for (auto& mapping2 : importedMap)
        {
            if (mapping.first == mapping2.first)
            {
                mapping.second = mapping2.second;
            }
        }
    }
}
