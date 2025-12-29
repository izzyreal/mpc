#pragma once

#include <unordered_map>
#include <string>

namespace mpc::hardware
{
    enum ComponentId
    {
        NONE = 0,
        PAD_1_OR_AB,
        PAD_2_OR_CD,
        PAD_3_OR_EF,
        PAD_4_OR_GH,
        PAD_5_OR_IJ,
        PAD_6_OR_KL,
        PAD_7_OR_MN,
        PAD_8_OR_OP,
        PAD_9_OR_QR,
        PAD_10_OR_ST,
        PAD_11_OR_UV,
        PAD_12_OR_WX,
        PAD_13_OR_YZ,
        PAD_14_OR_AMPERSAND_OCTOTHORPE,
        PAD_15_OR_HYPHEN_EXCLAMATION_MARK,
        PAD_16_OR_PARENTHESES,

        CURSOR_LEFT_OR_DIGIT,
        CURSOR_RIGHT_OR_DIGIT,
        CURSOR_UP,
        CURSOR_DOWN,
        REC,
        OVERDUB,
        STOP,
        PLAY,
        PLAY_START,
        MAIN_SCREEN,
        PREV_STEP_OR_EVENT,
        NEXT_STEP_OR_EVENT,
        GO_TO,
        PREV_BAR_OR_START,
        NEXT_BAR_OR_END,
        TAP_TEMPO_OR_NOTE_REPEAT,
        NEXT_SEQ,
        TRACK_MUTE,
        OPEN_WINDOW,
        FULL_LEVEL_OR_CASE_SWITCH,
        SIXTEEN_LEVELS_OR_SPACE,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        SHIFT,
        ENTER_OR_SAVE,
        UNDO_SEQ,
        ERASE,
        AFTER_OR_ASSIGN,
        BANK_A,
        BANK_B,
        BANK_C,
        BANK_D,
        NUM_0_OR_VMPC,
        NUM_1_OR_SONG,
        NUM_2_OR_MISC,
        NUM_3_OR_LOAD,
        NUM_4_OR_SAMPLE,
        NUM_5_OR_TRIM,
        NUM_6_OR_PROGRAM,
        NUM_7_OR_MIXER,
        NUM_8_OR_OTHER,
        NUM_9_OR_MIDI_SYNC,

        DATA_WHEEL,
        SLIDER,
        REC_GAIN_POT,
        MAIN_VOLUME_POT,

        FULL_LEVEL_OR_CASE_SWITCH_LED,
        SIXTEEN_LEVELS_OR_SPACE_LED,
        NEXT_SEQ_LED,
        TRACK_MUTE_LED,
        BANK_A_LED,
        BANK_B_LED,
        BANK_C_LED,
        BANK_D_LED,
        AFTER_OR_ASSIGN_LED,
        UNDO_SEQ_LED,
        REC_LED,
        OVERDUB_LED,
        PLAY_LED,

        COMPONENT_ID_COUNT
    };

    static bool isPadId(const ComponentId id)
    {
        return id >= PAD_1_OR_AB && id <= PAD_16_OR_PARENTHESES;
    }

    static bool isButtonId(const ComponentId id)
    {
        return id >= CURSOR_LEFT_OR_DIGIT && id <= NUM_9_OR_MIDI_SYNC;
    }

    static const std::unordered_map<ComponentId, std::string>
        componentIdToLabel{
            {PAD_1_OR_AB, "pad-1-or-ab"},
            {PAD_2_OR_CD, "pad-2-or-cd"},
            {PAD_3_OR_EF, "pad-3-or-ef"},
            {PAD_4_OR_GH, "pad-4-or-gh"},
            {PAD_5_OR_IJ, "pad-5-or-ij"},
            {PAD_6_OR_KL, "pad-6-or-kl"},
            {PAD_7_OR_MN, "pad-7-or-mn"},
            {PAD_8_OR_OP, "pad-8-or-op"},
            {PAD_9_OR_QR, "pad-9-or-qr"},
            {PAD_10_OR_ST, "pad-10-or-st"},
            {PAD_11_OR_UV, "pad-11-or-uv"},
            {PAD_12_OR_WX, "pad-12-or-wx"},
            {PAD_13_OR_YZ, "pad-13-or-yz"},
            {PAD_14_OR_AMPERSAND_OCTOTHORPE, "pad-14-or-ampersand-octothorpe"},
            {PAD_15_OR_HYPHEN_EXCLAMATION_MARK,
             "pad-15-or-hyphen-exclamation-mark"},
            {PAD_16_OR_PARENTHESES, "pad-16-or-parentheses"},

            {CURSOR_LEFT_OR_DIGIT, "cursor-left-or-digit"},
            {CURSOR_RIGHT_OR_DIGIT, "cursor-right-or-digit"},
            {CURSOR_UP, "cursor-up"},
            {CURSOR_DOWN, "cursor-down"},
            {REC, "rec"},
            {OVERDUB, "overdub"},
            {STOP, "stop"},
            {PLAY, "play"},
            {PLAY_START, "play-start"},
            {MAIN_SCREEN, "main-screen"},
            {PREV_STEP_OR_EVENT, "prev-step-or-event"},
            {NEXT_STEP_OR_EVENT, "next-step-or-event"},
            {GO_TO, "go-to"},
            {PREV_BAR_OR_START, "prev-bar-or-start"},
            {NEXT_BAR_OR_END, "next-bar-or-end"},
            {TAP_TEMPO_OR_NOTE_REPEAT, "tap-or-note-repeat"},
            {NEXT_SEQ, "next-seq"},
            {TRACK_MUTE, "track-mute"},
            {OPEN_WINDOW, "open-window"},
            {FULL_LEVEL_OR_CASE_SWITCH, "full-level-or-case-switch"},
            {SIXTEEN_LEVELS_OR_SPACE, "sixteen-levels-or-space"},
            {F1, "f1"},
            {F2, "f2"},
            {F3, "f3"},
            {F4, "f4"},
            {F5, "f5"},
            {F6, "f6"},
            {SHIFT, "shift"},
            {ENTER_OR_SAVE, "enter-or-save"},
            {UNDO_SEQ, "undo-seq"},
            {ERASE, "erase"},
            {AFTER_OR_ASSIGN, "after-or-assign"},
            {BANK_A, "bank-a"},
            {BANK_B, "bank-b"},
            {BANK_C, "bank-c"},
            {BANK_D, "bank-d"},
            {NUM_0_OR_VMPC, "0-or-vmpc"},
            {NUM_1_OR_SONG, "1-or-song"},
            {NUM_2_OR_MISC, "2-or-misc"},
            {NUM_3_OR_LOAD, "3-or-load"},
            {NUM_4_OR_SAMPLE, "4-or-sample"},
            {NUM_5_OR_TRIM, "5-or-trim"},
            {NUM_6_OR_PROGRAM, "6-or-program"},
            {NUM_7_OR_MIXER, "7-or-mixer"},
            {NUM_8_OR_OTHER, "8-or-other"},
            {NUM_9_OR_MIDI_SYNC, "9-or-midi-sync"},

            {DATA_WHEEL, "data-wheel"},
            {SLIDER, "slider"},
            {REC_GAIN_POT, "rec-gain-pot"},
            {MAIN_VOLUME_POT, "main-volume-pot"},

            {FULL_LEVEL_OR_CASE_SWITCH_LED, "full-level-or-case-switch-led"},
            {SIXTEEN_LEVELS_OR_SPACE_LED, "sixteen-levels-or-space-led"},
            {NEXT_SEQ_LED, "next-seq-led"},
            {TRACK_MUTE_LED, "track-mute-led"},
            {BANK_A_LED, "bank-a-led"},
            {BANK_B_LED, "bank-b-led"},
            {BANK_C_LED, "bank-c-led"},
            {BANK_D_LED, "bank-d-led"},
            {AFTER_OR_ASSIGN_LED, "after-or-assign-led"},
            {UNDO_SEQ_LED, "undo-seq-led"},
            {REC_LED, "rec-led"},
            {OVERDUB_LED, "overdub-led"},
            {PLAY_LED, "play-led"}};

    static const std::unordered_map<std::string, ComponentId>
        componentLabelToId = []
    {
        std::unordered_map<std::string, ComponentId> map;
        for (const auto &[key, value] : componentIdToLabel)
        {
            map[value] = key;
        }
        return map;
    }();
} // namespace mpc::hardware
