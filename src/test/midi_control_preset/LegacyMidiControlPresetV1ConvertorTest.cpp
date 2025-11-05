#include "catch2/catch_test_macros.hpp"
#include "controls/midi/legacy/LegacyMidiControlPresetV1Convertor.hpp"
#include "controls/midi/legacy/LegacyMidiControlPresetPatcher.hpp"
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <cmrc/cmrc.hpp>
#include <string>
#include <iostream>
#include <set>

#include "iRigPadsUtil.hpp"

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

TEST_CASE("Legacy preset V1 conversion validates against new schema",
          "[legacy-midi-control-preset-v1-conversion]")
{
    // Load legacy binary preset
    auto data = load_resource("test/LegacyMidiControlPresetV1/iRig_PADS.vmp");

    // Convert to JSON using the parser
    json convertedPreset =
        mpc::controls::midi::legacy::parseLegacyMidiControlPresetV1(data);
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

        // std::cerr << "Converted JSON:\n" << convertedPreset.dump(4) << "\n";

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

TEST_CASE(
    "Legacy preset V1 parses all ' (extra)' labels correctly and preserves "
    "values after patching",
    "[legacy-midi-control-preset-v1-conversion]")
{
    // Load legacy binary preset with extra labels
    auto data = load_resource(
        "test/LegacyMidiControlPresetV1/erroneous_extra_first_run.vmp");

    // Convert to JSON using the parser
    json convertedPreset =
        mpc::controls::midi::legacy::parseLegacyMidiControlPresetV1(data);

    // Expected extra labels
    std::set<std::string> expectedLabels = {
        "0 (extra)", "1 (extra)", "2 (extra)", "3 (extra)", "4 (extra)",
        "5 (extra)", "6 (extra)", "7 (extra)", "8 (extra)", "9 (extra)"};

    // Collect actual labels from bindings and set interesting values
    std::set<std::string> actualLabels;
    std::map<std::string, json> bindingValues;
    int index = 0;
    for (auto &binding : convertedPreset["bindings"])
    {
        if (binding.contains("labelName") && binding["labelName"].is_string())
        {
            std::string label = binding["labelName"].get<std::string>();
            actualLabels.insert(label);

            // Set interesting values for extra labels
            if (label.find("(extra)") != std::string::npos)
            {
                binding["messageType"] = (index % 2 == 0) ? "CC" : "Note";
                if (index % 2 == 0)
                {
                    binding["midiValue"] = 20 + index;
                }
                binding["midiChannelIndex"] = index;
                binding["enabled"] = (index % 2 == 0);
                binding["midiNumber"] = 10 + index;
                bindingValues[label] = binding;
                index++;
            }
        }
    }

    // Check that all expected extra labels are present
    for (const auto &expected : expectedLabels)
    {
        REQUIRE(actualLabels.find(expected) != actualLabels.end());
    }

    // Verify exactly 10 extra labels were found
    size_t extraLabelCount = 0;
    for (const auto &label : actualLabels)
    {
        if (label.find("(extra)") != std::string::npos)
        {
            extraLabelCount++;
        }
    }
    REQUIRE(extraLabelCount == 10);

    SUCCEED(
        "All ' (extra)' labels parsed correctly from "
        "erroneous_extra_first_run.vmp.");

    // Apply patching
    json schemaJson = json::parse(
        load_resource("test/MidiControlPreset/"
                      "vmpc2000xl_midi_control_preset.schema.v3.json"));
    mpc::controls::midi::legacy::patchLegacyPreset(convertedPreset, schemaJson);

    // Verify that values survived under new label names
    for (int i = 0; i < 10; ++i)
    {
        std::string oldLabel = std::to_string(i) + " (extra)";
        std::string newLabel = std::to_string(i) + "_extra";

        // Find the binding with the new label
        bool found = false;
        for (const auto &binding : convertedPreset["bindings"])
        {
            if (binding["labelName"] == newLabel)
            {
                found = true;
                // Compare values with those set before patching
                REQUIRE(binding["messageType"] ==
                        bindingValues[oldLabel]["messageType"]);
                REQUIRE(binding["midiChannelIndex"] ==
                        bindingValues[oldLabel]["midiChannelIndex"]);
                REQUIRE(binding["enabled"] ==
                        bindingValues[oldLabel]["enabled"]);
                REQUIRE(binding["midiNumber"] ==
                        bindingValues[oldLabel]["midiNumber"]);
                if (binding["messageType"] == "CC")
                {
                    REQUIRE(binding["midiValue"] ==
                            bindingValues[oldLabel]["midiValue"]);
                }
            }
        }
        REQUIRE(found); // Ensure the new label exists
    }

    SUCCEED("All extra label values preserved after patching.");
}

TEST_CASE(
    "Legacy preset V1 with erroneously parsed and persisted ' (extra)' labels "
    "preserves valid binding values after patching",
    "[legacy-midi-control-preset-v1-conversion]")
{
    auto data = load_resource(
        "test/LegacyMidiControlPresetV1/erroneous_extra_second_run.vmp");

    // Convert to JSON using the parser
    json convertedPreset =
        mpc::controls::midi::legacy::parseLegacyMidiControlPresetV1(data);
    json schemaJson = json::parse(
        load_resource("test/MidiControlPreset/"
                      "vmpc2000xl_midi_control_preset.schema.v3.json"));

    // Store expected values for valid bindings
    std::vector<std::pair<std::string, json>> expectedBindings = {
        {"pad-1",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 35}}},
        {"pad-2",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 36}}},
        {"pad-3",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 37}}},
        {"pad-4",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 38}}},
        {"pad-5",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 39}}},
        {"pad-6",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 40}}},
        {"pad-7",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 41}}},
        {"pad-8",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 42}}},
        {"pad-9",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 43}}},
        {"pad-10",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 44}}},
        {"pad-11",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 45}}},
        {"pad-12",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 46}}},
        {"pad-13",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 47}}},
        {"pad-14",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 48}}},
        {"pad-15",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 49}}},
        {"pad-16",
         {{"messageType", "Note"},
          {"midiChannelIndex", -1},
          {"midiNumber", 50}}},
        {"datawheel",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"datawheel-up",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"datawheel-down",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"slider",
         {{"messageType", "CC"}, {"midiChannelIndex", -1}, {"midiNumber", 7}}},
        {"rec-gain",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"main-volume",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"left",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"right",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"up",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"down",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"rec",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"overdub",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"stop",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"play",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"play-start",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"main-screen",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"prev-step-event",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"next-step-event",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"go-to",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"prev-bar-start",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"next-bar-end",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"tap",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"next-seq",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"track-mute",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"open-window",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"full-level",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"sixteen-levels",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"f1",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"f2",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"f3",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"f4",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"f5",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"f6",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"shift",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"shift_#1",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"shift_#2",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"shift_#3",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"enter",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"undo-seq",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"erase",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"after",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"bank-a",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"bank-b",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"bank-c",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 0},
          {"enabled", false}}},
        {"bank-d",
         {{"messageType", "CC"},
          {"midiChannelIndex", -1},
          {"midiNumber", 97},
          {"enabled", true}}},
    };

    // Apply patching

    mpc::controls::midi::legacy::patchLegacyPreset(convertedPreset, schemaJson);
    // Verify valid bindings retain their values
    for (const auto &[label, expected] : expectedBindings)
    {
        bool found = false;
        for (const auto &binding : convertedPreset["bindings"])
        {
            if (binding["labelName"] == label)
            {
                found = true;
                REQUIRE(binding["messageType"] == expected["messageType"]);
                REQUIRE(binding["midiChannelIndex"] ==
                        expected["midiChannelIndex"]);
                REQUIRE(binding["midiNumber"] == expected["midiNumber"]);
                if (expected.contains("enabled"))
                {
                    REQUIRE(binding["enabled"] == expected["enabled"]);
                }
            }
        }
        REQUIRE(found); // Ensure the binding exists
    }

    SUCCEED("Valid bindings preserved values after patching.");
}
