#include "catch2/catch_test_macros.hpp"
#include "input/midi/legacy/LegacyMidiControlPresetV2Convertor.hpp"
#include "input/midi/legacy/LegacyMidiControlPresetPatcher.hpp"
#include "input/midi/MidiControlPresetUtil.hpp"

#include "TestUtil.hpp"
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <cmrc/cmrc.hpp>
#include <string>
#include <iostream>

#include "iRigPadsUtil.hpp"

CMRC_DECLARE(mpctest);

using nlohmann::json;
using nlohmann::json_schema::json_validator;
using namespace mpc::input::midi;

TEST_CASE("Legacy iRig PADS preset V2 conversion validates against new schema",
          "[legacy-midi-control-preset-v2-convertor]")
{
    auto data = load_resource("test/LegacyMidiControlPresetV2/iRig_PADS.vmp");

    json convertedPreset =
        legacy::parseLegacyMidiControlPresetV2(data);

    legacy::patchLegacyPreset(convertedPreset, MidiControlPresetUtil::load_schema());

    try
    {
        (void) MidiControlPresetUtil::make_validator().validate(convertedPreset);
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
          "[legacy-midi-control-preset-v2-convertor]")
{
    auto data = load_resource("test/LegacyMidiControlPresetV2/MPK25.vmp");

    json convertedPreset =
        legacy::parseLegacyMidiControlPresetV2(data);

    legacy::patchLegacyPreset(convertedPreset, MidiControlPresetUtil::load_schema());

    try
    {
        (void) MidiControlPresetUtil::make_validator().validate(convertedPreset);
        SUCCEED("Converted preset passed schema validation.");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Schema validation failed:\n" << e.what() << "\n";

        std::cerr << "Converted JSON:\n" << convertedPreset.dump(4) << "\n";

        FAIL("Converted preset did not pass schema validation.");
    }
}
