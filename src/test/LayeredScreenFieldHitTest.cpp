#include <catch2/catch_test_macros.hpp>

#include <limits>

#include "TestMpc.hpp"

#include "lcdgui/Field.hpp"
#include "lcdgui/FunctionKeys.hpp"
#include "lcdgui/Layer.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/ScreenComponent.hpp"

#include "hardware/Component.hpp"
#include "hardware/ComponentId.hpp"
#include "hardware/Hardware.hpp"
#include "input/HostInputEvent.hpp"

using namespace mpc;
using namespace mpc::lcdgui;

namespace
{
    LcdPoint centerOf(const MRECT rect)
    {
        return {static_cast<float>(rect.L + rect.R) * 0.5f,
                static_cast<float>(rect.T + rect.B) * 0.5f};
    }
} // namespace

TEST_CASE("LCD coordinates focus visible fields", "[lcd][focus][hit-test]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("sequencer");
    const auto screen = layeredScreen->getCurrentScreen();
    const auto field = screen->findField("tempo-source");
    REQUIRE(field);

    const auto point = centerOf(field->getRect());
    REQUIRE(layeredScreen->focusFieldAt(point) ==
            FieldFocusResult::FocusChanged);
    CHECK(layeredScreen->getFocusedFieldName() == "tempo-source");
}

TEST_CASE("LCD hit targets support padding without focusing blank space",
          "[lcd][focus][hit-test]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("sequencer");
    const auto screen = layeredScreen->getCurrentScreen();
    const auto field = screen->findField("sq");
    REQUIRE(field);
    const auto rect = field->getRect();

    REQUIRE(layeredScreen->setFocus("tempo-source"));
    const auto y = static_cast<float>(rect.T + rect.B) * 0.5f;
    const LcdPoint paddedPoint{static_cast<float>(rect.L) - 1.f, y};
    CHECK(layeredScreen->focusFieldAt(paddedPoint) ==
          FieldFocusResult::NoTarget);
    CHECK(layeredScreen->getFocusedFieldName() == "tempo-source");

    LcdHitTestOptions padded;
    padded.horizontalPadding = 2.f;
    REQUIRE(layeredScreen->focusFieldAt(paddedPoint, padded) ==
            FieldFocusResult::FocusChanged);
    CHECK(layeredScreen->getFocusedFieldName() == "sq");

    field->setSplit(true);
    REQUIRE(layeredScreen->focusFieldAt(paddedPoint, padded) ==
            FieldFocusResult::AlreadyFocused);
    CHECK(field->isSplit());

    REQUIRE(layeredScreen->setFocus("tempo-source"));
    LcdHitTestOptions minimumTarget;
    minimumTarget.minimumTargetHeight = static_cast<float>(rect.H() + 4);
    REQUIRE(layeredScreen->focusFieldAt(
                {static_cast<float>(rect.L + rect.R) * 0.5f,
                 static_cast<float>(rect.T) - 1.f},
                minimumTarget) == FieldFocusResult::FocusChanged);
    CHECK(layeredScreen->getFocusedFieldName() == "sq");

    REQUIRE(layeredScreen->setFocus("tempo-source"));
    CHECK(layeredScreen->focusFieldAt({247.f, 59.f}, padded) ==
          FieldFocusResult::NoTarget);
    CHECK(layeredScreen->getFocusedFieldName() == "tempo-source");

    padded.horizontalPadding = 100.f;
    CHECK(layeredScreen->focusFieldAt({-0.01f, y}, padded) ==
          FieldFocusResult::NoTarget);
    CHECK(layeredScreen->focusFieldAt({static_cast<float>(LCD_WIDTH), y},
                                      padded) == FieldFocusResult::NoTarget);
}

TEST_CASE("LCD hit testing skips fields that cannot be touched",
          "[lcd][focus][hit-test]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("sequencer");
    const auto screen = layeredScreen->getCurrentScreen();
    const auto field = screen->findField("tempo-source");
    REQUIRE(field);
    const auto point = centerOf(field->getRect());

    REQUIRE(layeredScreen->setFocus("sq"));
    field->setFocusable(false);
    CHECK(layeredScreen->focusFieldAt(point) == FieldFocusResult::NoTarget);
    CHECK(layeredScreen->getFocusedFieldName() == "sq");

    field->setFocusable(true);
    field->Hide(true);
    CHECK(layeredScreen->focusFieldAt(point) == FieldFocusResult::NoTarget);
    CHECK(layeredScreen->getFocusedFieldName() == "sq");
}

TEST_CASE("LCD hit testing targets only the active top layer",
          "[lcd][focus][hit-test]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("sequencer");
    layeredScreen->openScreen("timing-correct");
    REQUIRE(layeredScreen->getFocusedLayerIndex() > 0);

    const auto window = layeredScreen->getCurrentScreen();
    const auto field = window->findField("notevalue");
    REQUIRE(field);
    const auto result = layeredScreen->focusFieldAt(centerOf(field->getRect()));
    CHECK((result == FieldFocusResult::FocusChanged ||
           result == FieldFocusResult::AlreadyFocused));
    CHECK(layeredScreen->getFocusedFieldName() == "notevalue");
}

TEST_CASE(
    "LCD layer hit testing uses half-open bounds and deterministic ranking",
    "[lcd][focus][hit-test]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);

    Layer layer(0);
    const auto screen = std::make_shared<ScreenComponent>(mpc, "hit-test", 0);
    const auto left = std::make_shared<Field>(mpc, "left", 1, 1, 10);
    const auto right = std::make_shared<Field>(mpc, "right", 11, 1, 10);
    screen->addChild(left);
    screen->addChild(right);
    layer.addChild(screen);

    const auto sharedEdge = layer.findFocusableFieldAt({10.f, 4.f}, {});
    REQUIRE(sharedEdge);
    CHECK(sharedEdge->getName() == "right");

    LcdHitTestOptions padded;
    padded.horizontalPadding = 4.f;
    const auto insideLeft = layer.findFocusableFieldAt({9.f, 4.f}, padded);
    REQUIRE(insideLeft);
    CHECK(insideLeft->getName() == "left");

    left->setLocation(0, 0);
    left->setSize(4, 9);
    right->setLocation(8, 0);
    right->setSize(4, 9);
    padded.horizontalPadding = 3.f;
    REQUIRE(layer.setFocus("right"));
    const auto tied = layer.findFocusableFieldAt({6.f, 4.f}, padded);
    REQUIRE(tied);
    CHECK(tied->getName() == "right");

    padded.horizontalPadding = -100.f;
    padded.minimumTargetWidth = std::numeric_limits<float>::quiet_NaN();
    CHECK_FALSE(layer.findFocusableFieldAt({6.f, 4.f}, padded));
}

TEST_CASE("LCD hit testing resolves only rendered function keys",
          "[lcd][function-key][hit-test]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("sequencer");
    const auto screen = layeredScreen->getCurrentScreen();
    const auto functionKey = screen->findChild<FunctionKey>("fk0");
    REQUIRE(functionKey);

    const auto point = centerOf(functionKey->getRect());
    const auto target = layeredScreen->findLcdTargetAt(point);
    REQUIRE(target);
    const auto functionKeyTarget =
        std::get_if<LcdFunctionKeyHitTarget>(&*target);
    REQUIRE(functionKeyTarget);
    CHECK(functionKeyTarget->functionKeyIndex == 0);

    const auto previousFocus = layeredScreen->getFocusedFieldName();
    CHECK(layeredScreen->focusFieldAt(point) == FieldFocusResult::NoTarget);
    CHECK(layeredScreen->getFocusedFieldName() == previousFocus);

    const auto functionKeys = screen->findChild<FunctionKeys>("function-keys");
    REQUIRE(functionKeys);
    functionKeys->setActiveArrangement(1);
    CHECK_FALSE(layeredScreen->findLcdTargetAt(point));
    functionKeys->setActiveArrangement(0);
    const auto restoredTarget = layeredScreen->findLcdTargetAt(point);
    REQUIRE(restoredTarget);
    CHECK(std::get<LcdFunctionKeyHitTarget>(*restoredTarget).functionKeyIndex ==
          0);
}

TEST_CASE("Resolved LCD function keys use the hardware press lifecycle",
          "[lcd][function-key][input]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("sequencer");
    const auto functionKey =
        layeredScreen->getCurrentScreen()->findChild<FunctionKey>("fk0");
    REQUIRE(functionKey);
    const auto target =
        layeredScreen->findLcdTargetAt(centerOf(functionKey->getRect()));
    REQUIRE(target);
    const auto functionKeyTarget =
        std::get_if<LcdFunctionKeyHitTarget>(&*target);
    REQUIRE(functionKeyTarget);

    const auto componentId = static_cast<hardware::ComponentId>(
        static_cast<int>(hardware::ComponentId::F1) +
        functionKeyTarget->functionKeyIndex);
    const auto gesture = [&](const input::GestureEvent::Type type)
    {
        return input::HostInputEvent(input::GestureEvent{
            type, input::GestureEvent::Movement::NoMovement, 0.f, 0.f, 0.f, 0,
            0, componentId, false, false, false,
            input::GestureEvent::InputDeviceType::Touch});
    };

    mpc.dispatchHostInput(gesture(input::GestureEvent::Type::BEGIN));
    CHECK(mpc.getHardware()->getButton(componentId)->isPressed());
    mpc.dispatchHostInput(gesture(input::GestureEvent::Type::END));
    CHECK_FALSE(mpc.getHardware()->getButton(componentId)->isPressed());
}

TEST_CASE("Native LCD controls outrank enlarged neighbouring touch targets",
          "[lcd][interaction][hit-test]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);

    Layer layer(0);
    const auto screen = std::make_shared<ScreenComponent>(mpc, "hit-test", 0);
    const auto field = std::make_shared<Field>(mpc, "field", 3, 44, 20);
    const auto functionKey = std::make_shared<FunctionKey>(mpc, "fk0", 0, 2);
    functionKey->setType(1);
    screen->addChild(field);
    screen->addChild(functionKey);
    layer.addChild(screen);

    LcdHitTestOptions touchTarget;
    touchTarget.minimumTargetHeight = 30.f;

    const auto fieldTarget =
        layer.findLcdTargetAt(centerOf(field->getRect()), touchTarget);
    REQUIRE(fieldTarget);
    const auto fieldHit = std::get_if<LcdFieldHitTarget>(&*fieldTarget);
    REQUIRE(fieldHit);
    CHECK(fieldHit->fieldName == "field");

    const auto functionTarget =
        layer.findLcdTargetAt(centerOf(functionKey->getRect()), touchTarget);
    REQUIRE(functionTarget);
    const auto functionKeyHit =
        std::get_if<LcdFunctionKeyHitTarget>(&*functionTarget);
    REQUIRE(functionKeyHit);
    CHECK(functionKeyHit->functionKeyIndex == 0);
}
