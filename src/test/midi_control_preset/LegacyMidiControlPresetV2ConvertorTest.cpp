#include "catch2/catch_test_macros.hpp"
#include "controls/midi/legacy/LegacyMidiControlPresetV2Convertor.h"
#include "controls/midi/legacy/LegacyMidiControlPresetPatcher.h"
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <cmrc/cmrc.hpp>
#include <string>
#include <iostream>

#include "iRigPadsUtil.h"

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
    json schemaJson = json::parse(
        load_resource("test/MidiControlPreset/"
                      "vmpc2000xl_midi_control_preset.schema.v3.json"));

    // Default constructor is fine if you don't need remote $ref resolution
    json_validator validator;
    validator.set_root_schema(schemaJson);

    return validator;
}

TEST_CASE("Legacy iRig PADS preset V2 conversion validates against new schema",
          "[legacy-midi-control-preset-v2-conversion]")
{
    // Load legacy binary preset
    auto data = load_resource("test/LegacyMidiControlPresetV2/iRig_PADS.vmp");

    // Convert to JSON using the parser
    json convertedPreset =
        mpc::controls::midi::legacy::parseLegacyMidiControlPresetV2(data);

    json schemaJson = json::parse(
        load_resource("test/MidiControlPreset/"
                      "vmpc2000xl_midi_control_preset.schema.v3.json"));
    mpc::controls::midi::legacy::patchLegacyPreset(convertedPreset, schemaJson);

    // Create validator from schema
    auto validator = make_validator();

    // Validate and report detailed errors
    try
    {
        validator.validate(convertedPreset);
        // If we reach here, validation succeeded
        SUCCEED("Converted preset passed schema validation.");
    }
    catch (const std::exception &e)
    {
        // Print full validation error details
        std::cerr << "Schema validation failed:\n" << e.what() << "\n";

        std::cerr << "Converted JSON:\n" << convertedPreset.dump(4) << "\n";

        // Fail the test explicitly
        FAIL("Converted preset did not pass schema validation.");
    }

    try
    {
        checkIRigPadsPreset(convertedPreset);

        SUCCEED("Converted preset matches iRig PADS spec.");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Consistency check failed:\n" << e.what() << "\n";
        FAIL("Converted preset did not pass consistency check.");
    }
}

TEST_CASE("Legacy MPK25 preset V2 conversion validates against new schema",
          "[mpk]")
{
    // Load legacy binary preset
    auto data = load_resource("test/LegacyMidiControlPresetV2/MPK25.vmp");

    // Convert to JSON using the parser
    json convertedPreset =
        mpc::controls::midi::legacy::parseLegacyMidiControlPresetV2(data);

    json schemaJson = json::parse(
        load_resource("test/MidiControlPreset/"
                      "vmpc2000xl_midi_control_preset.schema.v3.json"));
    mpc::controls::midi::legacy::patchLegacyPreset(convertedPreset, schemaJson);

    // Create validator from schema
    auto validator = make_validator();

    // Validate and report detailed errors
    try
    {
        validator.validate(convertedPreset);
        // If we reach here, validation succeeded
        SUCCEED("Converted preset passed schema validation.");
    }
    catch (const std::exception &e)
    {
        // Print full validation error details
        std::cerr << "Schema validation failed:\n" << e.what() << "\n";

        std::cerr << "Converted JSON:\n" << convertedPreset.dump(4) << "\n";

        // Fail the test explicitly
        FAIL("Converted preset did not pass schema validation.");
    }
    /*
        try {
            checkIRigPadsPreset(convertedPreset);

            SUCCEED("Converted preset matches iRig PADS spec.");
        } catch (const std::exception &e) {
            std::cerr << "Consistency check failed:\n" << e.what() << "\n";
            FAIL("Converted preset did not pass consistency check.");
        }*/
}
