#pragma once

#include "MpcResourceUtil.hpp"

#include <nlohmann/json.hpp>

#include <set>

#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>

using nlohmann::json;
using nlohmann::json_schema::json_validator;

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

inline std::set<std::string> load_available_targets()
{
    auto schema = load_schema();
    if (!schema.contains("$defs") || !schema["$defs"].contains("binding") ||
        !schema["$defs"]["binding"].contains("properties") ||
        !schema["$defs"]["binding"]["properties"].contains("target") ||
        !schema["$defs"]["binding"]["properties"]["target"].contains("enum"))
    {
        throw std::runtime_error(
            "Schema missing target enum at expected path");
    }

    const auto &enumArray =
        schema["$defs"]["binding"]["properties"]["target"]["enum"];
    std::set<std::string> targets;
    for (const auto &v : enumArray)
    {
        targets.insert(v.get<std::string>());
    }

    return targets;
}
