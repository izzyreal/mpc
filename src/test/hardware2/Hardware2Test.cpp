#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include <optional>
#include <unordered_set>

#include "hardware2/Hardware2.h"
#include "ghc/filesystem.hpp"

using namespace mpc::hardware2;
using namespace mpc::inputlogic;

// ---- TESTS ----

TEST_CASE("Hardware2 construction", "[hardware2]")
{
    ClientInputMapper mapper;
    auto kbMapping = std::make_shared<mpc::controls::KbMapping>(fs::temp_directory_path());
    Hardware2 hw(mapper, kbMapping);

    SECTION("Pads initialized")
    {
        REQUIRE(hw.getPads().size() == 16);
        for (int i = 0; i < 16; ++i)
            REQUIRE(hw.getPad(i) != nullptr);
    }

    SECTION("Buttons initialized with valid labels")
    {
        auto labels = hw.getButtonLabels();
        REQUIRE_FALSE(labels.empty());
        for (const auto& l : labels)
            REQUIRE(hw.getButton(l) != nullptr);
    }

    SECTION("LEDs initialized")
    {
        auto leds = hw.getLeds();
        REQUIRE_FALSE(leds.empty());
        auto ledLabels = std::unordered_set<std::string>{
            "full-level", "sixteen-levels", "next-seq", "track-mute",
            "bank-a", "bank-b", "bank-c", "bank-d",
            "after", "undo-seq", "rec", "overdub", "play"
        };

        REQUIRE(leds.size() == ledLabels.size());

        for (auto& l : leds)
        {
            REQUIRE(ledLabels.count(l->getLabel()) > 0);
        }
    }

    SECTION("DataWheel, Slider, Pots are initialized")
    {
        REQUIRE(hw.getDataWheel());
        REQUIRE(hw.getSlider());
        REQUIRE(hw.getRecPot());
        REQUIRE(hw.getVolPot());
    }
}

TEST_CASE("Hardware2 getPad/getButton/getLed safety", "[hardware2]")
{
    ClientInputMapper mapper;
    Hardware2 hw(mapper, nullptr);

    SECTION("getPad out of range returns nullptr")
    {
        REQUIRE(hw.getPad(-1) == nullptr);
        REQUIRE(hw.getPad(16) == nullptr);
    }

    SECTION("getButton with invalid label returns nullptr")
    {
        REQUIRE(hw.getButton("not-a-button") == nullptr);
    }

    SECTION("getLed finds correct LED")
    {
        auto led = hw.getLed("rec");
        REQUIRE(led);
        REQUIRE(led->getLabel() == "rec");
    }

    SECTION("getLed with nonexistent label returns empty")
    {
        REQUIRE(hw.getLed("unknown") == nullptr);
    }

    SECTION("getComponentByLabel for button")
    {
        auto labels = hw.getButtonLabels();
        REQUIRE_FALSE(labels.empty());
        auto label = labels.front();
        auto comp = hw.getComponentByLabel(label);
        REQUIRE(comp);
    }

    SECTION("getComponentByLabel for pad")
    {
        auto comp = hw.getComponentByLabel("pad-0");
        REQUIRE(comp);
        REQUIRE(comp == hw.getPad(0));
    }

    SECTION("getComponentByLabel for invalid label returns nullptr")
    {
        REQUIRE(hw.getComponentByLabel("invalid") == nullptr);
        REQUIRE(hw.getComponentByLabel("pad-999") == nullptr);
    }
}

TEST_CASE("Hardware2 dispatchClientInput behavior", "[hardware2]")
{
    ClientInputMapper mapper;
    Hardware2 hw(mapper, nullptr);

    SECTION("Handles empty optional safely")
    {
        hw.dispatchClientInput(std::nullopt); // should not crash
    }

    SECTION("PadPress invokes correct pad index safely")
    {
        ClientInput input;
        input.type = ClientInput::Type::PadPress;
        input.index = 0;
        input.value = 100;
        hw.dispatchClientInput(input); // should not crash
    }

    SECTION("PadRelease invokes correct pad index safely")
    {
        ClientInput input;
        input.type = ClientInput::Type::PadRelease;
        input.index = 1;
        hw.dispatchClientInput(input);
    }

    SECTION("PadAftertouch invokes correct pad index safely")
    {
        ClientInput input;
        input.type = ClientInput::Type::PadAftertouch;
        input.index = 2;
        input.value = 80;
        hw.dispatchClientInput(input);
    }

    SECTION("ButtonPress invokes correct button safely")
    {
        auto labels = hw.getButtonLabels();
        REQUIRE_FALSE(labels.empty());
        ClientInput input;
        input.type = ClientInput::Type::ButtonPress;
        input.label = labels.front();
        hw.dispatchClientInput(input);
    }

    SECTION("ButtonRelease invokes correct button safely")
    {
        auto labels = hw.getButtonLabels();
        REQUIRE_FALSE(labels.empty());
        ClientInput input;
        input.type = ClientInput::Type::ButtonRelease;
        input.label = labels.back();
        hw.dispatchClientInput(input);
    }

    SECTION("DataWheelTurn handled safely")
    {
        ClientInput input;
        input.type = ClientInput::Type::DataWheelTurn;
        input.value = 1;
        hw.dispatchClientInput(input);
    }

    SECTION("SliderMove handled safely")
    {
        ClientInput input;
        input.type = ClientInput::Type::SliderMove;
        input.value = 42;
        hw.dispatchClientInput(input);
    }

    SECTION("PotMove handled safely (no-op)")
    {
        ClientInput input;
        input.type = ClientInput::Type::PotMove;
        input.value = 10;
        hw.dispatchClientInput(input);
    }
}

#include <catch2/catch_test_macros.hpp>
#include "hardware2/Hardware2.h"

using namespace mpc::hardware2;
using namespace mpc::inputlogic;

TEST_CASE("Hardware2 dispatchHostInput integration with valid tagged events", "[hardware2]") {
    ClientInputMapper mapper;
    auto kbMapping = std::make_shared<mpc::controls::KbMapping>(fs::temp_directory_path());
    Hardware2 hw(mapper, kbMapping);

    SECTION("KeyEvent") {
        KeyEvent key{ true, 42, false, false, false };
        HostInputEvent event(key);

        REQUIRE(event.getSource() == HostInputEvent::Source::KEYBOARD);

        // dispatch should not crash
        hw.dispatchHostInput(event);
    }

    SECTION("GestureEvent") {
        GestureEvent gesture{};
        gesture.componentId = ComponentId::PAD1;
        gesture.type = GestureEvent::Type::BEGIN;
        gesture.normY = 0.5f;

        HostInputEvent event(gesture);
        REQUIRE(event.getSource() == HostInputEvent::Source::GESTURE);

        hw.dispatchHostInput(event);
    }

    SECTION("MidiEvent") {
        MidiEvent midi{};
        midi.messageType = MidiEvent::NOTE;
        midi.data1 = 0;   // pad index 0
        midi.data2 = 100; // velocity
        HostInputEvent event(midi);

        REQUIRE(event.getSource() == HostInputEvent::Source::MIDI);

        hw.dispatchHostInput(event);
    }
}

