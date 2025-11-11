#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>

#include "hardware/ComponentId.hpp"
#include "controller/ClientMidiFootswitchAssignmentController.hpp"
#include "midi/input/MidiControlTarget.hpp" // For SequencerTarget::Command

namespace mpc::controller
{

    // Forward maps (existing)
    extern const std::unordered_map<MidiFootswitchFunction,
                                    hardware::ComponentId>
        footswitchToComponentId;

    extern const std::unordered_map<
        MidiFootswitchFunction,
        midi::input::MidiControlTarget::SequencerTarget::Command>
        footswitchToSequencerCmd;

    extern const std::unordered_map<MidiFootswitchFunction, std::string>
        functionNameFromEnum;

    // === REVERSE LOOKUP HELPERS (inline, header-only) ===

    inline std::optional<MidiFootswitchFunction>
    componentIdToFootswitch(hardware::ComponentId cid)
    {
        for (const auto &[fn, id] : footswitchToComponentId)
        {
            if (id == cid)
            {
                return fn;
            }
        }
        return std::nullopt;
    }

    inline std::optional<MidiFootswitchFunction> sequencerCmdToFootswitch(
        midi::input::MidiControlTarget::SequencerTarget::Command cmd)
    {
        for (const auto &[fn, c] : footswitchToSequencerCmd)
        {
            if (c == cmd)
            {
                return fn;
            }
        }
        return std::nullopt;
    }

    inline std::optional<hardware::ComponentId>
    footswitchToComponentIdOpt(MidiFootswitchFunction fn)
    {
        auto it = footswitchToComponentId.find(fn);
        return it != footswitchToComponentId.end()
                   ? std::make_optional(it->second)
                   : std::nullopt;
    }

    inline std::optional<
        midi::input::MidiControlTarget::SequencerTarget::Command>
    footswitchToSequencerCmdOpt(MidiFootswitchFunction fn)
    {
        auto it = footswitchToSequencerCmd.find(fn);
        return it != footswitchToSequencerCmd.end()
                   ? std::make_optional(it->second)
                   : std::nullopt;
    }

    // === ORDERED LIST OF ALL FUNCTIONS (for turnWheel) ===
    inline const std::vector<MidiFootswitchFunction> &
    getAllFootswitchFunctions()
    {
        static const auto functions =
            []() -> std::vector<MidiFootswitchFunction>
        {
            std::vector<MidiFootswitchFunction> v;
            constexpr int maxVal = static_cast<int>(MidiFootswitchFunction::F6);
            for (int i = 0; i <= maxVal; ++i)
            {
                v.push_back(static_cast<MidiFootswitchFunction>(i));
            }
            return v;
        }();
        return functions;
    }

} // namespace mpc::controller
