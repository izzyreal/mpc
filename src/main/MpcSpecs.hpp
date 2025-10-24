#pragma once

#include <cstdint>

namespace mpc
{
    struct Mpc2000XlSpecs
    {
        static constexpr uint16_t MAX_SOUND_COUNT_IN_MEMORY = 256;
        static constexpr uint16_t MAX_PROGRAM_COUNT = 24;
        static constexpr uint16_t PROGRAM_BANK_COUNT = 4;
        static constexpr uint16_t HARDWARE_PAD_COUNT = 16;
        static constexpr uint16_t PADS_PER_BANK_COUNT = 16;
        static constexpr uint16_t PROGRAM_PAD_COUNT = PROGRAM_BANK_COUNT * PADS_PER_BANK_COUNT;
        static constexpr uint8_t FIRST_DRUM_NOTE = 35;
        static constexpr uint8_t LAST_DRUM_NOTE = 98;
        static constexpr uint8_t MUTE_ASSIGN_DISABLED = 34;
        static constexpr uint8_t OPTIONAL_NOTE_DISABLED = 34;
    };
} // namespace mpc
