#include "catch2/catch_test_macros.hpp"
#include "input/midi/legacy/LegacyMidiControlPresetV1Convertor.hpp"

#include "TestUtil.hpp"
#include <nlohmann/json.hpp>

#include <string>
#include <iostream>
#include <set>

#include "iRigPadsUtil.hpp"

using nlohmann::json;

TEST_CASE("Legacy preset V1 conversion iRig PADS",
          "[legacy-midi-control-preset-v1-convertor]")
{
    auto data = load_resource("test/LegacyMidiControlPresetV1/iRig_PADS.vmp");

    json convertedPreset =
        mpc::input::midi::legacy::parseLegacyMidiControlPresetV1(data);

    try
    {
        make_validator().validate(convertedPreset);
        SUCCEED("Converted preset passed schema validation.");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Schema validation failed:\n" << e.what() << "\n";
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
                 std::cerr << "Converted JSON:\n" << convertedPreset.dump(4) << "\n";
        FAIL("Converted preset did not pass consistency check.");
    }
}

TEST_CASE("Legacy preset V1 parses all ' (extra)' labels correctly",
          "[legacy-midi-control-preset-v1-convertor]")
{
    const auto data = load_resource(
        "test/LegacyMidiControlPresetV1/erroneous_extra_first_run.vmp");

    json convertedPreset =
        mpc::input::midi::legacy::parseLegacyMidiControlPresetV1(data);

    try
    {
        make_validator().validate(convertedPreset);
        SUCCEED("Converted preset passed schema validation.");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Schema validation failed:\n" << e.what() << "\n";
        FAIL("Converted preset did not pass schema validation.");
    }
}

TEST_CASE(
    "Legacy preset V1 with erroneously parsed and persisted ' (extra)' labels ",
    "[legacy-midi-control-preset-v1-convertor]")
{
    auto data = load_resource(
        "test/LegacyMidiControlPresetV1/erroneous_extra_second_run.vmp");

    json convertedPreset =
        mpc::input::midi::legacy::parseLegacyMidiControlPresetV1(data);

    try
    {
        make_validator().validate(convertedPreset);
        SUCCEED("Converted preset passed schema validation.");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Schema validation failed:\n" << e.what() << "\n";
        FAIL("Converted preset did not pass schema validation.");
    }
}
