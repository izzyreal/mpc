#pragma once
#include <optional>
#include <variant>
#include "input/midi/MidiControlTarget.hpp"

namespace mpc::input::midi
{

    enum class Interaction
    {
        Press,
        Release,
        Toggle,
        Momentary
    };

    struct MidiBindingBase
    {
        int channel{-1};          // 0–15 or -1 for OMNI
        int number{0};            // note or CC number
        std::optional<int> value; // optional threshold / velocity
        Interaction interaction{Interaction::Press};
        enum class MessageType
        {
            Note,
            CC
        } messageType{MessageType::CC};
    };

    struct HardwareBinding : MidiBindingBase
    {
        MidiControlTarget::HardwareTarget target;
    };

    struct SequencerBinding : MidiBindingBase
    {
        MidiControlTarget::SequencerTarget target;
    };

    using MidiControlTargetBinding =
        std::variant<HardwareBinding, SequencerBinding>;

} // namespace mpc::midi::input
