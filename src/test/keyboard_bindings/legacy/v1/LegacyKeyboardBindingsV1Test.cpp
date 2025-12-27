#include "catch2/catch_test_macros.hpp"
#include "input/keyboard/legacy/LegacyKeyboardBindingsConvertor.hpp"

#include <nlohmann/json.hpp>

#include <cmrc/cmrc.hpp>

#include <string>
#include <iostream>

CMRC_DECLARE(mpctest);

using nlohmann::json;

inline std::string load_resource(const std::string &path)
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open(path);
    return std::string(file.begin(), file.end());
}

TEST_CASE("Legacy preset V1 conversion works",
          "[legacy-keyboard-binding-v1-conversion]")
{
    auto data = load_resource("test/LegacyKeyboardBindingsV1/keys.txt");

    auto bindingsData = mpc::input::keyboard::legacy::
        LegacyKeyboardBindingsConvertor::parseLegacyKeyboardBindings(data);

    REQUIRE(bindingsData.size() == 76);

    using VmpcKeyCode = mpc::input::keyboard::VmpcKeyCode;
    using Id = mpc::hardware::ComponentId;

    mpc::input::keyboard::KeyboardBindings bindings(bindingsData);

    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_LeftArrow)
                ->getComponentId() == Id::CURSOR_LEFT_OR_DIGIT);
    REQUIRE(
        bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_RightArrow)
            ->getComponentId() == Id::CURSOR_RIGHT_OR_DIGIT);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_UpArrow)
                ->getComponentId() == Id::CURSOR_UP);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_DownArrow)
                ->getComponentId() == Id::CURSOR_DOWN);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_L)
                ->getComponentId() == Id::REC);
    REQUIRE(bindings
                .lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_Semicolon)
                ->getComponentId() == Id::OVERDUB);
    REQUIRE(
        bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_Quote)
            ->getComponentId() == Id::STOP);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_Space)
                ->getComponentId() == Id::PLAY);
    REQUIRE(bindings
                .lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_Backslash)
                ->getComponentId() == Id::PLAY_START);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_Escape)
                ->getComponentId() == Id::MAIN_SCREEN);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_I)
                ->getComponentId() == Id::OPEN_WINDOW);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_Q)
                ->getComponentId() == Id::PREV_STEP_OR_EVENT);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_W)
                ->getComponentId() == Id::NEXT_STEP_OR_EVENT);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_E)
                ->getComponentId() == Id::GO_TO);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_R)
                ->getComponentId() == Id::PREV_BAR_OR_START);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_T)
                ->getComponentId() == Id::NEXT_BAR_OR_END);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_Y)
                ->getComponentId() == Id::TAP_TEMPO_OR_NOTE_REPEAT);
    REQUIRE(
        bindings
            .lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_LeftBracket)
            ->getComponentId() == Id::NEXT_SEQ);
    REQUIRE(
        bindings
            .lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_RightBracket)
            ->getComponentId() == Id::TRACK_MUTE);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_O)
                ->getComponentId() == Id::FULL_LEVEL_OR_CASE_SWITCH);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_P)
                ->getComponentId() == Id::SIXTEEN_LEVELS_OR_SPACE);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_F1)
                ->getComponentId() == Id::F1);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_F2)
                ->getComponentId() == Id::F2);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_F3)
                ->getComponentId() == Id::F3);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_F4)
                ->getComponentId() == Id::F4);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_F5)
                ->getComponentId() == Id::F5);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_F6)
                ->getComponentId() == Id::F6);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_Shift)
                ->getComponentId() == Id::SHIFT);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_LeftShift)
                ->getComponentId() == Id::SHIFT);
    REQUIRE(
        bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_RightShift)
            ->getComponentId() == Id::SHIFT);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_Return)
                ->getComponentId() == Id::ENTER_OR_SAVE);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_F10)
                ->getComponentId() == Id::UNDO_SEQ);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_F8)
                ->getComponentId() == Id::ERASE);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_F9)
                ->getComponentId() == Id::AFTER_OR_ASSIGN);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_Home)
                ->getComponentId() == Id::BANK_A);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_End)
                ->getComponentId() == Id::BANK_B);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_Insert)
                ->getComponentId() == Id::BANK_C);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_Delete)
                ->getComponentId() == Id::BANK_D);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_0)
                ->getComponentId() == Id::NUM_0_OR_VMPC);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_1)
                ->getComponentId() == Id::NUM_1_OR_SONG);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_2)
                ->getComponentId() == Id::NUM_2_OR_MISC);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_3)
                ->getComponentId() == Id::NUM_3_OR_LOAD);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_4)
                ->getComponentId() == Id::NUM_4_OR_SAMPLE);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_5)
                ->getComponentId() == Id::NUM_5_OR_TRIM);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_6)
                ->getComponentId() == Id::NUM_6_OR_PROGRAM);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_7)
                ->getComponentId() == Id::NUM_7_OR_MIXER);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_8)
                ->getComponentId() == Id::NUM_8_OR_OTHER);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_9)
                ->getComponentId() == Id::NUM_9_OR_MIDI_SYNC);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_Z)
                ->getComponentId() == Id::PAD_1_OR_AB);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_X)
                ->getComponentId() == Id::PAD_2_OR_CD);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_C)
                ->getComponentId() == Id::PAD_3_OR_EF);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_V)
                ->getComponentId() == Id::PAD_4_OR_GH);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_A)
                ->getComponentId() == Id::PAD_5_OR_IJ);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_S)
                ->getComponentId() == Id::PAD_6_OR_KL);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_D)
                ->getComponentId() == Id::PAD_7_OR_MN);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_F)
                ->getComponentId() == Id::PAD_8_OR_OP);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_B)
                ->getComponentId() == Id::PAD_9_OR_QR);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_N)
                ->getComponentId() == Id::PAD_10_OR_ST);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_M)
                ->getComponentId() == Id::PAD_11_OR_UV);
    REQUIRE(
        bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_Comma)
            ->getComponentId() == Id::PAD_12_OR_WX);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_G)
                ->getComponentId() == Id::PAD_13_OR_YZ);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_H)
                ->getComponentId() == Id::PAD_14_OR_AMPERSAND_OCTOTHORPE);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_J)
                ->getComponentId() == Id::PAD_15_OR_HYPHEN_EXCLAMATION_MARK);
    REQUIRE(bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_K)
                ->getComponentId() == Id::PAD_16_OR_PARENTHESES);
    REQUIRE(
        bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_Minus)
            ->getComponentId() == Id::DATA_WHEEL);
    REQUIRE(
        bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_Minus)
            ->direction == mpc::input::Direction::Negative);

    REQUIRE(
        bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_Equal)
            ->getComponentId() == Id::DATA_WHEEL);
    REQUIRE(
        bindings.lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_ANSI_Equal)
            ->direction == mpc::input::Direction::Positive);

    REQUIRE(bindings.hasNoDuplicates());
}
