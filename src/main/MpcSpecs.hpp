#pragma once

#include <cstdint>

namespace mpc
{
    struct Mpc2000XlSpecs
    {
        static constexpr uint16_t MAX_SOUND_COUNT_IN_MEMORY = 256;
        static constexpr uint16_t MAX_PROGRAM_COUNT = 24;
        static constexpr uint16_t MAX_LAST_PROGRAM_INDEX = MAX_PROGRAM_COUNT - 1;
        static constexpr uint16_t PROGRAM_BANK_COUNT = 4;
        static constexpr uint16_t HARDWARE_PAD_COUNT = 16;
        static constexpr uint16_t PADS_PER_BANK_COUNT = 16;
        static constexpr uint16_t PROGRAM_PAD_COUNT =
            PROGRAM_BANK_COUNT * PADS_PER_BANK_COUNT;
        static constexpr uint8_t FIRST_DRUM_NOTE = 35;
        static constexpr uint8_t LAST_DRUM_NOTE = 98;
        static constexpr uint8_t MUTE_ASSIGN_DISABLED = 34;
        static constexpr uint8_t OPTIONAL_NOTE_DISABLED = 34;
        static constexpr uint8_t MIDI_BUS_COUNT = 1;
        static constexpr uint8_t DRUM_BUS_COUNT = 4;
        static constexpr uint8_t TOTAL_BUS_COUNT = MIDI_BUS_COUNT + DRUM_BUS_COUNT;
        static constexpr uint8_t TRACK_COUNT = 64;
        static constexpr uint8_t LAST_TRACK_INDEX = TRACK_COUNT - 1;
        static constexpr uint8_t SEQUENCE_COUNT = 99;
        static constexpr uint16_t MAX_BAR_COUNT = 999;
        static constexpr uint16_t MAX_LAST_BAR_INDEX = MAX_BAR_COUNT - 1;
        static constexpr uint8_t LAST_SEQUENCE_INDEX = SEQUENCE_COUNT - 1;
        static constexpr uint16_t MAX_NOTE_EVENT_DURATION = 9999;

    };
} // namespace mpc
