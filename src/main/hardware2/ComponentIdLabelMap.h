#pragma once

#include <unordered_map>
#include <string>

namespace mpc::hardware2 {

    enum ComponentId {
        NONE,
        PAD1, PAD2, PAD3, PAD4,
        PAD5, PAD6, PAD7, PAD8,
        PAD9, PAD10, PAD11, PAD12,
        PAD13, PAD14, PAD15, PAD16,

        CURSOR_LEFT,
        CURSOR_RIGHT,
        CURSOR_UP,
        CURSOR_DOWN,
        REC,
        OVERDUB,
        STOP,
        PLAY,
        PLAY_START,
        MAIN_SCREEN,
        PREV_STEP_EVENT,
        NEXT_STEP_EVENT,
        GO_TO,
        PREV_BAR_START,
        NEXT_BAR_END,
        TAP,
        NEXT_SEQ,
        TRACK_MUTE,
        OPEN_WINDOW,
        FULL_LEVEL,
        SIXTEEN_LEVELS,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        SHIFT,
        ENTER,
        UNDO_SEQ,
        ERASE,
        AFTER,
        BANK_A,
        BANK_B,
        BANK_C,
        BANK_D,
        NUM_0,
        NUM_1,
        NUM_2,
        NUM_3,
        NUM_4,
        NUM_5,
        NUM_6,
        NUM_7,
        NUM_8,
        NUM_9,

        DATA_WHEEL,
        SLIDER,
        REC_GAIN_POT,
        MAIN_VOLUME_POT
    };

    static const std::unordered_map<ComponentId, std::string> componentIdToLabel {
        { ComponentId::PAD1, "pad-1" },   { ComponentId::PAD2, "pad-2" },
        { ComponentId::PAD3, "pad-3" },   { ComponentId::PAD4, "pad-4" },
        { ComponentId::PAD5, "pad-5" },   { ComponentId::PAD6, "pad-6" },
        { ComponentId::PAD7, "pad-7" },   { ComponentId::PAD8, "pad-8" },
        { ComponentId::PAD9, "pad-9" },   { ComponentId::PAD10, "pad-10" },
        { ComponentId::PAD11, "pad-11" }, { ComponentId::PAD12, "pad-12" },
        { ComponentId::PAD13, "pad-13" }, { ComponentId::PAD14, "pad-14" },
        { ComponentId::PAD15, "pad-15" }, { ComponentId::PAD16, "pad-16" },

        { ComponentId::CURSOR_LEFT,     "left" },
        { ComponentId::CURSOR_RIGHT,    "right" },
        { ComponentId::CURSOR_UP,       "up" },
        { ComponentId::CURSOR_DOWN,     "down" },
        { ComponentId::REC,             "rec" },
        { ComponentId::OVERDUB,         "overdub" },
        { ComponentId::STOP,            "stop" },
        { ComponentId::PLAY,            "play" },
        { ComponentId::PLAY_START,      "play-start" },
        { ComponentId::MAIN_SCREEN,     "main-screen" },
        { ComponentId::PREV_STEP_EVENT, "prev-step-event" },
        { ComponentId::NEXT_STEP_EVENT, "next-step-event" },
        { ComponentId::GO_TO,           "go-to" },
        { ComponentId::PREV_BAR_START,  "prev-bar-start" },
        { ComponentId::NEXT_BAR_END,    "next-bar-end" },
        { ComponentId::TAP,             "tap" },
        { ComponentId::NEXT_SEQ,        "next-seq" },
        { ComponentId::TRACK_MUTE,      "track-mute" },
        { ComponentId::OPEN_WINDOW,     "open-window" },
        { ComponentId::FULL_LEVEL,      "full-level" },
        { ComponentId::SIXTEEN_LEVELS,  "sixteen-levels" },
        { ComponentId::F1,              "f1" },
        { ComponentId::F2,              "f2" },
        { ComponentId::F3,              "f3" },
        { ComponentId::F4,              "f4" },
        { ComponentId::F5,              "f5" },
        { ComponentId::F6,              "f6" },
        { ComponentId::SHIFT,           "shift" },
        { ComponentId::ENTER,           "enter" },
        { ComponentId::UNDO_SEQ,        "undo-seq" },
        { ComponentId::ERASE,           "erase" },
        { ComponentId::AFTER,           "after" },
        { ComponentId::BANK_A,          "bank-a" },
        { ComponentId::BANK_B,          "bank-b" },
        { ComponentId::BANK_C,          "bank-c" },
        { ComponentId::BANK_D,          "bank-d" },
        { ComponentId::NUM_0,           "0" },
        { ComponentId::NUM_1,           "1" },
        { ComponentId::NUM_2,           "2" },
        { ComponentId::NUM_3,           "3" },
        { ComponentId::NUM_4,           "4" },
        { ComponentId::NUM_5,           "5" },
        { ComponentId::NUM_6,           "6" },
        { ComponentId::NUM_7,           "7" },
        { ComponentId::NUM_8,           "8" },
        { ComponentId::NUM_9,           "9" },

        { ComponentId::DATA_WHEEL,      "datawheel" },
        { ComponentId::SLIDER,          "slider" },
        { ComponentId::REC_GAIN_POT,    "rec-gain" },
        { ComponentId::MAIN_VOLUME_POT, "main-volume" }
    };

    static const std::unordered_map<std::string, ComponentId> componentLabelToId = [] {
        std::unordered_map<std::string, ComponentId> map;
        for (const auto& [key, value] : componentIdToLabel)
            map[value] = key;
        return map;
    }();
}

