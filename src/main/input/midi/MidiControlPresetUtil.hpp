#pragma once

#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>

#include <set>
#include <memory>

using nlohmann::json;
using nlohmann::json_schema::json_validator;

namespace mpc::input::midi
{
    struct MidiControlPresetV3;

    class MidiControlPresetUtil
    {
    public:
        static json load_schema();

        static json_validator make_validator();

        static std::set<std::string> load_available_targets();

        static void resetMidiControlPreset(
            std::shared_ptr<mpc::input::midi::MidiControlPresetV3>);
    };
} // namespace mpc::input::midi