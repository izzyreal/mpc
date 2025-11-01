#pragma once
#include <string>
#include <variant>
#include "hardware/Component.hpp"

namespace mpc::midi::input {

struct MidiControlTarget {
    struct HardwareTarget {
        mpc::hardware::ComponentId componentId;
    };

    struct SequencerTarget {
        enum class Command {
            PLAY,
            REC,
            STOP,
            REC_PLUS_PLAY,
            ODUB_PLUS_PLAY,
            REC_PUNCH,
            ODUB_PUNCH
        } command;
    };

    std::string label; // human-readable
    std::variant<HardwareTarget, SequencerTarget> payload;
};

} // namespace mpc::midi::input
