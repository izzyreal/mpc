#pragma once

#include "MpcResourceUtil.hpp"

#include <string>

#include <cmrc/cmrc.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>

CMRC_DECLARE(mpctest);

using nlohmann::json;
using nlohmann::json_schema::json_validator;

inline std::string load_resource(const std::string &path)
{
    const auto fs = cmrc::mpctest::get_filesystem();
    const auto file = fs.open(path);
    return std::string(file.begin(), file.end());
}

inline json load_schema()
{
    auto schemaData = mpc::MpcResourceUtil::get_resource_data(
        "midicontrolpresets/vmpc2000xl_midi_control_preset.schema.v3.json");
    return json::parse(schemaData);
}

inline json_validator make_validator()
{
    const json schemaJson = load_schema();
    json_validator validator;
    validator.set_root_schema(schemaJson);
    return validator;
}
