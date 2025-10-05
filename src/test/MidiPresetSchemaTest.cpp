#include "catch2/catch_test_macros.hpp"
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <cmrc/cmrc.hpp>
#include <string>

CMRC_DECLARE(mpctest);

using nlohmann::json;
using nlohmann::json_schema::json_validator;

// Utility to load a file from the embedded test resources
inline std::string load_resource(const std::string &path)
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open(path);
    return std::string(file.begin(), file.end());
}

// Helper to create a validator from the schema resource
inline json_validator make_validator()
{
    json schemaJson = json::parse(load_resource("test/MidiPresetJson/vmpc2000xl_midi_preset.schema.v1.json"));

    // Default constructor is fine if you don't need remote $ref resolution
    json_validator validator;
    validator.set_root_schema(schemaJson);

    return validator;
}

TEST_CASE("Test schema is parsable", "[midi-preset-schema]")
{
    auto schemaStr = load_resource("test/MidiPresetJson/vmpc2000xl_midi_preset.schema.v1.json");
    REQUIRE_NOTHROW(json::parse(schemaStr));
}

TEST_CASE("Valid MIDI preset passes validation", "[midi-preset-schema]")
{
    auto validator = make_validator();
    auto presetStr = load_resource("test/MidiPresetJson/midi-preset-valid.json");
    json presetJson = json::parse(presetStr);

    REQUIRE_NOTHROW(validator.validate(presetJson));
}

TEST_CASE("Invalid MIDI preset fails validation", "[midi-preset-schema]")
{
    auto validator = make_validator();
    auto presetStr = load_resource("test/MidiPresetJson/midi-preset-invalid.json");
    json presetJson = json::parse(presetStr);

    REQUIRE_THROWS_AS(validator.validate(presetJson), std::exception);
}

TEST_CASE("Preset name length constraint", "[midi-preset-schema]")
{
    auto validator = make_validator();
    json presetJson = json::parse(load_resource("test/MidiPresetJson/midi-preset-valid.json"));

    // Make the name too long
    presetJson["name"] = "ThisNameIsWayTooLong";

    REQUIRE_THROWS_AS(validator.validate(presetJson), std::exception);

    // Fix to valid length
    presetJson["name"] = "ShortName";

    REQUIRE_NOTHROW(validator.validate(presetJson));
}

