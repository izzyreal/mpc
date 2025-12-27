#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>

#include "hardware/ComponentId.hpp"
#include "controller/ClientMidiFootswitchAssignmentController.hpp"
#include "input/midi/MidiControlTarget.hpp"

namespace mpc::controller
{
    extern const std::unordered_map<MidiFootswitchFunction,
                                    hardware::ComponentId>
        footswitchToComponentId;

    extern const std::unordered_map<
        MidiFootswitchFunction,
        input::midi::MidiControlTarget::SequencerTarget::Command>
        footswitchToSequencerCmd;

    extern const std::unordered_map<MidiFootswitchFunction, std::string>
        functionNameFromEnum;

    inline std::optional<MidiFootswitchFunction>
    componentIdToFootswitch(const hardware::ComponentId cid)
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
        const input::midi::MidiControlTarget::SequencerTarget::Command cmd)
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
    footswitchToComponentIdOpt(const MidiFootswitchFunction fn)
    {
        const auto it = footswitchToComponentId.find(fn);
        return it != footswitchToComponentId.end()
                   ? std::make_optional(it->second)
                   : std::nullopt;
    }

    inline std::optional<
        input::midi::MidiControlTarget::SequencerTarget::Command>
    footswitchToSequencerCmdOpt(const MidiFootswitchFunction fn)
    {
        const auto it = footswitchToSequencerCmd.find(fn);
        return it != footswitchToSequencerCmd.end()
                   ? std::make_optional(it->second)
                   : std::nullopt;
    }

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
