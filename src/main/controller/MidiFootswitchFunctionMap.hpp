#pragma once
#include <string>
#include <unordered_map>
#include "hardware/ComponentId.hpp"
#include "controller/ClientMidiFootswitchAssignmentController.hpp"

namespace mpc::controller
{

    enum class MidiFootswitchFunction;

    extern const std::unordered_map<MidiFootswitchFunction,
                                    mpc::hardware::ComponentId>
        footswitchToComponentId;
    extern const std::unordered_map<
        MidiFootswitchFunction,
        mpc::midi::input::MidiControlTarget::SequencerTarget::Command>
        footswitchToSequencerCmd;
    extern const std::unordered_map<MidiFootswitchFunction, std::string>
        functionNameFromEnum;

} // namespace mpc::controller
