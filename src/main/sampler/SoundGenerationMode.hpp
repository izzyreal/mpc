#pragma once

#include <cstdint>

namespace mpc::sampler
{
    enum class SoundGenerationMode : int8_t
    {
        Normal = 0,
        Simult = 1,
        VelocitySwitch = 2,
        DecaySwitch = 3,
    };

    constexpr int8_t toRaw(const SoundGenerationMode mode)
    {
        return static_cast<int8_t>(mode);
    }

    constexpr SoundGenerationMode soundGenerationModeFromRaw(const int rawValue)
    {
        switch (rawValue)
        {
            case 1:
                return SoundGenerationMode::Simult;
            case 2:
                return SoundGenerationMode::VelocitySwitch;
            case 3:
                return SoundGenerationMode::DecaySwitch;
            default:
                return SoundGenerationMode::Normal;
        }
    }
}
