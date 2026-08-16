#include "Layer.hpp"
#include "Field.hpp"

#include "FunctionKeys.hpp"
#include "ScreenComponent.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

using namespace mpc::lcdgui;

Layer::Layer(const int index) : Component("layer " + std::to_string(index)) {}

Background *Layer::getBackground()
{
    return findChild<Background>("").get();
}

bool Layer::setFocus(const std::string &fieldName)
{
    const auto newFocus = findField(fieldName);

    if (!newFocus || !newFocus->isFocusable() ||
        (newFocus->IsHidden() && !newFocus->isFocusableWhenHidden()))
    {
        return false;
    }

    for (const auto &f : findFields())
    {
        f->loseFocus(fieldName);
    }

    focus = fieldName;

    newFocus->takeFocus();
    const auto screen = findChild<ScreenComponent>();
    screen->bringToFront(newFocus.get());

    if (const auto footerLabel = screen->findChild("footer-label");
        footerLabel && !footerLabel->IsHidden())
    {
        screen->bringToFront(footerLabel.get());
    }
    return true;
}

std::string Layer::getFocus() const
{
    return focus;
}

std::shared_ptr<Field>
Layer::findFocusableFieldAt(const LcdPoint point,
                            const LcdHitTestOptions &options) const
{
    const auto target = findLcdTargetAt(point, options);
    if (!target)
    {
        return {};
    }
    const auto fieldTarget = std::get_if<LcdFieldHitTarget>(&*target);
    return fieldTarget ? findField(fieldTarget->fieldName)
                       : std::shared_ptr<Field>{};
}

std::optional<LcdHitTarget>
Layer::findLcdTargetAt(const LcdPoint point,
                       const LcdHitTestOptions &options) const
{
    if (!std::isfinite(point.x) || !std::isfinite(point.y) || point.x < 0.f ||
        point.x >= static_cast<float>(LCD_WIDTH) || point.y < 0.f ||
        point.y >= static_cast<float>(LCD_HEIGHT))
    {
        return std::nullopt;
    }

    const auto nonNegative = [](const float value)
    {
        return std::isfinite(value) ? std::max(0.f, value) : 0.f;
    };
    const auto horizontalPadding = nonNegative(options.horizontalPadding);
    const auto verticalPadding = nonNegative(options.verticalPadding);
    const auto minimumTargetWidth = nonNegative(options.minimumTargetWidth);
    const auto minimumTargetHeight = nonNegative(options.minimumTargetHeight);

    std::optional<LcdHitTarget> bestTarget;
    auto bestIsInside = false;
    auto bestDistance = std::numeric_limits<float>::infinity();
    auto bestCenterDistance = std::numeric_limits<float>::infinity();

    const auto consider = [&](const LcdHitTarget &target, const MRECT rect,
                              const bool isCurrentFocus)
    {
        const auto width = static_cast<float>(rect.W());
        const auto height = static_cast<float>(rect.H());
        if (width <= 0.f || height <= 0.f)
        {
            return;
        }

        const auto left = static_cast<float>(rect.L);
        const auto right = static_cast<float>(rect.R);
        const auto top = static_cast<float>(rect.T);
        const auto bottom = static_cast<float>(rect.B);
        const auto centerX = left + width * 0.5f;
        const auto centerY = top + height * 0.5f;
        const auto targetWidth =
            std::max(width + horizontalPadding * 2.f, minimumTargetWidth);
        const auto targetHeight =
            std::max(height + verticalPadding * 2.f, minimumTargetHeight);
        const auto targetLeft = centerX - targetWidth * 0.5f;
        const auto targetRight = centerX + targetWidth * 0.5f;
        const auto targetTop = centerY - targetHeight * 0.5f;
        const auto targetBottom = centerY + targetHeight * 0.5f;
        if (point.x < targetLeft || point.x >= targetRight ||
            point.y < targetTop || point.y >= targetBottom)
        {
            return;
        }

        const auto isInside = point.x >= left && point.x < right &&
                              point.y >= top && point.y < bottom;
        const auto closestX = std::clamp(point.x, left, right);
        const auto closestY = std::clamp(point.y, top, bottom);
        const auto distanceX = point.x - closestX;
        const auto distanceY = point.y - closestY;
        const auto distance = distanceX * distanceX + distanceY * distanceY;
        const auto centerDistanceX = point.x - centerX;
        const auto centerDistanceY = point.y - centerY;
        const auto centerDistance = centerDistanceX * centerDistanceX +
                                    centerDistanceY * centerDistanceY;
        const auto bestFieldTarget =
            bestTarget ? std::get_if<LcdFieldHitTarget>(&*bestTarget) : nullptr;
        const auto bestIsCurrentFocus =
            bestFieldTarget && bestFieldTarget->fieldName == focus;

        const auto isBetter = !bestTarget || (isInside && !bestIsInside) ||
                              (isInside == bestIsInside &&
                               (distance < bestDistance ||
                                (distance == bestDistance &&
                                 (centerDistance < bestCenterDistance ||
                                  (centerDistance == bestCenterDistance &&
                                   isCurrentFocus && !bestIsCurrentFocus)))));
        if (isBetter)
        {
            bestTarget = target;
            bestIsInside = isInside;
            bestDistance = distance;
            bestCenterDistance = centerDistance;
        }
    };

    for (const auto &field : findFields())
    {
        if (field->IsHidden() || !field->isFocusable())
        {
            continue;
        }
        consider(LcdFieldHitTarget{field->getName()}, field->getRect(),
                 field->getName() == focus);
    }

    for (const auto &functionKey : findFunctionKeys())
    {
        if (functionKey->IsHidden() || functionKey->type < 0)
        {
            continue;
        }
        consider(LcdFunctionKeyHitTarget{functionKey->getIndex()},
                 functionKey->getRect(), false);
    }

    return bestTarget;
}
