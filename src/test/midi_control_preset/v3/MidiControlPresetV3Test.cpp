#include "catch2/catch_test_macros.hpp"

#include "input/midi/MidiControlPresetV3.hpp"
#include "input/midi/MidiControlPresetUtil.hpp"

#include <nlohmann/json.hpp>
#include <cmrc/cmrc.hpp>
#include <set>
#include <stdexcept>

CMRC_DECLARE(mpctest);

using namespace mpc::input::midi;
using nlohmann::json;

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

TEST_CASE("MidiControlPresetV3 rejects wrong binding targets",
          "[MidiControlPresetV3]")
{
    MidiControlPresetV3 preset;
    std::vector<Binding> b(81);
    for (int i = 0; i < 81; ++i)
    {
        b[i].setTarget("pad-" + std::to_string(i));
        b[i].setMessageType("Note");
        b[i].setMidiNumber(60);
        b[i].setMidiChannelIndex(0);
        b[i].setEnabled(true);
    }
    REQUIRE_THROWS_AS(preset.setBindings(b), std::invalid_argument);
}

TEST_CASE("MidiControlPresetV3 accepts correct binding target set",
          "[MidiControlPresetV3]")
{
    MidiControlPresetV3 preset;
    auto targets = load_available_targets();
    std::vector<Binding> b;
    b.reserve(targets.size());

    for (auto &target : targets)
    {
        Binding bind;
        bind.setTarget(target);
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

    auto targets = load_available_targets();
    std::vector<Binding> b;
    b.reserve(targets.size());
    for (auto &lbl : targets)
    {
        Binding bind;
        bind.setTarget(lbl);
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
    REQUIRE(restored.getBindings().size() == 74);
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

TEST_CASE("Binding::from_json enforces midiValue rules",
          "[MidiControlPresetV3]")
{
    json validCC = {{"target", "left"},   {"messageType", "CC"},
                    {"midiNumber", 10},      {"midiValue", 20},
                    {"midiChannelIndex", 0}, {"enabled", true}};
    json invalidCC = validCC;
    invalidCC.erase("midiValue");

    json validNote = {{"target", "right"},
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

    auto targets = load_available_targets();
    std::vector<Binding> b;
    for (auto &lbl : targets)
    {
        Binding bind;
        bind.setTarget(lbl);
        bind.setMessageType("Note");
        bind.setMidiNumber(10);
        bind.setMidiChannelIndex(0);
        bind.setEnabled(true);
        b.push_back(bind);
    }
    preset.setBindings(b);

    json j = preset;
    MidiControlPresetV3 copy = j.get<MidiControlPresetV3>();
    REQUIRE(copy.getMidiControllerDeviceName() == "MPK Mini Plus");
}

TEST_CASE("MidiControlPresetV3 rejects unknown JSON keys",
          "[MidiControlPresetV3]")
{
    std::vector<json> bindings(81);
    for (int i = 0; i < 81; ++i)
    {
        bindings[i] = {{"target", "left"},
                       {"messageType", "CC"}, {"midiNumber", 0},
                       {"midiValue", 0},      {"midiChannelIndex", 0},
                       {"enabled", false}};
    }

    json j = {
        {"version", 3},
        {"name", "Preset1"},
        {"autoLoad", "No"},
        {"bindings", bindings},
        {"unknownField", "oops"}
    };

    REQUIRE_THROWS_AS(j.get<MidiControlPresetV3>(), std::invalid_argument);
}
