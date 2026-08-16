#include "input/LcdGestureHandler.hpp"

#include "lcdgui/LcdGeometry.hpp"
#include "lcdgui/LayeredScreen.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

namespace mpc::input
{
    namespace
    {
        constexpr float mouseAndPenHitPadding = 1.f;
        constexpr float touchMinimumTargetSize = 9.f;
        constexpr float mouseAndPenDragSlop = 1.f;
        constexpr float touchDragSlop = 2.f;
        constexpr float wheelDeltaPerLcdHeight = 100.f;
    } // namespace

    LcdGestureHandler::LcdGestureHandler(
        std::shared_ptr<lcdgui::LayeredScreen> layeredScreenToUse)
        : layeredScreen(std::move(layeredScreenToUse))
    {
    }

    LcdGestureHandler::Result
    LcdGestureHandler::handle(const GestureEvent &gesture)
    {
        Result result;
        if (gesture.componentId != hardware::ComponentId::LCD ||
            gesture.type == GestureEvent::Type::REPEAT ||
            !std::isfinite(gesture.normX) || !std::isfinite(gesture.normY))
        {
            return result;
        }

        if (gesture.movement == GestureEvent::Movement::Relative)
        {
            if (gesture.type != GestureEvent::Type::UPDATE ||
                !std::isfinite(gesture.continuousDelta) ||
                gesture.continuousDelta == 0.f || gesture.normX < 0.f ||
                gesture.normX > 1.f || gesture.normY < 0.f ||
                gesture.normY > 1.f)
            {
                return result;
            }

            const lcdgui::LcdPoint point{
                gesture.normX * static_cast<float>(lcdgui::LCD_WIDTH),
                gesture.normY * static_cast<float>(lcdgui::LCD_HEIGHT)};
            const auto target = layeredScreen->findLcdTargetAt(
                point, hitTestOptionsFor(gesture.inputDeviceType));
            if (!target ||
                !std::holds_alternative<lcdgui::LcdFieldHitTarget>(*target) ||
                layeredScreen->focusField(*target) ==
                    lcdgui::FieldFocusResult::NoTarget)
            {
                return result;
            }

            result.derivedGestures.push_back(deriveGesture(
                gesture, GestureEvent::Type::UPDATE,
                hardware::ComponentId::DATA_WHEEL,
                GestureEvent::Movement::Relative, gesture.continuousDelta));
            result.inputResult = HostInputResult::Handled;
            return result;
        }

        if (gesture.movement != GestureEvent::Movement::Absolute)
        {
            return result;
        }

        auto normalizedGesture = gesture;
        normalizedGesture.normX = std::clamp(normalizedGesture.normX, 0.f, 1.f);
        normalizedGesture.normY = std::clamp(normalizedGesture.normY, 0.f, 1.f);

        if (gesture.type == GestureEvent::Type::BEGIN)
        {
            if (gesture.normX < 0.f || gesture.normX > 1.f ||
                gesture.normY < 0.f || gesture.normY > 1.f)
            {
                return result;
            }

            if (const auto existing =
                    activeInteractions.find(gesture.sourceIndex);
                existing != activeInteractions.end())
            {
                endInteraction(existing->second, result.derivedGestures);
                activeInteractions.erase(existing);
            }

            const lcdgui::LcdPoint point{
                normalizedGesture.normX * static_cast<float>(lcdgui::LCD_WIDTH),
                normalizedGesture.normY *
                    static_cast<float>(lcdgui::LCD_HEIGHT)};
            const auto target = layeredScreen->findLcdTargetAt(
                point, hitTestOptionsFor(gesture.inputDeviceType));
            if (!target)
            {
                return result;
            }

            ActiveInteraction interaction{*target,
                                          normalizedGesture,
                                          normalizedGesture.normX,
                                          normalizedGesture.normY,
                                          normalizedGesture.normY,
                                          layeredScreen->getFocusedLayerIndex(),
                                          layeredScreen->getCurrentScreen()};

            if (std::holds_alternative<lcdgui::LcdFieldHitTarget>(*target))
            {
                if (layeredScreen->focusField(*target) ==
                    lcdgui::FieldFocusResult::NoTarget)
                {
                    return result;
                }
                interaction.layerIndex = layeredScreen->getFocusedLayerIndex();
                interaction.screen = layeredScreen->getCurrentScreen();
            }
            else
            {
                const auto &functionKeyTarget =
                    std::get<lcdgui::LcdFunctionKeyHitTarget>(*target);
                const auto componentId =
                    functionKeyComponentId(functionKeyTarget.functionKeyIndex);
                if (!componentId)
                {
                    return result;
                }
                result.derivedGestures.push_back(deriveGesture(
                    normalizedGesture, GestureEvent::Type::BEGIN, *componentId,
                    GestureEvent::Movement::NoMovement));
            }

            activeInteractions.insert_or_assign(gesture.sourceIndex,
                                                std::move(interaction));
            result.inputResult = HostInputResult::Handled;
            return result;
        }

        const auto found = activeInteractions.find(gesture.sourceIndex);
        if (found == activeInteractions.end())
        {
            return result;
        }

        result.inputResult = HostInputResult::Handled;
        auto &interaction = found->second;
        interaction.lastGesture = normalizedGesture;

        if (gesture.type == GestureEvent::Type::END)
        {
            endInteraction(interaction, result.derivedGestures);
            activeInteractions.erase(found);
            return result;
        }

        if (gesture.type != GestureEvent::Type::UPDATE ||
            std::holds_alternative<lcdgui::LcdFunctionKeyHitTarget>(
                interaction.target))
        {
            return result;
        }

        if (!fieldCaptureIsCurrent(interaction))
        {
            endInteraction(interaction, result.derivedGestures);
            activeInteractions.erase(found);
            return result;
        }

        const auto totalVerticalDelta =
            (interaction.startNormY - normalizedGesture.normY) *
            static_cast<float>(lcdgui::LCD_HEIGHT);
        const auto totalHorizontalDelta =
            (interaction.startNormX - normalizedGesture.normX) *
            static_cast<float>(lcdgui::LCD_WIDTH);

        if (!interaction.wheelGestureStarted)
        {
            const auto threshold = interaction.lastGesture.inputDeviceType ==
                                           GestureEvent::InputDeviceType::Touch
                                       ? touchDragSlop
                                       : mouseAndPenDragSlop;
            if (std::abs(totalVerticalDelta) < threshold ||
                std::abs(totalVerticalDelta) < std::abs(totalHorizontalDelta))
            {
                return result;
            }

            interaction.wheelGestureStarted = true;
            result.derivedGestures.push_back(
                deriveGesture(normalizedGesture, GestureEvent::Type::BEGIN,
                              hardware::ComponentId::DATA_WHEEL,
                              GestureEvent::Movement::NoMovement));
        }

        const auto normalizedDelta =
            interaction.previousNormY == interaction.startNormY
                ? interaction.startNormY - normalizedGesture.normY
                : interaction.previousNormY - normalizedGesture.normY;
        interaction.previousNormY = normalizedGesture.normY;
        if (normalizedDelta != 0.f)
        {
            result.derivedGestures.push_back(
                deriveGesture(normalizedGesture, GestureEvent::Type::UPDATE,
                              hardware::ComponentId::DATA_WHEEL,
                              GestureEvent::Movement::Relative,
                              normalizedDelta * wheelDeltaPerLcdHeight));
        }
        return result;
    }

    std::vector<GestureEvent> LcdGestureHandler::cancelAll()
    {
        std::vector<GestureEvent> result;
        for (const auto &[sourceIndex, interaction] : activeInteractions)
        {
            static_cast<void>(sourceIndex);
            endInteraction(interaction, result);
        }
        activeInteractions.clear();
        return result;
    }

    lcdgui::LcdHitTestOptions LcdGestureHandler::hitTestOptionsFor(
        const GestureEvent::InputDeviceType deviceType)
    {
        lcdgui::LcdHitTestOptions result;
        if (deviceType == GestureEvent::InputDeviceType::Touch)
        {
            result.minimumTargetWidth = touchMinimumTargetSize;
            result.minimumTargetHeight = touchMinimumTargetSize;
        }
        else
        {
            result.horizontalPadding = mouseAndPenHitPadding;
            result.verticalPadding = mouseAndPenHitPadding;
        }
        return result;
    }

    std::optional<hardware::ComponentId>
    LcdGestureHandler::functionKeyComponentId(const int functionKeyIndex)
    {
        if (functionKeyIndex < 0 || functionKeyIndex >= 6)
        {
            return std::nullopt;
        }
        return static_cast<hardware::ComponentId>(
            static_cast<int>(hardware::ComponentId::F1) + functionKeyIndex);
    }

    GestureEvent LcdGestureHandler::deriveGesture(
        const GestureEvent &source, const GestureEvent::Type type,
        const hardware::ComponentId componentId,
        const GestureEvent::Movement movement, const float continuousDelta)
    {
        return {type,
                movement,
                source.normX,
                source.normY,
                continuousDelta,
                0,
                source.sourceIndex,
                componentId,
                source.shiftDown,
                source.ctrlDown,
                source.altDown,
                source.inputDeviceType};
    }

    bool LcdGestureHandler::fieldCaptureIsCurrent(
        const ActiveInteraction &interaction) const
    {
        const auto fieldTarget =
            std::get_if<lcdgui::LcdFieldHitTarget>(&interaction.target);
        return fieldTarget &&
               layeredScreen->getFocusedLayerIndex() ==
                   interaction.layerIndex &&
               layeredScreen->getCurrentScreen() == interaction.screen.lock() &&
               layeredScreen->getFocusedFieldName() == fieldTarget->fieldName;
    }

    void LcdGestureHandler::endInteraction(
        const ActiveInteraction &interaction,
        std::vector<GestureEvent> &derivedGestures)
    {
        if (const auto functionKeyTarget =
                std::get_if<lcdgui::LcdFunctionKeyHitTarget>(
                    &interaction.target);
            functionKeyTarget)
        {
            if (const auto componentId =
                    functionKeyComponentId(functionKeyTarget->functionKeyIndex);
                componentId)
            {
                derivedGestures.push_back(deriveGesture(
                    interaction.lastGesture, GestureEvent::Type::END,
                    *componentId, GestureEvent::Movement::NoMovement));
            }
        }
        else if (interaction.wheelGestureStarted)
        {
            derivedGestures.push_back(
                deriveGesture(interaction.lastGesture, GestureEvent::Type::END,
                              hardware::ComponentId::DATA_WHEEL,
                              GestureEvent::Movement::NoMovement));
        }
    }
} // namespace mpc::input
