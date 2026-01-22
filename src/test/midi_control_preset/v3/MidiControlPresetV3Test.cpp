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

TEST_CASE("MidiControlPresetV3 rejects invalid autoLoadMode values",
          "[MidiControlPresetV3]")
{
    MidiControlPresetV3 preset;
    REQUIRE_THROWS_AS(preset.setAutoLoadMode(static_cast<AutoLoadMode>(3)),
                      std::invalid_argument);

    REQUIRE_NOTHROW(preset.setAutoLoadMode(AutoLoadModeNo));
    REQUIRE_NOTHROW(preset.setAutoLoadMode(AutoLoadModeYes));
    REQUIRE_NOTHROW(preset.setAutoLoadMode(AutoLoadModeAsk));
}

TEST_CASE("MidiControlPresetV3 rejects wrong binding targets",
          "[MidiControlPresetV3]")
{
    MidiControlPresetV3 preset;
    std::vector<Binding> b(81);
    for (int i = 0; i < 81; ++i)
    {
        b[i].setTarget("pad-" + std::to_string(i));
        b[i].setMessageType(BindingMessageType::Note);
        b[i].setMidiNumber(60);
        b[i].setMidiChannelIndex(0);
    }
    REQUIRE_THROWS_AS(preset.setBindings(b), std::invalid_argument);
}

TEST_CASE("MidiControlPresetV3 accepts correct binding target set",
          "[MidiControlPresetV3]")
{
    MidiControlPresetV3 preset;
    auto targets = MidiControlPresetUtil::load_available_targets();
    std::vector<Binding> b;
    b.reserve(targets.size());

    for (auto &target : targets)
    {
        Binding bind;
        bind.setTarget(target);
        bind.setMessageType(BindingMessageType::Controller);
        bind.setMidiNumber(1);
        bind.setMidiValue(10);
        bind.setMidiChannelIndex(0);
        b.push_back(bind);
    }

    REQUIRE_NOTHROW(preset.setBindings(b));
}

TEST_CASE("MidiControlPresetV3 round-trips via JSON", "[MidiControlPresetV3]")
{
    MidiControlPresetV3 preset;
    preset.setName("RoundTrip");
    preset.setAutoLoadMode(AutoLoadModeAsk);

    std::vector<Binding> bindings;

    // 1) Hardware button, CC with midiValue
    {
        Binding b;
        b.setTarget("hardware:play");
        b.setMessageType(BindingMessageType::Controller);
        b.setMidiNumber(10);
        b.setMidiValue(64);
        b.setMidiChannelIndex(0);
        bindings.push_back(b);
    }

    // 2) Pad, Note without midiValue
    {
        Binding b;
        b.setTarget("hardware:pad-1-or-ab");
        b.setMessageType(BindingMessageType::Note);
        b.setMidiNumber(36);
        b.setMidiChannelIndex(1);
        bindings.push_back(b);
    }

    // 3) Pot/slider, CC without midiValue
    {
        Binding b;
        b.setTarget("hardware:slider");
        b.setMessageType(BindingMessageType::Controller);
        b.setMidiNumber(7);
        b.setMidiChannelIndex(2);
        bindings.push_back(b);
    }

    // 4) Encoder-style data wheel (no midiValue, has encoderMode)
    {
        Binding b;
        b.setTarget("hardware:data-wheel");
        b.setMessageType(BindingMessageType::Controller);
        b.setEncoderMode(BindingEncoderMode::RelativeStateless);
        b.setMidiNumber(16);
        b.setMidiChannelIndex(0);
        bindings.push_back(b);
    }

    // 5) Button-like data wheel negative (has no midiValue)
    {
        Binding b;
        b.setTarget("hardware:data-wheel:negative");
        b.setMessageType(BindingMessageType::Controller);
        b.setMidiNumber(17);
        b.setMidiChannelIndex(0);
        bindings.push_back(b);
    }

    preset.setBindings(bindings);

    json j = preset;
    auto restored = j.get<MidiControlPresetV3>();

    REQUIRE(restored.getVersion() == CURRENT_PRESET_VERSION);
    REQUIRE(restored.getName() == "RoundTrip");
    REQUIRE(restored.getAutoLoadMode() == AutoLoadModeAsk);
    REQUIRE(restored.getBindings().size() == bindings.size());

    const auto &rb = restored.getBindings();

    // 1) Hardware button
    REQUIRE(rb[0].getTarget() == "hardware:play");
    REQUIRE(rb[0].isController());
    REQUIRE(rb[0].getMidiNumber() == 10);
    REQUIRE(rb[0].getMidiValue() == 64);
    REQUIRE(rb[0].getMidiChannelIndex() == 0);
    REQUIRE(rb[0].isEnabled());

    // 2) Pad note, no midiValue
    REQUIRE(rb[1].getTarget() == "hardware:pad-1-or-ab");
    REQUIRE(rb[1].isNote());
    REQUIRE(rb[1].getMidiNumber() == 36);
    REQUIRE(rb[1].getMidiChannelIndex() == 1);
    REQUIRE(rb[1].isEnabled());

    // 3) Slider CC, no midiValue
    REQUIRE(rb[2].getTarget() == "hardware:slider");
    REQUIRE(rb[2].isController());
    REQUIRE(rb[2].getMidiNumber() == 7);
    REQUIRE(rb[2].getMidiChannelIndex() == 2);
    REQUIRE(rb[2].isEnabled());

    // 4) Encoder-style data wheel
    REQUIRE(rb[3].getTarget() == "hardware:data-wheel");
    REQUIRE(rb[3].isController());
    REQUIRE(rb[3].getEncoderMode() == BindingEncoderMode::RelativeStateless);
    REQUIRE(rb[3].getMidiNumber() == 16);
    REQUIRE(rb[3].getMidiChannelIndex() == 0);
    REQUIRE(rb[3].isEnabled());

    // 5) Button-like data wheel negative
    REQUIRE(rb[4].getTarget() == "hardware:data-wheel:negative");
    REQUIRE(rb[4].isController());
    REQUIRE(rb[4].getMidiNumber() == 17);
    REQUIRE(rb[4].getMidiChannelIndex() == 0);
    REQUIRE(rb[4].isEnabled());
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
    json validCC = {{"target", "hardware:cursor-left-or-digit"},
                    {"messageType", "controller"},
                    {"midiNumber", 10},
                    {"midiValue", 20},
                    {"midiChannelIndex", 0},
                    {"enabled", true}};

    json invalidCC = validCC;
    invalidCC.erase("midiValue");

    json validNote = {{"target", "right"},
                      {"messageType", "note"},
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
    preset.setAutoLoadMode(AutoLoadModeYes);
    preset.setMidiControllerDeviceName("MPK Mini Plus");

    auto targets = MidiControlPresetUtil::load_available_targets();
    std::vector<Binding> b;
    for (auto &lbl : targets)
    {
        Binding bind;
        bind.setTarget(lbl);
        bind.setMessageType(BindingMessageType::Note);
        bind.setMidiNumber(10);
        bind.setMidiChannelIndex(0);
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
        bindings[i] = {{"target", "left"},      {"messageType", "CC"},
                       {"midiNumber", 0},       {"midiValue", 0},
                       {"midiChannelIndex", 0}, {"enabled", false}};
    }

    json j = {{"version", 3},
              {"name", "Preset1"},
              {"autoLoad", "No"},
              {"bindings", bindings},
              {"unknownField", "oops"}};

    REQUIRE_THROWS_AS(j.get<MidiControlPresetV3>(), std::invalid_argument);
}
