#include "input/KeyboardBindings.hpp"

#include <cassert>

using namespace mpc::input;

mpc::hardware::ComponentId KeyboardBinding::getComponentId() const
{
    if (hardware::componentLabelToId.count(componentLabel) == 0)
    {
        return hardware::ComponentId::NONE;
    }

    return hardware::componentLabelToId.at(componentLabel);
}

KeyboardBindings::KeyboardBindings()
{
    initializeDefaults();
}

std::vector<VmpcKeyCode>
KeyboardBindings::lookupComponent(hardware::ComponentId id) const
{
    std::vector<VmpcKeyCode> result;

    for (const auto &[key, binding] : bindings)
    {
        if (binding.componentLabel == hardware::componentIdToLabel.at(id))
        {
            if (binding.direction == Direction::NoDirection)
            {
                result.push_back(key);
                break; // only first one for non-directional bindings
            }

            if (binding.direction == Direction::Negative)
            {
                result.insert(result.begin(), key);
            }
            else
            {
                result.push_back(key);
            }
        }
    }

    return result;
}

std::optional<KeyboardBinding> KeyboardBindings::lookup(VmpcKeyCode key) const
{
    if (auto it = bindings.find(key); it != bindings.end())
    {
        return it->second;
    }
    return std::nullopt;
}

void KeyboardBindings::initializeDefaults()
{
    using VmpcKeyCode = VmpcKeyCode;
    using Id = hardware::ComponentId;

    std::function getLabel = [&](Id id)
    {
        return hardware::componentIdToLabel.at(id);
    };

    bindings[VmpcKeyCode::VMPC_KEY_ANSI_Minus] = {getLabel(Id::DATA_WHEEL),
                                                  Direction::Negative};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_Equal] = {getLabel(Id::DATA_WHEEL),
                                                  Direction::Positive};

    // Navigation
    bindings[VmpcKeyCode::VMPC_KEY_LeftArrow] = {
        getLabel(Id::CURSOR_LEFT_OR_DIGIT)};
    bindings[VmpcKeyCode::VMPC_KEY_RightArrow] = {
        getLabel(Id::CURSOR_RIGHT_OR_DIGIT)};
    bindings[VmpcKeyCode::VMPC_KEY_UpArrow] = {getLabel(Id::CURSOR_UP)};
    bindings[VmpcKeyCode::VMPC_KEY_DownArrow] = {getLabel(Id::CURSOR_DOWN)};

    // Transport / core controls
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_L] = {getLabel(Id::REC)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_Semicolon] = {getLabel(Id::OVERDUB)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_Quote] = {getLabel(Id::STOP)};
    bindings[VmpcKeyCode::VMPC_KEY_Space] = {getLabel(Id::PLAY)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_Backslash] = {getLabel(Id::PLAY_START)};
    bindings[VmpcKeyCode::VMPC_KEY_Escape] = {getLabel(Id::MAIN_SCREEN)};

    // Editing / navigation
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_I] = {getLabel(Id::OPEN_WINDOW)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_Q] = {getLabel(Id::PREV_STEP_OR_EVENT)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_W] = {getLabel(Id::NEXT_STEP_OR_EVENT)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_E] = {getLabel(Id::GO_TO)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_R] = {getLabel(Id::PREV_BAR_OR_START)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_T] = {getLabel(Id::NEXT_BAR_OR_END)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_Y] = {
        getLabel(Id::TAP_TEMPO_OR_NOTE_REPEAT)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_LeftBracket] = {getLabel(Id::NEXT_SEQ)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_RightBracket] = {
        getLabel(Id::TRACK_MUTE)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_O] = {
        getLabel(Id::FULL_LEVEL_OR_CASE_SWITCH)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_P] = {
        getLabel(Id::SIXTEEN_LEVELS_OR_SPACE)};

    // Function keys
    bindings[VmpcKeyCode::VMPC_KEY_F1] = {getLabel(Id::F1)};
    bindings[VmpcKeyCode::VMPC_KEY_F2] = {getLabel(Id::F2)};
    bindings[VmpcKeyCode::VMPC_KEY_F3] = {getLabel(Id::F3)};
    bindings[VmpcKeyCode::VMPC_KEY_F4] = {getLabel(Id::F4)};
    bindings[VmpcKeyCode::VMPC_KEY_F5] = {getLabel(Id::F5)};
    bindings[VmpcKeyCode::VMPC_KEY_F6] = {getLabel(Id::F6)};
    bindings[VmpcKeyCode::VMPC_KEY_F8] = {getLabel(Id::ERASE)};
    bindings[VmpcKeyCode::VMPC_KEY_F9] = {getLabel(Id::AFTER_OR_ASSIGN)};
    bindings[VmpcKeyCode::VMPC_KEY_F10] = {getLabel(Id::UNDO_SEQ)};

    // Modifiers
    bindings[VmpcKeyCode::VMPC_KEY_Shift] = {getLabel(Id::SHIFT)};
    bindings[VmpcKeyCode::VMPC_KEY_LeftShift] = {getLabel(Id::SHIFT)};
    bindings[VmpcKeyCode::VMPC_KEY_RightShift] = {getLabel(Id::SHIFT)};

    // Misc
    bindings[VmpcKeyCode::VMPC_KEY_Return] = {getLabel(Id::ENTER_OR_SAVE)};

    // Banks
    bindings[VmpcKeyCode::VMPC_KEY_Home] = {getLabel(Id::BANK_A)};
    bindings[VmpcKeyCode::VMPC_KEY_End] = {getLabel(Id::BANK_B)};
    bindings[VmpcKeyCode::VMPC_KEY_Insert] = {getLabel(Id::BANK_C)};
    bindings[VmpcKeyCode::VMPC_KEY_Delete] = {getLabel(Id::BANK_D)};

    // Numeric (top row + keypad)
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_0] = {getLabel(Id::NUM_0_OR_VMPC)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_1] = {getLabel(Id::NUM_1_OR_SONG)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_2] = {getLabel(Id::NUM_2_OR_MISC)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_3] = {getLabel(Id::NUM_3_OR_LOAD)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_4] = {getLabel(Id::NUM_4_OR_SAMPLE)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_5] = {getLabel(Id::NUM_5_OR_TRIM)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_6] = {getLabel(Id::NUM_6_OR_PROGRAM)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_7] = {getLabel(Id::NUM_7_OR_MIXER)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_8] = {getLabel(Id::NUM_8_OR_OTHER)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_9] = {getLabel(Id::NUM_9_OR_MIDI_SYNC)};

    bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad0] = {
        getLabel(Id::NUM_0_OR_VMPC)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad1] = {
        getLabel(Id::NUM_1_OR_SONG)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad2] = {
        getLabel(Id::NUM_2_OR_MISC)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad3] = {
        getLabel(Id::NUM_3_OR_LOAD)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad4] = {
        getLabel(Id::NUM_4_OR_SAMPLE)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad5] = {
        getLabel(Id::NUM_5_OR_TRIM)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad6] = {
        getLabel(Id::NUM_6_OR_PROGRAM)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad7] = {
        getLabel(Id::NUM_7_OR_MIXER)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad8] = {
        getLabel(Id::NUM_8_OR_OTHER)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_Keypad9] = {
        getLabel(Id::NUM_9_OR_MIDI_SYNC)};

    // Pads
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_Z] = {getLabel(Id::PAD_1_OR_AB)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_X] = {getLabel(Id::PAD_2_OR_CD)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_C] = {getLabel(Id::PAD_3_OR_EF)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_V] = {getLabel(Id::PAD_4_OR_GH)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_A] = {getLabel(Id::PAD_5_OR_IJ)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_S] = {getLabel(Id::PAD_6_OR_KL)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_D] = {getLabel(Id::PAD_7_OR_MN)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_F] = {getLabel(Id::PAD_8_OR_OP)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_B] = {getLabel(Id::PAD_9_OR_QR)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_N] = {getLabel(Id::PAD_10_OR_ST)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_M] = {getLabel(Id::PAD_11_OR_UV)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_Comma] = {getLabel(Id::PAD_12_OR_WX)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_G] = {getLabel(Id::PAD_13_OR_YZ)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_H] = {
        getLabel(Id::PAD_14_OR_AMPERSAND_OCTOTHORPE)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_J] = {
        getLabel(Id::PAD_15_OR_HYPHEN_EXCLAMATION_MARK)};
    bindings[VmpcKeyCode::VMPC_KEY_ANSI_K] = {
        getLabel(Id::PAD_16_OR_PARENTHESES)};
}

KeyboardBindingsData KeyboardBindings::getKeyboardBindingsData() const
{
    return bindings;
}

void KeyboardBindings::setBinding(const hardware::ComponentId id,
                                  const Direction direction,
                                  const mpc::input::VmpcKeyCode key)
{
    setBinding(hardware::componentIdToLabel.at(id), direction, key);
}

void KeyboardBindings::setBinding(const std::string componentLabel, const Direction direction,
                                  const VmpcKeyCode key)
{
    assert(hardware::componentLabelToId.count(componentLabel) > 0);
    bindings[key] = {componentLabel, direction};
}
