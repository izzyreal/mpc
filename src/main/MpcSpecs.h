#pragma once

#include <cstdint>

namespace mpc {
    const uint16_t MAX_SOUND_COUNT_IN_MEMORY = 256;
    const uint16_t MAX_PROGRAM_COUNT = 24;
    const uint16_t PROGRAM_BANK_COUNT = 4;
    const uint16_t HARDWARE_PAD_COUNT = 16;
    const uint16_t PADS_PER_BANK_COUNT = 16;
    const uint16_t PROGRAM_PAD_COUNT = PROGRAM_BANK_COUNT * PADS_PER_BANK_COUNT;
}
