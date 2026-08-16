#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"

#include "hardware/Component.hpp"
#include "hardware/Hardware.hpp"
#include "input/LcdGestureHandler.hpp"
#include "lcdgui/Field.hpp"
#include "lcdgui/FunctionKeys.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/ScreenComponent.hpp"

#include <limits>

using namespace mpc;
using namespace mpc::input;
using namespace mpc::lcdgui;

namespace
{
    GestureEvent lcdGesture(const GestureEvent::Type type, const float normX,
                            const float normY, const int sourceIndex = 0,
                            const GestureEvent::InputDeviceType deviceType =
                                GestureEvent::InputDeviceType::Mouse)
    {
        return {type,        GestureEvent::Movement::Absolute,
                normX,       normY,
                0.f,         0,
                sourceIndex, hardware::ComponentId::LCD,
                false,       false,
                false,       deviceType};
    }

    GestureEvent lcdWheelGesture(const float normX, const float normY,
                                 const float continuousDelta,
                                 const int sourceIndex = 0)
    {
        return {GestureEvent::Type::UPDATE,
                GestureEvent::Movement::Relative,
                normX,
                normY,
                continuousDelta,
                0,
                sourceIndex,
                hardware::ComponentId::LCD,
                false,
                false,
                false,
                GestureEvent::InputDeviceType::Mouse};
    }

    std::pair<float, float> normalizedCenter(const MRECT rect)
    {
        return {static_cast<float>(rect.L + rect.R) * 0.5f /
                    static_cast<float>(LCD_WIDTH),
                static_cast<float>(rect.T + rect.B) * 0.5f /
                    static_cast<float>(LCD_HEIGHT)};
    }
} // namespace

TEST_CASE("Core LCD gestures derive the rendered function-key lifecycle",
          "[lcd][input][function-key]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("sequencer");
    const auto functionKey =
        layeredScreen->getCurrentScreen()->findChild<FunctionKey>("fk0");
    REQUIRE(functionKey);
    const auto [x, y] = normalizedCenter(functionKey->getRect());

    LcdGestureHandler handler(layeredScreen);
    const auto begin =
        handler.handle(lcdGesture(GestureEvent::Type::BEGIN, x, y, 3,
                                  GestureEvent::InputDeviceType::Touch));
    CHECK(begin.inputResult == HostInputResult::Handled);
    REQUIRE(begin.derivedGestures.size() == 1);
    CHECK(begin.derivedGestures[0].type == GestureEvent::Type::BEGIN);
    CHECK(begin.derivedGestures[0].componentId == hardware::ComponentId::F1);
    CHECK(begin.derivedGestures[0].sourceIndex == 3);

    layeredScreen->openScreen("timing-correct");
    const auto end =
        handler.handle(lcdGesture(GestureEvent::Type::END, 1.25f, -0.25f, 3));
    CHECK(end.inputResult == HostInputResult::Handled);
    REQUIRE(end.derivedGestures.size() == 1);
    CHECK(end.derivedGestures[0].type == GestureEvent::Type::END);
    CHECK(end.derivedGestures[0].componentId == hardware::ComponentId::F1);
}

TEST_CASE("Core LCD field drags derive component-relative wheel gestures",
          "[lcd][input][field-drag]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("sequencer");
    const auto field =
        layeredScreen->getCurrentScreen()->findField("tempo-source");
    REQUIRE(field);
    const auto [x, y] = normalizedCenter(field->getRect());

    LcdGestureHandler handler(layeredScreen);
    const auto begin =
        handler.handle(lcdGesture(GestureEvent::Type::BEGIN, x, y));
    CHECK(begin.inputResult == HostInputResult::Handled);
    CHECK(begin.derivedGestures.empty());
    CHECK(layeredScreen->getFocusedFieldName() == "tempo-source");

    const auto belowSlop = handler.handle(
        lcdGesture(GestureEvent::Type::UPDATE, x, y - 0.5f / LCD_HEIGHT));
    CHECK(belowSlop.derivedGestures.empty());

    const auto horizontal = handler.handle(lcdGesture(
        GestureEvent::Type::UPDATE, x + 2.f / LCD_WIDTH, y - 1.f / LCD_HEIGHT));
    CHECK(horizontal.derivedGestures.empty());

    const auto upward = handler.handle(
        lcdGesture(GestureEvent::Type::UPDATE, x, y - 2.f / LCD_HEIGHT));
    REQUIRE(upward.derivedGestures.size() == 2);
    CHECK(upward.derivedGestures[0].type == GestureEvent::Type::BEGIN);
    CHECK(upward.derivedGestures[0].componentId ==
          hardware::ComponentId::DATA_WHEEL);
    CHECK(upward.derivedGestures[1].type == GestureEvent::Type::UPDATE);
    CHECK(upward.derivedGestures[1].continuousDelta ==
          Catch::Approx(100.f * 2.f / LCD_HEIGHT));

    const auto downward = handler.handle(
        lcdGesture(GestureEvent::Type::UPDATE, x, y + 1.f / LCD_HEIGHT));
    REQUIRE(downward.derivedGestures.size() == 1);
    CHECK(downward.derivedGestures[0].continuousDelta ==
          Catch::Approx(-100.f * 3.f / LCD_HEIGHT));

    const auto end = handler.handle(lcdGesture(GestureEvent::Type::END, x, y));
    REQUIRE(end.derivedGestures.size() == 1);
    CHECK(end.derivedGestures[0].type == GestureEvent::Type::END);
    CHECK(end.derivedGestures[0].componentId ==
          hardware::ComponentId::DATA_WHEEL);
}

TEST_CASE("Core LCD field drags continue and reverse beyond display bounds",
          "[lcd][input][field-drag]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("sequencer");
    const auto field =
        layeredScreen->getCurrentScreen()->findField("tempo-source");
    REQUIRE(field);
    const auto [x, y] = normalizedCenter(field->getRect());

    LcdGestureHandler handler(layeredScreen);
    REQUIRE(handler.handle(lcdGesture(GestureEvent::Type::BEGIN, x, y))
                .inputResult == HostInputResult::Handled);

    const auto firstY = y - 2.f / LCD_HEIGHT;
    const auto startDrag =
        handler.handle(lcdGesture(GestureEvent::Type::UPDATE, x, firstY));
    REQUIRE(startDrag.derivedGestures.size() == 2);

    constexpr float outsideY = -0.25f;
    const auto outside =
        handler.handle(lcdGesture(GestureEvent::Type::UPDATE, x, outsideY));
    REQUIRE(outside.derivedGestures.size() == 1);
    CHECK(outside.derivedGestures[0].continuousDelta ==
          Catch::Approx((firstY - outsideY) * 100.f));

    constexpr float fartherOutsideY = -0.5f;
    const auto fartherOutside = handler.handle(
        lcdGesture(GestureEvent::Type::UPDATE, x, fartherOutsideY));
    REQUIRE(fartherOutside.derivedGestures.size() == 1);
    CHECK(fartherOutside.derivedGestures[0].continuousDelta ==
          Catch::Approx(25.f));

    constexpr float reversedOutsideY = -0.4f;
    const auto reversed = handler.handle(
        lcdGesture(GestureEvent::Type::UPDATE, x, reversedOutsideY));
    REQUIRE(reversed.derivedGestures.size() == 1);
    CHECK(reversed.derivedGestures[0].continuousDelta == Catch::Approx(-10.f));

    const auto end =
        handler.handle(lcdGesture(GestureEvent::Type::END, x, 1.5f));
    CHECK(end.inputResult == HostInputResult::Handled);
    REQUIRE(end.derivedGestures.size() == 1);
    CHECK(end.derivedGestures[0].type == GestureEvent::Type::END);
    CHECK(end.derivedGestures[0].componentId ==
          hardware::ComponentId::DATA_WHEEL);
}

TEST_CASE("Core LCD gestures apply touch slop and cancel stale fields",
          "[lcd][input][field-drag]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("sequencer");
    const auto field =
        layeredScreen->getCurrentScreen()->findField("tempo-source");
    REQUIRE(field);
    const auto [x, y] = normalizedCenter(field->getRect());

    LcdGestureHandler handler(layeredScreen);
    REQUIRE(handler
                .handle(lcdGesture(GestureEvent::Type::BEGIN, x, y, 0,
                                   GestureEvent::InputDeviceType::Touch))
                .inputResult == HostInputResult::Handled);
    CHECK(handler
              .handle(lcdGesture(GestureEvent::Type::UPDATE, x,
                                 y - 1.5f / LCD_HEIGHT, 0,
                                 GestureEvent::InputDeviceType::Touch))
              .derivedGestures.empty());

    REQUIRE(layeredScreen->setFocus("sq"));
    const auto stale = handler.handle(
        lcdGesture(GestureEvent::Type::UPDATE, x, y - 3.f / LCD_HEIGHT, 0,
                   GestureEvent::InputDeviceType::Touch));
    CHECK(stale.inputResult == HostInputResult::Handled);
    CHECK(stale.derivedGestures.empty());
    CHECK(
        handler.handle(lcdGesture(GestureEvent::Type::END, x, y)).inputResult ==
        HostInputResult::Ignored);
}

TEST_CASE("Core LCD wheel input focuses hovered fields and derives data wheel",
          "[lcd][input][wheel]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("sequencer");
    const auto field =
        layeredScreen->getCurrentScreen()->findField("tempo-source");
    REQUIRE(field);
    const auto [x, y] = normalizedCenter(field->getRect());

    LcdGestureHandler handler(layeredScreen);
    auto wheel = lcdWheelGesture(x, y, 12.5f, 6);
    wheel.altDown = true;
    const auto result = handler.handle(wheel);
    CHECK(result.inputResult == HostInputResult::Handled);
    CHECK(layeredScreen->getFocusedFieldName() == "tempo-source");
    REQUIRE(result.derivedGestures.size() == 1);
    CHECK(result.derivedGestures[0].type == GestureEvent::Type::UPDATE);
    CHECK(result.derivedGestures[0].movement ==
          GestureEvent::Movement::Relative);
    CHECK(result.derivedGestures[0].componentId ==
          hardware::ComponentId::DATA_WHEEL);
    CHECK(result.derivedGestures[0].continuousDelta == Catch::Approx(12.5f));
    CHECK(result.derivedGestures[0].sourceIndex == 6);
    CHECK(result.derivedGestures[0].altDown);

    REQUIRE(handler.handle(lcdGesture(GestureEvent::Type::BEGIN, x, y, 6))
                .inputResult == HostInputResult::Handled);
    CHECK(handler.handle(lcdWheelGesture(x, y, 2.f, 6)).inputResult ==
          HostInputResult::Handled);
    const auto continuedDrag = handler.handle(
        lcdGesture(GestureEvent::Type::UPDATE, x, y - 2.f / LCD_HEIGHT, 6));
    REQUIRE(continuedDrag.derivedGestures.size() == 2);
    CHECK(continuedDrag.derivedGestures[0].type == GestureEvent::Type::BEGIN);
}

TEST_CASE("Core LCD labels support field drags and wheel input",
          "[lcd][input][parameter]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("sequencer");
    const auto label = layeredScreen->getCurrentScreen()->findLabel("sq");
    REQUIRE(label);
    const auto [x, y] = normalizedCenter(label->getRect());

    LcdGestureHandler handler(layeredScreen);
    const auto wheel = handler.handle(lcdWheelGesture(x, y, 3.f));
    CHECK(wheel.inputResult == HostInputResult::Handled);
    CHECK(layeredScreen->getFocusedFieldName() == "sq");
    REQUIRE(wheel.derivedGestures.size() == 1);
    CHECK(wheel.derivedGestures[0].componentId ==
          hardware::ComponentId::DATA_WHEEL);

    REQUIRE(handler.handle(lcdGesture(GestureEvent::Type::BEGIN, x, y))
                .inputResult == HostInputResult::Handled);
    const auto drag = handler.handle(
        lcdGesture(GestureEvent::Type::UPDATE, x, y - 2.f / LCD_HEIGHT));
    REQUIRE(drag.derivedGestures.size() == 2);
    CHECK(drag.derivedGestures[0].type == GestureEvent::Type::BEGIN);
    CHECK(drag.derivedGestures[0].componentId ==
          hardware::ComponentId::DATA_WHEEL);
    CHECK(drag.derivedGestures[1].type == GestureEvent::Type::UPDATE);
}

TEST_CASE("Core LCD wheel input ignores non-field and invalid targets",
          "[lcd][input][wheel]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("sequencer");
    const auto functionKey =
        layeredScreen->getCurrentScreen()->findChild<FunctionKey>("fk0");
    REQUIRE(functionKey);
    const auto [functionX, functionY] =
        normalizedCenter(functionKey->getRect());

    LcdGestureHandler handler(layeredScreen);
    CHECK(handler.handle(lcdWheelGesture(functionX, functionY, 1.f))
              .inputResult == HostInputResult::Ignored);
    CHECK(handler.handle(lcdWheelGesture(1.f, 1.f, 1.f)).inputResult ==
          HostInputResult::Ignored);
    CHECK(handler.handle(lcdWheelGesture(-0.1f, 0.5f, 1.f)).inputResult ==
          HostInputResult::Ignored);
    CHECK(handler.handle(lcdWheelGesture(0.5f, 0.5f, 0.f)).inputResult ==
          HostInputResult::Ignored);
    CHECK(handler
              .handle(lcdWheelGesture(0.5f, 0.5f,
                                      std::numeric_limits<float>::quiet_NaN()))
              .inputResult == HostInputResult::Ignored);

    auto wrongType = lcdWheelGesture(0.5f, 0.5f, 1.f);
    wrongType.type = GestureEvent::Type::BEGIN;
    CHECK(handler.handle(wrongType).inputResult == HostInputResult::Ignored);
}

TEST_CASE("Core LCD input reports blank and invalid gestures as ignored",
          "[lcd][input]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("sequencer");
    LcdGestureHandler handler(layeredScreen);

    CHECK(handler.handle(lcdGesture(GestureEvent::Type::BEGIN, 1.f, 1.f))
              .inputResult == HostInputResult::Ignored);
    CHECK(handler.handle(lcdGesture(GestureEvent::Type::BEGIN, -0.1f, 0.5f))
              .inputResult == HostInputResult::Ignored);
    CHECK(handler.handle(lcdGesture(GestureEvent::Type::UPDATE, -0.1f, 1.2f))
              .inputResult == HostInputResult::Ignored);
    CHECK(handler
              .handle(lcdGesture(GestureEvent::Type::BEGIN,
                                 std::numeric_limits<float>::quiet_NaN(), 0.5f))
              .inputResult == HostInputResult::Ignored);
    CHECK(handler.handle(lcdGesture(GestureEvent::Type::REPEAT, 0.5f, 0.5f))
              .inputResult == HostInputResult::Ignored);
}

TEST_CASE("Core LCD input tracks and cancels concurrent pointer sources",
          "[lcd][input][multi-touch]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("sequencer");
    const auto functionKey =
        layeredScreen->getCurrentScreen()->findChild<FunctionKey>("fk0");
    REQUIRE(functionKey);
    const auto [x, y] = normalizedCenter(functionKey->getRect());

    LcdGestureHandler handler(layeredScreen);
    auto first = lcdGesture(GestureEvent::Type::BEGIN, x, y, 1);
    first.shiftDown = true;
    const auto firstResult = handler.handle(first);
    REQUIRE(firstResult.derivedGestures.size() == 1);
    CHECK(firstResult.derivedGestures[0].shiftDown);

    const auto secondResult =
        handler.handle(lcdGesture(GestureEvent::Type::BEGIN, x, y, 2));
    REQUIRE(secondResult.derivedGestures.size() == 1);

    const auto cancelled = handler.cancelAll();
    REQUIRE(cancelled.size() == 2);
    CHECK(cancelled[0].type == GestureEvent::Type::END);
    CHECK(cancelled[1].type == GestureEvent::Type::END);
    CHECK((cancelled[0].sourceIndex == 1 || cancelled[0].sourceIndex == 2));
    CHECK((cancelled[1].sourceIndex == 1 || cancelled[1].sourceIndex == 2));
    CHECK(cancelled[0].sourceIndex != cancelled[1].sourceIndex);
}

TEST_CASE("Dispatched LCD gestures use hardware input and focus cancellation",
          "[lcd][input][integration]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("sequencer");
    const auto functionKey =
        layeredScreen->getCurrentScreen()->findChild<FunctionKey>("fk0");
    REQUIRE(functionKey);
    const auto [x, y] = normalizedCenter(functionKey->getRect());

    CHECK(mpc.dispatchHostInput(HostInputEvent(lcdGesture(
              GestureEvent::Type::BEGIN, x, y))) == HostInputResult::Handled);
    CHECK(mpc.getHardware()->getButton(hardware::ComponentId::F1)->isPressed());

    CHECK(mpc.dispatchHostInput(HostInputEvent(
              FocusEvent{FocusEvent::Type::Lost})) == HostInputResult::Handled);
    CHECK_FALSE(
        mpc.getHardware()->getButton(hardware::ComponentId::F1)->isPressed());

    layeredScreen->openScreen("sequencer");
    const auto field =
        layeredScreen->getCurrentScreen()->findField("tempo-source");
    REQUIRE(field);
    const auto [fieldX, fieldY] = normalizedCenter(field->getRect());

    REQUIRE(mpc.dispatchHostInput(HostInputEvent(
                lcdGesture(GestureEvent::Type::BEGIN, fieldX, fieldY, 3))) ==
            HostInputResult::Handled);
    CHECK(mpc.dispatchHostInput(HostInputEvent(
              FocusEvent{FocusEvent::Type::Lost})) == HostInputResult::Handled);
    CHECK(mpc.dispatchHostInput(HostInputEvent(lcdGesture(
              GestureEvent::Type::UPDATE, fieldX, fieldY - 3.f / LCD_HEIGHT,
              3))) == HostInputResult::Ignored);

    const auto previousWheelAngle =
        mpc.getHardware()->getDataWheel()->getAngle();
    REQUIRE(mpc.dispatchHostInput(HostInputEvent(
                lcdGesture(GestureEvent::Type::BEGIN, fieldX, fieldY, 4))) ==
            HostInputResult::Handled);
    REQUIRE(mpc.dispatchHostInput(HostInputEvent(lcdGesture(
                GestureEvent::Type::UPDATE, fieldX, fieldY - 3.f / LCD_HEIGHT,
                4))) == HostInputResult::Handled);
    REQUIRE(mpc.dispatchHostInput(HostInputEvent(
                lcdGesture(GestureEvent::Type::END, fieldX, fieldY, 4))) ==
            HostInputResult::Handled);
    CHECK(mpc.getHardware()->getDataWheel()->getAngle() > previousWheelAngle);

    const auto wheelAngleBeforeScroll =
        mpc.getHardware()->getDataWheel()->getAngle();
    REQUIRE(mpc.dispatchHostInput(HostInputEvent(lcdWheelGesture(
                fieldX, fieldY, 10.f, 5))) == HostInputResult::Handled);
    CHECK(mpc.getHardware()->getDataWheel()->getAngle() >
          wheelAngleBeforeScroll);

    CHECK(mpc.dispatchHostInput(HostInputEvent(
              lcdGesture(GestureEvent::Type::BEGIN, 1.f, 1.f))) ==
          HostInputResult::Ignored);
}
