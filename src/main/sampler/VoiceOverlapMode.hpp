#pragma once

#include <cstdint>

namespace mpc::sampler
{
    enum class VoiceOverlapMode : uint8_t
    {
        POLY = 0,
        MONO = 1,
        NOTE_OFF = 2
    };
} // namespace mpc::sampler
