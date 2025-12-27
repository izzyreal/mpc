#pragma once
#include <string>
#include <variant>
#include "hardware/Component.hpp"

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

        std::string label;
        std::variant<HardwareTarget, SequencerTarget> payload;
    };
} // namespace mpc::input::midi
