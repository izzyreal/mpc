#pragma once

#include "hardware/Component.hpp"

#include <variant>

namespace mpc::input::midi
{
    struct MidiControlTarget
    {
        struct HardwareTarget
        {
            hardware::ComponentId componentId;
        };

        struct SequencerTarget
        {
            enum class Command
            {
                PLAY,
                REC,
                STOP,
                REC_PLUS_PLAY,
                ODUB_PLUS_PLAY,
                REC_PUNCH,
                ODUB_PUNCH
            } command;
        };

        std::variant<HardwareTarget, SequencerTarget> payload;
    };
} // namespace mpc::input::midi
