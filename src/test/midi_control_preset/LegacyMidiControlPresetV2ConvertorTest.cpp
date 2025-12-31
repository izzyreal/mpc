#include "catch2/catch_test_macros.hpp"
#include "input/midi/legacy/LegacyMidiControlPresetV2Convertor.hpp"

#include "TestUtil.hpp"
#include "input/midi/legacy/LegacyMidiControlPresetPatcher.hpp"
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <cmrc/cmrc.hpp>
#include <string>
#include <iostream>

#include "iRigPadsUtil.hpp"

CMRC_DECLARE(mpctest);

using nlohmann::json;
using nlohmann::json_schema::json_validator;

TEST_CASE("Legacy iRig PADS preset V2 conversion validates against new schema",
          "[legacy-midi-control-preset-v2-conversion]")
{
    auto data = load_resource("test/LegacyMidiControlPresetV2/iRig_PADS.vmp");

    json convertedPreset =
        mpc::input::midi::legacy::parseLegacyMidiControlPresetV2(data);

    json schemaJson = json::parse(
        load_resource("test/MidiControlPreset/"
                      "vmpc2000xl_midi_control_preset.schema.v3.json"));
    mpc::input::midi::legacy::patchLegacyPreset(convertedPreset, schemaJson);

    auto validator = make_validator();

    try
    {
        (void) validator.validate(convertedPreset);
        SUCCEED("Converted preset passed schema validation.");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Schema validation failed:\n" << e.what() << "\n";
        std::cerr << "Converted JSON:\n" << convertedPreset.dump(4) << "\n";
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
    auto data = load_resource("test/LegacyMidiControlPresetV2/MPK25.vmp");

    json convertedPreset =
        mpc::input::midi::legacy::parseLegacyMidiControlPresetV2(data);

    json schemaJson = json::parse(
        load_resource("test/MidiControlPreset/"
                      "vmpc2000xl_midi_control_preset.schema.v3.json"));
    mpc::input::midi::legacy::patchLegacyPreset(convertedPreset, schemaJson);

    auto validator = make_validator();

    try
    {
        (void) validator.validate(convertedPreset);
        SUCCEED("Converted preset passed schema validation.");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Schema validation failed:\n" << e.what() << "\n";

        std::cerr << "Converted JSON:\n" << convertedPreset.dump(4) << "\n";

        FAIL("Converted preset did not pass schema validation.");
    }
}
