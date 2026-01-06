#pragma once

#include "mpc_fs.hpp"

#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>

#include <vector>
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

        static std::vector<std::string> load_available_targets();

        static void resetMidiControlPreset(
            std::shared_ptr<mpc::input::midi::MidiControlPresetV3>);

        static bool doesPresetWithNameExist(const fs::path &, std::string name);
    };
} // namespace mpc::input::midi