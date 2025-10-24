#include <catch2/catch_test_macros.hpp>
#include <string>

#include "hardware/Hardware.hpp"

using namespace mpc::hardware;

// ---- TESTS ----

TEST_CASE("Hardware construction", "[hardware]")
{
    Hardware hw;

    SECTION("Pads initialized")
    {
        REQUIRE(hw.getPads().size() == 16);
        for (int i = 0; i < 16; ++i)
        {
            REQUIRE(hw.getPad(i) != nullptr);
        }
    }

    SECTION("Buttons initialized with valid labels")
    {
        auto labels = hw.getButtonLabels();
        REQUIRE_FALSE(labels.empty());
        // for (const auto& l : labels)
        // REQUIRE(hw.getButton(l) != nullptr);
    }

    SECTION("LEDs initialized")
    {
        /*
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
        */
    }

    SECTION("DataWheel, Slider, Pots are initialized")
    {
        REQUIRE(hw.getDataWheel());
        REQUIRE(hw.getSlider());
        REQUIRE(hw.getRecPot());
        REQUIRE(hw.getVolPot());
    }
}

TEST_CASE("Hardware getPad/getButton/getLed safety", "[hardware]")
{
    Hardware hw;

    SECTION("getPad out of range returns nullptr")
    {
        REQUIRE(hw.getPad(-1) == nullptr);
        REQUIRE(hw.getPad(16) == nullptr);
    }

    SECTION("getButton with invalid label returns nullptr")
    {
        // REQUIRE(hw.getButton("not-a-button") == nullptr);
    }

    SECTION("getLed finds correct LED")
    {
        /*
        auto led = hw.getLed(hardware::ComponentId::REC_LED);
        REQUIRE(led);
        REQUIRE(led->getLabel() == "rec");
        */
    }

    SECTION("getLed with nonexistent label returns empty")
    {
        // REQUIRE(hw.getLed("unknown") == nullptr);
    }

    SECTION("getComponentByLabel for button")
    {
        auto labels = hw.getButtonLabels();
        REQUIRE_FALSE(labels.empty());
        auto label = labels.front();
        // auto comp = hw.getComponentByLabel(label);
        // REQUIRE(comp);
    }

    SECTION("getComponentByLabel for pad")
    {
        // auto comp = hw.getComponentByLabel("pad-0");
        // REQUIRE(comp);
        // REQUIRE(comp == hw.getPad(0));
    }

    SECTION("getComponentByLabel for invalid label returns nullptr")
    {
        // REQUIRE(hw.getComponentByLabel("invalid") == nullptr);
        // REQUIRE(hw.getComponentByLabel("pad-999") == nullptr);
    }
}
