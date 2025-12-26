#include "input/KeyboardBindings.hpp"

#include <cassert>
#include <set>

using namespace mpc::input;
using namespace mpc::hardware;

ComponentId KeyBinding::getComponentId() const
{
    if (componentLabelToId.count(componentLabel) == 0)
    {
        return NONE;
    }

    return componentLabelToId.at(componentLabel);
}

int KeyBinding::getPlatformKeyCode() const
{
    return KeyCodeHelper::getPlatformFromVmpcKeyCode(keyCode);
}

KeyboardBindings::KeyboardBindings()
{
    initializeDefaults();
}

KeyboardBindings::KeyboardBindings(const KeyboardBindingsData &bindingsData)
    : bindings(bindingsData)
{
}

bool KeyboardBindings::isSameAs(const KeyboardBindingsData &other) const
{
    return bindings == other;
}

bool KeyboardBindings::hasNoDuplicateVmpcKeyCodes() const
{
    std::set<VmpcKeyCode> seen;
    for (const auto &binding : bindings)
    {
        if (!seen.insert(binding.keyCode).second)
        {
            return false;
        }
    }
    return true;
}

bool KeyboardBindings::hasNoDuplicateKeyBindings() const
{
    std::set<KeyBinding> seen;

    for (auto binding : bindings)
    {
        if (!seen.insert(binding).second)
        {
            return false;
        }
    }

    return true;
}

bool KeyboardBindings::hasNoDuplicates() const
{
    return hasNoDuplicateKeyBindings() && hasNoDuplicateVmpcKeyCodes();
}

std::vector<VmpcKeyCode>
KeyboardBindings::lookupComponentKeyCodes(const ComponentId id) const
{
    return lookupComponentKeyCodes(componentIdToLabel.at(id));
}

std::vector<VmpcKeyCode> KeyboardBindings::lookupComponentKeyCodes(
    const std::string &componentLabel) const
{
    std::vector<VmpcKeyCode> result;

    for (const auto &binding : bindings)
    {
        if (binding.componentLabel == componentLabel)
        {
            if (binding.direction == Direction::NoDirection)
            {
                result.push_back(binding.keyCode);
                break; // only first one for non-directional bindings
            }

            if (binding.direction == Direction::Negative)
            {
                result.insert(result.begin(), binding.keyCode);
            }
            else
            {
                result.push_back(binding.keyCode);
            }
        }
    }

    return result;
}

std::vector<KeyBinding *>
KeyboardBindings::lookupComponentBindings(const std::string &componentLabel)
{
    std::vector<KeyBinding *> result;

    for (auto &binding : bindings)
    {
        if (binding.componentLabel == componentLabel)
        {
            result.push_back(&binding);
        }
    }

    return result;
}

KeyBinding *
KeyboardBindings::lookupFirstKeyCodeBinding(const VmpcKeyCode vmpcKeyCode)
{
    const auto allKeyCodeBindings = lookupKeyCodeBindings(vmpcKeyCode);

    if (allKeyCodeBindings.empty())
    {
        return nullptr;
    }

    return allKeyCodeBindings.front();
}

std::vector<KeyBinding *>
KeyboardBindings::lookupKeyCodeBindings(const VmpcKeyCode vmpcKeyCode)
{
    std::vector<KeyBinding *> result;
    for (auto &binding : bindings)
    {
        if (binding.keyCode == vmpcKeyCode)
        {
            result.push_back(&binding);
        }
    }

    return result;
}

void KeyboardBindings::initializeDefaults()
{
    bindings.clear();

    using VmpcKeyCode = VmpcKeyCode;
    using Id = ComponentId;

    const std::function getLabel = [&](const Id id)
    {
        return componentIdToLabel.at(id);
    };

    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_Minus, getLabel(DATA_WHEEL),
                        Direction::Negative});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_Equal, getLabel(DATA_WHEEL),
                        Direction::Positive});

    // Navigation
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_LeftArrow, getLabel(CURSOR_LEFT_OR_DIGIT)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_RightArrow, getLabel(CURSOR_RIGHT_OR_DIGIT)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_UpArrow, getLabel(CURSOR_UP)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_DownArrow, getLabel(CURSOR_DOWN)});

    // Transport / core controls
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_L, getLabel(REC)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_Semicolon, getLabel(OVERDUB)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_Quote, getLabel(STOP)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_Space, getLabel(PLAY)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_Backslash, getLabel(PLAY_START)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_Escape, getLabel(MAIN_SCREEN)});

    // Editing / navigation
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_I, getLabel(OPEN_WINDOW)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_Q, getLabel(PREV_STEP_OR_EVENT)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_W, getLabel(NEXT_STEP_OR_EVENT)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_E, getLabel(GO_TO)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_R, getLabel(PREV_BAR_OR_START)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_T, getLabel(NEXT_BAR_OR_END)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_Y, getLabel(TAP_TEMPO_OR_NOTE_REPEAT)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_LeftBracket, getLabel(NEXT_SEQ)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_RightBracket, getLabel(TRACK_MUTE)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_O, getLabel(FULL_LEVEL_OR_CASE_SWITCH)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_P, getLabel(SIXTEEN_LEVELS_OR_SPACE)});

    // Function keys
    bindings.push_back({VmpcKeyCode::VMPC_KEY_F1, getLabel(F1)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_F2, getLabel(F2)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_F3, getLabel(F3)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_F4, getLabel(F4)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_F5, getLabel(F5)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_F6, getLabel(F6)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_F8, getLabel(ERASE)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_F9, getLabel(AFTER_OR_ASSIGN)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_F10, getLabel(UNDO_SEQ)});

    // Modifiers
    bindings.push_back({VmpcKeyCode::VMPC_KEY_Shift, getLabel(SHIFT)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_LeftShift, getLabel(SHIFT)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_RightShift, getLabel(SHIFT)});

    // Misc
    bindings.push_back({VmpcKeyCode::VMPC_KEY_Return, getLabel(ENTER_OR_SAVE)});

    // Banks
    bindings.push_back({VmpcKeyCode::VMPC_KEY_Home, getLabel(BANK_A)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_End, getLabel(BANK_B)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_Insert, getLabel(BANK_C)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_Delete, getLabel(BANK_D)});

    // Numeric (top row + keypad)
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_0, getLabel(NUM_0_OR_VMPC)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_1, getLabel(NUM_1_OR_SONG)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_2, getLabel(NUM_2_OR_MISC)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_3, getLabel(NUM_3_OR_LOAD)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_4, getLabel(NUM_4_OR_SAMPLE)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_5, getLabel(NUM_5_OR_TRIM)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_6, getLabel(NUM_6_OR_PROGRAM)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_7, getLabel(NUM_7_OR_MIXER)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_8, getLabel(NUM_8_OR_OTHER)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_9, getLabel(NUM_9_OR_MIDI_SYNC)});

    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_Keypad0, getLabel(NUM_0_OR_VMPC)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_Keypad1, getLabel(NUM_1_OR_SONG)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_Keypad2, getLabel(NUM_2_OR_MISC)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_Keypad3, getLabel(NUM_3_OR_LOAD)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_Keypad4, getLabel(NUM_4_OR_SAMPLE)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_Keypad5, getLabel(NUM_5_OR_TRIM)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_Keypad6, getLabel(NUM_6_OR_PROGRAM)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_Keypad7, getLabel(NUM_7_OR_MIXER)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_Keypad8, getLabel(NUM_8_OR_OTHER)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_Keypad9, getLabel(NUM_9_OR_MIDI_SYNC)});

    // Pads
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_Z, getLabel(PAD_1_OR_AB)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_X, getLabel(PAD_2_OR_CD)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_C, getLabel(PAD_3_OR_EF)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_V, getLabel(PAD_4_OR_GH)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_A, getLabel(PAD_5_OR_IJ)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_S, getLabel(PAD_6_OR_KL)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_D, getLabel(PAD_7_OR_MN)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_F, getLabel(PAD_8_OR_OP)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_B, getLabel(PAD_9_OR_QR)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_N, getLabel(PAD_10_OR_ST)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_M, getLabel(PAD_11_OR_UV)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_Comma, getLabel(PAD_12_OR_WX)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_G, getLabel(PAD_13_OR_YZ)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_H,
                        getLabel(PAD_14_OR_AMPERSAND_OCTOTHORPE)});
    bindings.push_back({VmpcKeyCode::VMPC_KEY_ANSI_J,
                        getLabel(PAD_15_OR_HYPHEN_EXCLAMATION_MARK)});
    bindings.push_back(
        {VmpcKeyCode::VMPC_KEY_ANSI_K, getLabel(PAD_16_OR_PARENTHESES)});
}

const KeyboardBindingsData &KeyboardBindings::getKeyboardBindingsData() const
{
    return bindings;
}

KeyBinding *KeyboardBindings::getByIndex(const int idx)
{
    if (idx >= bindings.size())
    {
        return nullptr;
    }

    return &bindings[idx];
}

void KeyboardBindings::setBindingsData(const KeyboardBindingsData &bindingsData)
{
    bindings = bindingsData;
}

int KeyboardBindings::getBindingCount() const
{
    return bindings.size();
}

void KeyboardBindings::setBinding(const std::string &componentLabel,
                                  const Direction direction,
                                  const VmpcKeyCode keyCode)
{
    assert(componentLabelToId.count(componentLabel) > 0);

    const auto componentId = componentLabelToId.at(componentLabel);

    const auto componentBindings = lookupComponentBindings(componentLabel);

    assert((componentBindings.size() == 1 && componentId != DATA_WHEEL) ||
           (componentBindings.size() == 2 && componentId == DATA_WHEEL));

    for (auto &binding : componentBindings)
    {
        if (binding->direction == direction)
        {
            binding->keyCode = keyCode;
        }
    }
}
