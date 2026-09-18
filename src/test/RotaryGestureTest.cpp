#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "input/HostToClientTranslator.hpp"
#include "TestMpc.hpp"
#include "hardware/Component.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/VmpcSettingsScreen.hpp"
#include <limits>

using namespace mpc::input;
using namespace mpc::hardware;
using mpc::client::event::ClientHardwareEvent;

namespace
{
    GestureEvent pointer(GestureEvent::Type type, ComponentId id, float x,
                         float y, int source = 0)
    {
        return {type,   GestureEvent::Movement::RotaryDrag,
                x,      y,
                0.f,    0,
                source, id,
                false,  false,
                false,  GestureEvent::InputDeviceType::Mouse};
    }

    float delta(HostToClientTranslator &translator, GestureEvent gesture)
    {
        const auto result =
            translator.translate(HostInputEvent{gesture}, nullptr);
        REQUIRE(result);
        const auto &hardware = std::get<ClientHardwareEvent>(result->payload);
        REQUIRE(hardware.deltaValue);
        return *hardware.deltaValue;
    }

    void begin(HostToClientTranslator &translator, GestureEvent gesture,
               RotaryDragMode mode = RotaryDragMode::ByPosition)
    {
        const auto result =
            translator.translate(HostInputEvent{gesture}, nullptr, mode);
        REQUIRE(result);
        REQUIRE_FALSE(
            std::get<ClientHardwareEvent>(result->payload).deltaValue);
    }
} // namespace

TEST_CASE(
    "Rotary drag modes are selected at contact and stay fixed until release",
    "[rotary][input]")
{
    HostToClientTranslator translator;
    begin(translator, pointer(GestureEvent::Type::BEGIN, DATA_WHEEL, 0.5f, 0.f),
          RotaryDragMode::Vertical);
    auto update = pointer(GestureEvent::Type::UPDATE, DATA_WHEEL, 1.f, 0.5f);
    update.continuousDelta = 10.f;
    REQUIRE(delta(translator, update) == Catch::Approx(3.f));
    // Circular mode also works when starting inside the auto-mode threshold.
    begin(translator,
          pointer(GestureEvent::Type::BEGIN, DATA_WHEEL, 0.5f, 0.3f),
          RotaryDragMode::Circular);
    update.normX = 0.7f;
    REQUIRE(delta(translator, update) == Catch::Approx(25.f));
    // Exact-center contact waits for a usable angle, without jumping.
    begin(translator,
          pointer(GestureEvent::Type::BEGIN, DATA_WHEEL, 0.5f, 0.5f),
          RotaryDragMode::Circular);
    REQUIRE_FALSE(translator.translate(HostInputEvent{update}, nullptr));
    update.normX = 0.5f;
    update.normY = 0.7f;
    REQUIRE(delta(translator, update) == Catch::Approx(25.f));
}

TEST_CASE("Rotary input reaches the pot model and cancels on focus loss",
          "[rotary][input]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutIoServices(mpc);
    mpc.getLayeredScreen()->openScreenById(
        mpc::lcdgui::ScreenId::VmpcSettingsScreen);
    const auto settings =
        mpc.screens->get<mpc::lcdgui::ScreenId::VmpcSettingsScreen>();
    for (int i = 0; i < 8; ++i)
    {
        settings->down();
    }
    settings->turnWheel(1);
    for (const auto id : {REC_GAIN_POT, MAIN_VOLUME_POT})
    {
        const auto pot = id == REC_GAIN_POT ? mpc.getHardware()->getRecPot()
                                            : mpc.getHardware()->getVolPot();
        pot->setValue(0.9f);
        mpc.dispatchHostInput(
            HostInputEvent{pointer(GestureEvent::Type::BEGIN, id, 0.5f, 0.f)});
        REQUIRE(pot->getValue() == Catch::Approx(0.9f));
        mpc.dispatchHostInput(
            HostInputEvent{pointer(GestureEvent::Type::UPDATE, id, 1.f, 0.5f)});
        REQUIRE(pot->getValue() == Catch::Approx(1.f));
        mpc.dispatchHostInput(
            HostInputEvent{pointer(GestureEvent::Type::UPDATE, id, 0.5f, 0.f)});
        REQUIRE(pot->getValue() == Catch::Approx(1.f - 90.f / 306.f));
        pot->setValue(0.1f);
        mpc.dispatchHostInput(
            HostInputEvent{pointer(GestureEvent::Type::UPDATE, id, 0.f, 0.5f)});
        REQUIRE(pot->getValue() == Catch::Approx(0.f));
        mpc.dispatchHostInput(
            HostInputEvent{FocusEvent{FocusEvent::Type::Lost}});
        REQUIRE(mpc.dispatchHostInput(HostInputEvent{
                    pointer(GestureEvent::Type::UPDATE, id, 0.5f, 0.f)}) ==
                HostInputResult::Ignored);
        REQUIRE(pot->getValue() == Catch::Approx(0.f));
    }
}

TEST_CASE(
    "Rotary wheel tracks full turns and reversals for every pointer device",
    "[rotary][input]")
{
    for (const auto device : {GestureEvent::InputDeviceType::Mouse,
                              GestureEvent::InputDeviceType::Touch,
                              GestureEvent::InputDeviceType::Pen})
    {
        HostToClientTranslator translator;
        auto gesture =
            pointer(GestureEvent::Type::BEGIN, DATA_WHEEL, 0.5f, 0.f);
        gesture.inputDeviceType = device;
        begin(translator, gesture);
        gesture.type = GestureEvent::Type::UPDATE;
        for (const auto point :
             {std::pair{1.f, 0.5f}, {0.5f, 1.f}, {0.f, 0.5f}, {0.5f, 0.f}})
        {
            gesture.normX = point.first;
            gesture.normY = point.second;
            REQUIRE(delta(translator, gesture) == Catch::Approx(25.f));
        }
        gesture.normX = 0.f;
        gesture.normY = 0.5f;
        REQUIRE(delta(translator, gesture) == Catch::Approx(-25.f));
        gesture.shiftDown = true;
        gesture.normX = 0.5f;
        gesture.normY = 0.f;
        REQUIRE(delta(translator, gesture) == Catch::Approx(250.f));
    }
}

TEST_CASE("Rotary pots follow their visual sweep without an initial jump",
          "[rotary][input]")
{
    for (const auto id : {REC_GAIN_POT, MAIN_VOLUME_POT})
    {
        HostToClientTranslator translator;
        begin(translator, pointer(GestureEvent::Type::BEGIN, id, 0.5f, 0.f));
        REQUIRE(delta(translator,
                      pointer(GestureEvent::Type::UPDATE, id, 1.f, 0.5f)) ==
                Catch::Approx(100.f * 90.f / 306.f));
        REQUIRE(delta(translator,
                      pointer(GestureEvent::Type::UPDATE, id, 0.5f, 0.f)) ==
                Catch::Approx(-100.f * 90.f / 306.f));
    }
}

TEST_CASE("Center drags and scroll input retain their existing sensitivity",
          "[rotary][input]")
{
    for (const auto id : {DATA_WHEEL, REC_GAIN_POT, MAIN_VOLUME_POT})
    {
        HostToClientTranslator translator;
        begin(translator, pointer(GestureEvent::Type::BEGIN, id, 0.5f, 0.5f));
        auto gesture = pointer(GestureEvent::Type::UPDATE, id, 0.5f, -2.f);
        gesture.continuousDelta = 10.f;
        REQUIRE(delta(translator, gesture) ==
                Catch::Approx(id == DATA_WHEEL ? 3.f : 10.f));
        gesture.movement = GestureEvent::Movement::Relative;
        REQUIRE(delta(translator, gesture) ==
                Catch::Approx(id == DATA_WHEEL ? 3.f : 10.f));
    }
}

TEST_CASE("Rotary drags unwrap the angle seam and capture outside bounds",
          "[rotary][input]")
{
    HostToClientTranslator translator;
    begin(translator,
          pointer(GestureEvent::Type::BEGIN, DATA_WHEEL, 0.f, 0.51f));
    REQUIRE(delta(translator, pointer(GestureEvent::Type::UPDATE, DATA_WHEEL,
                                      0.f, 0.49f)) ==
            Catch::Approx(100.f * std::atan(0.02f) / 3.14159265358979323846f));
    begin(translator,
          pointer(GestureEvent::Type::BEGIN, DATA_WHEEL, 0.5f, 0.f));
    REQUIRE(delta(translator, pointer(GestureEvent::Type::UPDATE, DATA_WHEEL,
                                      2.f, 0.5f)) == Catch::Approx(25.f));
}

TEST_CASE("Rotary drags re-anchor after center crossings and invalid samples",
          "[rotary][input]")
{
    HostToClientTranslator translator;
    begin(translator,
          pointer(GestureEvent::Type::BEGIN, DATA_WHEEL, 0.5f, 0.f));
    auto gesture = pointer(GestureEvent::Type::UPDATE, DATA_WHEEL, 0.5f, 0.5f);
    REQUIRE_FALSE(translator.translate(HostInputEvent{gesture}, nullptr));
    gesture.normY = 1.f;
    REQUIRE_FALSE(translator.translate(HostInputEvent{gesture}, nullptr));
    gesture.normX = 0.f;
    gesture.normY = 0.5f;
    REQUIRE(delta(translator, gesture) == Catch::Approx(25.f));
    gesture.normX = std::numeric_limits<float>::quiet_NaN();
    REQUIRE_FALSE(translator.translate(HostInputEvent{gesture}, nullptr));
    gesture.normX = 0.f;
    REQUIRE_FALSE(translator.translate(HostInputEvent{gesture}, nullptr));
    gesture.normX = 0.5f;
    gesture.normY = 0.f;
    REQUIRE(delta(translator, gesture) == Catch::Approx(25.f));
}

TEST_CASE(
    "Rotary contact lifecycle supports repeats, multiple sources and "
    "cancellation",
    "[rotary][input]")
{
    HostToClientTranslator translator;
    auto first = pointer(GestureEvent::Type::BEGIN, DATA_WHEEL, 0.5f, 0.f);
    begin(translator, first);
    first.type = GestureEvent::Type::REPEAT;
    begin(translator, first);
    first.type = GestureEvent::Type::UPDATE;
    first.normX = 1.f;
    first.normY = 0.5f;
    REQUIRE(delta(translator, first) == Catch::Approx(25.f));

    begin(translator,
          pointer(GestureEvent::Type::BEGIN, DATA_WHEEL, 0.5f, 0.f, 1));
    first.normX = 0.5f;
    first.normY = 1.f;
    REQUIRE(delta(translator, first) == Catch::Approx(250.f));
    const auto secondary =
        translator.translate(HostInputEvent{pointer(GestureEvent::Type::UPDATE,
                                                    DATA_WHEEL, 1.f, 0.5f, 1)},
                             nullptr);
    REQUIRE(secondary);
    REQUIRE_FALSE(std::get<ClientHardwareEvent>(secondary->payload).deltaValue);

    begin(translator,
          pointer(GestureEvent::Type::BEGIN, MAIN_VOLUME_POT, 0.5f, 0.f, 1));
    REQUIRE(delta(translator, pointer(GestureEvent::Type::UPDATE,
                                      MAIN_VOLUME_POT, 1.f, 0.5f, 1)) > 0.f);
    first.type = GestureEvent::Type::END;
    translator.translate(HostInputEvent{first}, nullptr);
    first.type = GestureEvent::Type::UPDATE;
    REQUIRE_FALSE(translator.translate(HostInputEvent{first}, nullptr));
    translator.cancelGestures();
    REQUIRE_FALSE(translator.translate(
        HostInputEvent{
            pointer(GestureEvent::Type::UPDATE, MAIN_VOLUME_POT, 0.5f, 1.f, 1)},
        nullptr));
    begin(translator,
          pointer(GestureEvent::Type::BEGIN, DATA_WHEEL, 0.5f, 0.f));
    REQUIRE(delta(translator, pointer(GestureEvent::Type::UPDATE, DATA_WHEEL,
                                      1.f, 0.5f)) == Catch::Approx(25.f));
}
