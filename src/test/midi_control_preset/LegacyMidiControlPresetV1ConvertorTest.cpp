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

    auto validator = make_validator();

    try
    {
        (void) validator.validate(convertedPreset);
        SUCCEED("Converted preset passed schema validation.");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Schema validation failed:\n" << e.what() << "\n";

//         std::cerr << "Converted JSON:\n" << convertedPreset.dump(4) << "\n";

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
          "[legacy-midi-control-preset-v1-conversion]")
{
    const auto data = load_resource(
        "test/LegacyMidiControlPresetV1/erroneous_extra_first_run.vmp");

    json convertedPreset =
        mpc::input::midi::legacy::parseLegacyMidiControlPresetV1(data);

    const std::vector<std::string> expectedTargets = {
        "hardware:0-or-vmpc",     "hardware:1-or-song",   "hardware:2-or-misc",
        "hardware:3-or-load",     "hardware:4-or-sample", "hardware:5-or-trim",
        "hardware:6-or-program",  "hardware:7-or-mixer",  "hardware:8-or-other",
        "hardware:9-or-midi-sync"};

    std::vector<std::string> actualTargets;

    for (auto &convertedPresetBinding : convertedPreset["bindings"])
    {
        if (convertedPresetBinding.contains("target") &&
            convertedPresetBinding["target"].is_string())
        {
            auto target =
                convertedPresetBinding["target"].get<std::string>();
            actualTargets.push_back(target);
        }
    }

    for (const auto &expected : expectedTargets)
    {
        int labelCount = 0;
        for (const auto &actualLabel : actualTargets)
        {
            if (actualLabel == expected) labelCount++;
        }

        REQUIRE(labelCount == 2);
    }
}

TEST_CASE(
    "Legacy preset V1 with erroneously parsed and persisted ' (extra)' labels ",
    "[legacy-midi-control-preset-v1-conversion]")
{
    auto data = load_resource(
        "test/LegacyMidiControlPresetV1/erroneous_extra_second_run.vmp");

    json convertedPreset =
        mpc::input::midi::legacy::parseLegacyMidiControlPresetV1(data);

    std::vector<std::pair<std::string, json>> expectedBindings = {
        {"hardware:pad-1-or-ab",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 35}}},
        {"hardware:pad-2-or-cd",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 36}}},
        {"hardware:pad-3-or-ef",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 37}}},
        {"hardware:pad-4-or-gh",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 38}}},
        {"hardware:pad-5-or-ij",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 39}}},
        {"hardware:pad-6-or-kl",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 40}}},
        {"hardware:pad-7-or-mn",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 41}}},
        {"hardware:pad-8-or-op",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 42}}},
        {"hardware:pad-9-or-qr",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 43}}},
        {"hardware:pad-10-or-st",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 44}}},
        {"hardware:pad-11-or-uv",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 45}}},
        {"hardware:pad-12-or-wx",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 46}}},
        {"hardware:pad-13-or-yz",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 47}}},
        {"hardware:pad-14-or-ampersand-octothorpe",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 48}}},
        {"hardware:pad-15-or-hyphen-eclamation-mark",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 49}}},
        {"hardware:pad-16-or-parentheses",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 50}}},
        {"hardware:data-wheel:positive",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:data-wheel:negative",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:data-wheel:positive",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:data-wheel:negative",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:slider",
         {{"messageType", "CC"}, {"midiChannelIndex", -1}, {"midiNumber", 7}}},
        {"hardware:rec-gain",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:main-volume",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:cursor-left-or-digit",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:cursor-right-or-digit",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:cursor-up",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:cursor-down",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:rec",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:overdub",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:stop",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:play",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:play-start",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:main-screen",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:prev-step-or-event",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:next-step-or-event",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:go-to",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:prev-bar-or-start",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:next-bar-or-end",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:tap-or-note-repeat",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:next-seq",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:track-mute",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:open-window",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:full-level-or-case-switch",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:sixteen-levels-or-space",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:f1",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:f2",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:f3",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:f4",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:f5",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:f6",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:shift",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:shift",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:shift",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:shift",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:enter-or-save",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:undo-seq",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:erase",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:after-or-assign",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:bank-a",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:bank-b",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:bank-c",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"hardware:bank-d",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 97},
          {"enabled", true}}},
    };

    for (const auto &expectedBinding : expectedBindings)
    {
        bool found = false;
        for (const auto &binding : convertedPreset["bindings"])
        {
            if (binding["target"] == expectedBinding.first)
            {
                found = true;
                REQUIRE(binding["messageType"] == expectedBinding.second["messageType"]);
                REQUIRE(binding["midiChannelIndex"] ==
                        expectedBinding.second["midiChannelIndex"]);
                REQUIRE(binding["midiNumber"] == expectedBinding.second["midiNumber"]);
                if (expectedBinding.second.contains("enabled"))
                {
                    REQUIRE(binding["enabled"] == expectedBinding.second["enabled"]);
                }
            }
        }
        REQUIRE(found);
    }
}
