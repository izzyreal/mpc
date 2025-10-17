#pragma once

#include <unordered_map>
#include <optional>

#include "controls/KeyCodeHelper.hpp"
#include "hardware/ComponentId.h"

namespace mpc::inputlogic
{

enum class Direction
{
    None,
    Positive,
    Negative
};

constexpr int toSign(Direction d)
{
    switch (d)
    {
        case Direction::Positive: return +1;
        case Direction::Negative: return -1;
        default:                  return 0;
    }
}

struct KeyboardBinding
{
    hardware::ComponentId componentId;
    Direction direction = Direction::None;
};

class KeyboardBindings
{
public:
    KeyboardBindings()
    {
        initializeDefaults();
    }

    std::vector<mpc::controls::VmpcKeyCode> lookupComponent(hardware::ComponentId id) const
    {
        std::vector<mpc::controls::VmpcKeyCode> result;

        for (const auto& [key, binding] : bindings)
        {
            if (binding.componentId == id)
            {
                if (binding.direction == Direction::None)
                {
                    result.push_back(key);
                    break; // only first one for non-directional bindings
                }

                if (binding.direction == Direction::Negative)
                    result.insert(result.begin(), key); // put first
                else
                    result.push_back(key);
            }
        }

        return result;
    }

    std::optional<KeyboardBinding> lookup(controls::VmpcKeyCode key) const
    {
        if (auto it = bindings.find(key); it != bindings.end())
        {
            return it->second;
        }

        return std::nullopt;
    }

private:
    std::map<controls::VmpcKeyCode, KeyboardBinding> bindings;

    void initializeDefaults()
    {
        using VmpcKeyCode = controls::VmpcKeyCode;
        using Id = hardware::ComponentId;

        bindings[VmpcKeyCode::VMPC_KEY_ANSI_Minus] = { Id::DATA_WHEEL, Direction::Negative };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_Equal] = { Id::DATA_WHEEL, Direction::Positive };

        // Navigation
        bindings[VmpcKeyCode::VMPC_KEY_LeftArrow]  = { Id::CURSOR_LEFT_OR_DIGIT };
        bindings[VmpcKeyCode::VMPC_KEY_RightArrow] = { Id::CURSOR_RIGHT_OR_DIGIT };
        bindings[VmpcKeyCode::VMPC_KEY_UpArrow]    = { Id::CURSOR_UP };
        bindings[VmpcKeyCode::VMPC_KEY_DownArrow]  = { Id::CURSOR_DOWN };

        // Transport / core controls
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_L]         = { Id::REC };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_Semicolon] = { Id::OVERDUB };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_Quote]     = { Id::STOP };
        bindings[VmpcKeyCode::VMPC_KEY_Space]          = { Id::PLAY };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_Backslash] = { Id::PLAY_START };
        bindings[VmpcKeyCode::VMPC_KEY_Escape]         = { Id::MAIN_SCREEN };

        // Editing / navigation
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_I] = { Id::OPEN_WINDOW };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_Q] = { Id::PREV_STEP_OR_EVENT };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_W] = { Id::NEXT_STEP_OR_EVENT };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_E] = { Id::GO_TO };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_R] = { Id::PREV_BAR_START };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_T] = { Id::NEXT_BAR_END };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_Y] = { Id::TAP_TEMPO_OR_NOTE_REPEAT };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_LeftBracket]  = { Id::NEXT_SEQ };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_RightBracket] = { Id::TRACK_MUTE };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_O] = { Id::FULL_LEVEL_OR_CASE_SWITCH };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_P] = { Id::SIXTEEN_LEVELS_OR_SPACE };

        // Function keys
        bindings[VmpcKeyCode::VMPC_KEY_F1] = { Id::F1 };
        bindings[VmpcKeyCode::VMPC_KEY_F2] = { Id::F2 };
        bindings[VmpcKeyCode::VMPC_KEY_F3] = { Id::F3 };
        bindings[VmpcKeyCode::VMPC_KEY_F4] = { Id::F4 };
        bindings[VmpcKeyCode::VMPC_KEY_F5] = { Id::F5 };
        bindings[VmpcKeyCode::VMPC_KEY_F6] = { Id::F6 };
        bindings[VmpcKeyCode::VMPC_KEY_F8] = { Id::ERASE };
        bindings[VmpcKeyCode::VMPC_KEY_F9] = { Id::AFTER_OR_ASSIGN };
        bindings[VmpcKeyCode::VMPC_KEY_F10] = { Id::UNDO_SEQ };

        // Modifiers
        bindings[VmpcKeyCode::VMPC_KEY_Shift]      = { Id::SHIFT };
        bindings[VmpcKeyCode::VMPC_KEY_LeftShift]  = { Id::SHIFT };
        bindings[VmpcKeyCode::VMPC_KEY_RightShift] = { Id::SHIFT };

        // Misc
        bindings[VmpcKeyCode::VMPC_KEY_Return] = { Id::ENTER_OR_SAVE };

        // Banks
        bindings[VmpcKeyCode::VMPC_KEY_Home]   = { Id::BANK_A };
        bindings[VmpcKeyCode::VMPC_KEY_End]    = { Id::BANK_B };
        bindings[VmpcKeyCode::VMPC_KEY_Insert] = { Id::BANK_C };
        bindings[VmpcKeyCode::VMPC_KEY_Delete] = { Id::BANK_D };

        // Numeric (top row + keypad)
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_0] = { Id::NUM_0_OR_VMPC };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_1] = { Id::NUM_1_OR_SONG };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_2] = { Id::NUM_2_OR_MISC };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_3] = { Id::NUM_3_OR_LOAD };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_4] = { Id::NUM_4_OR_SAMPLE };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_5] = { Id::NUM_5_OR_TRIM };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_6] = { Id::NUM_6_OR_PROGRAM };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_7] = { Id::NUM_7_OR_MIXER };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_8] = { Id::NUM_8_OR_OTHER };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_9] = { Id::NUM_9_OR_MIDI_SYNC };

        bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad0] = { Id::NUM_0_OR_VMPC };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad1] = { Id::NUM_1_OR_SONG };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad2] = { Id::NUM_2_OR_MISC };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad3] = { Id::NUM_3_OR_LOAD };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad4] = { Id::NUM_4_OR_SAMPLE };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad5] = { Id::NUM_5_OR_TRIM };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad6] = { Id::NUM_6_OR_PROGRAM };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad7] = { Id::NUM_7_OR_MIXER };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad8] = { Id::NUM_8_OR_OTHER };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad9] = { Id::NUM_9_OR_MIDI_SYNC };

        // Pads
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_Z]      = { Id::PAD_1_OR_AB };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_X]      = { Id::PAD_2_OR_CD };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_C]      = { Id::PAD_3_OR_EF };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_V]      = { Id::PAD_4_OR_GH };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_A]      = { Id::PAD_5_OR_IJ };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_S]      = { Id::PAD_6_OR_KL };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_D]      = { Id::PAD_7_OR_MN };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_F]      = { Id::PAD_8_OR_OP };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_B]      = { Id::PAD_9_OR_QR };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_N]      = { Id::PAD_10_OR_ST };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_M]      = { Id::PAD_11_OR_UV };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_Comma]  = { Id::PAD_12_OR_WX };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_G]      = { Id::PAD_13_OR_YZ };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_H]      = { Id::PAD_14_OR_AMPERSAND_OCTOTHORPE };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_J]      = { Id::PAD_15_OR_HYPHEN_EXCLAMATION_MARK };
        bindings[VmpcKeyCode::VMPC_KEY_ANSI_K]      = { Id::PAD_16_OR_PARENTHESES };
    }
};

} // namespace mpc::inputlogic
