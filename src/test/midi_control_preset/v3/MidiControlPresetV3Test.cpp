#include "catch2/catch_test_macros.hpp"
#include "controls/midi/MidiControlPresetV3.h"
#include <nlohmann/json.hpp>
#include <cmrc/cmrc.hpp>
#include <set>
#include <stdexcept>

CMRC_DECLARE(mpctest);

using namespace mpc::controls::midi;
using nlohmann::json;

// --- Utility: load embedded resource file ---
inline std::string load_resource(const std::string &path)
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open(path);
    return std::string(file.begin(), file.end());
}

// --- Utility: extract required labels directly from the schema ---
inline std::set<std::string> load_required_labels()
{
    json schema = json::parse(
        load_resource("test/MidiControlPreset/"
                      "vmpc2000xl_midi_control_preset.schema.v3.json"));

    if (!schema.contains("$defs") || !schema["$defs"].contains("binding") ||
        !schema["$defs"]["binding"].contains("properties") ||
        !schema["$defs"]["binding"]["properties"].contains("labelName") ||
        !schema["$defs"]["binding"]["properties"]["labelName"].contains("enum"))
    {
        throw std::runtime_error(
            "Schema missing labelName enum at expected path");
    }

    const auto &enumArray =
        schema["$defs"]["binding"]["properties"]["labelName"]["enum"];
    std::set<std::string> labels;
    for (const auto &v : enumArray)
    {
        labels.insert(v.get<std::string>());
    }

    if (labels.size() != 81)
    {
        throw std::runtime_error("Expected 81 labels in schema labelName enum");
    }

    return labels;
}

// --- TESTS ---

TEST_CASE("MidiControlPresetV3 rejects invalid preset names",
          "[MidiControlPresetV3]")
{
    MidiControlPresetV3 preset;
    REQUIRE_THROWS_AS(preset.setName(""), std::invalid_argument);
    REQUIRE_THROWS_AS(preset.setName("ThisNameIsWayTooLongToPass"),
                      std::invalid_argument);
    REQUIRE_NOTHROW(preset.setName("GoodName"));
}

TEST_CASE("MidiControlPresetV3 rejects invalid autoLoad values",
          "[MidiControlPresetV3]")
{
    MidiControlPresetV3 preset;
    REQUIRE_THROWS_AS(preset.setAutoLoad("Sometimes"), std::invalid_argument);
    REQUIRE_NOTHROW(preset.setAutoLoad("Ask"));
}

TEST_CASE("MidiControlPresetV3 rejects wrong binding count",
          "[MidiControlPresetV3]")
{
    MidiControlPresetV3 preset;
    std::vector<Binding> b(80);
    REQUIRE_THROWS_AS(preset.setBindings(b), std::invalid_argument);
}

TEST_CASE("MidiControlPresetV3 rejects wrong binding labels",
          "[MidiControlPresetV3]")
{
    MidiControlPresetV3 preset;
    std::vector<Binding> b(81);
    for (int i = 0; i < 81; ++i)
    {
        b[i].setLabelName("pad-" + std::to_string(i)); // invalid labels
        b[i].setMessageType("Note");
        b[i].setMidiNumber(60);
        b[i].setMidiChannelIndex(0);
        b[i].setEnabled(true);
    }
    REQUIRE_THROWS_AS(preset.setBindings(b), std::invalid_argument);
}

TEST_CASE("MidiControlPresetV3 accepts correct binding label set",
          "[MidiControlPresetV3]")
{
    MidiControlPresetV3 preset;
    auto labels = load_required_labels();
    std::vector<Binding> b;
    b.reserve(labels.size());

    for (auto &lbl : labels)
    {
        Binding bind;
        bind.setLabelName(lbl);
        bind.setMessageType("CC");
        bind.setMidiNumber(1);
        bind.setMidiValue(10);
        bind.setMidiChannelIndex(0);
        bind.setEnabled(true);
        b.push_back(bind);
    }

    REQUIRE_NOTHROW(preset.setBindings(b));
}

TEST_CASE("MidiControlPresetV3 round-trips via JSON", "[MidiControlPresetV3]")
{
    MidiControlPresetV3 preset;
    preset.setName("RoundTrip");
    preset.setAutoLoad("Ask");

    auto labels = load_required_labels();
    std::vector<Binding> b;
    b.reserve(labels.size());
    for (auto &lbl : labels)
    {
        Binding bind;
        bind.setLabelName(lbl);
        bind.setMessageType("CC");
        bind.setMidiNumber(10);
        bind.setMidiValue(127);
        bind.setMidiChannelIndex(0);
        bind.setEnabled(true);
        b.push_back(bind);
    }
    preset.setBindings(b);

    json j = preset;
    MidiControlPresetV3 restored = j.get<MidiControlPresetV3>();

    REQUIRE(restored.getVersion() == CURRENT_PRESET_VERSION);
    REQUIRE(restored.getName() == "RoundTrip");
    REQUIRE(restored.getAutoLoad() == "Ask");
    REQUIRE(restored.getBindings().size() == 81);
}

TEST_CASE("MidiControlPresetV3 rejects out-of-range versions",
          "[MidiControlPresetV3]")
{
    MidiControlPresetV3 preset;
    REQUIRE_THROWS_AS(preset.setVersion(-1), std::out_of_range);
    REQUIRE_THROWS_AS(preset.setVersion(4503599627370497LL), std::out_of_range);
}

TEST_CASE("MidiControlPresetV3 rejects invalid midiControllerDeviceName",
          "[MidiControlPresetV3]")
{
    MidiControlPresetV3 preset;
    REQUIRE_NOTHROW(preset.setMidiControllerDeviceName("MPK Mini Plus"));
    REQUIRE_THROWS_AS(preset.setMidiControllerDeviceName(std::string(256, 'a')),
                      std::invalid_argument);
    REQUIRE_THROWS_AS(preset.setMidiControllerDeviceName("💩"),
                      std::invalid_argument);
}

TEST_CASE("Binding rejects out-of-range MIDI values", "[MidiControlPresetV3]")
{
    Binding b;
    b.setLabelName("left");
    b.setMessageType("CC");

    REQUIRE_THROWS_AS(b.setMidiNumber(-1), std::out_of_range);
    REQUIRE_THROWS_AS(b.setMidiNumber(128), std::out_of_range);
    REQUIRE_THROWS_AS(b.setMidiValue(-2), std::out_of_range);
    REQUIRE_THROWS_AS(b.setMidiValue(128), std::out_of_range);
    REQUIRE_THROWS_AS(b.setMidiChannelIndex(16), std::out_of_range);
    REQUIRE_THROWS_AS(b.setMidiChannelIndex(-2), std::out_of_range);
}

TEST_CASE("Binding::from_json enforces midiValue rules",
          "[MidiControlPresetV3]")
{
    using json = nlohmann::json;
    json validCC = {{"labelName", "left"},   {"messageType", "CC"},
                    {"midiNumber", 10},      {"midiValue", 20},
                    {"midiChannelIndex", 0}, {"enabled", true}};
    json invalidCC = validCC;
    invalidCC.erase("midiValue");

    json validNote = {{"labelName", "right"},
                      {"messageType", "Note"},
                      {"midiNumber", 10},
                      {"midiChannelIndex", 0},
                      {"enabled", true}};
    json invalidNote = validNote;
    invalidNote["midiValue"] = 99;

    REQUIRE_NOTHROW(validCC.get<Binding>());
    REQUIRE_THROWS_AS(invalidCC.get<Binding>(), std::invalid_argument);
    REQUIRE_NOTHROW(validNote.get<Binding>());
    REQUIRE_THROWS_AS(invalidNote.get<Binding>(), std::invalid_argument);
}

TEST_CASE("MidiControlPresetV3 preserves midiControllerDeviceName through JSON",
          "[MidiControlPresetV3]")
{
    MidiControlPresetV3 preset;
    preset.setName("Named");
    preset.setAutoLoad("Yes");
    preset.setMidiControllerDeviceName("MPK Mini Plus");

    auto labels = load_required_labels();
    std::vector<Binding> b;
    for (auto &lbl : labels)
    {
        Binding bind;
        bind.setLabelName(lbl);
        bind.setMessageType("Note");
        bind.setMidiNumber(10);
        bind.setMidiChannelIndex(0);
        bind.setEnabled(true);
        b.push_back(bind);
    }
    preset.setBindings(b);

    nlohmann::json j = preset;
    MidiControlPresetV3 copy = j.get<MidiControlPresetV3>();
    REQUIRE(copy.getMidiControllerDeviceName() == "MPK Mini Plus");
}

TEST_CASE("MidiControlPresetV3 rejects unknown JSON keys",
          "[MidiControlPresetV3]")
{
    using json = nlohmann::json;

    // Minimal valid bindings for the test
    std::vector<json> bindings(81);
    for (int i = 0; i < 81; ++i)
    {
        bindings[i] = {{"labelName", "left"}, // the label doesn't matter here
                       {"messageType", "CC"}, {"midiNumber", 0},
                       {"midiValue", 0},      {"midiChannelIndex", 0},
                       {"enabled", false}};
    }

    json j = {
        {"version", 3},
        {"name", "Preset1"},
        {"autoLoad", "No"},
        {"bindings", bindings},
        {"unknownField", "oops"} // <-- should trigger error
    };

    REQUIRE_THROWS_AS(j.get<MidiControlPresetV3>(), std::invalid_argument);
}
