#pragma once

#include "input/HostInputEvent.hpp"

#include <cmath>
#include <map>
#include <optional>
#include <utility>

namespace mpc::input
{
    // Converts captured rotary-control drags into existing relative input
    // units. The mode selected on contact remains fixed until END.
    class RotaryGestureHandler
    {
        struct Drag
        {
            bool radial;
            std::optional<float> angle;
        };

        std::map<std::pair<hardware::ComponentId, int>, Drag> drags;

    public:
        void cancelAll()
        {
            drags.clear();
        }

        std::optional<GestureEvent> handle(GestureEvent gesture,
                                           RotaryDragMode mode)
        {
            using namespace hardware;
            if (gesture.movement != GestureEvent::Movement::RotaryDrag)
            {
                return gesture;
            }
            if (gesture.componentId != DATA_WHEEL &&
                gesture.componentId != REC_GAIN_POT &&
                gesture.componentId != MAIN_VOLUME_POT)
            {
                return std::nullopt;
            }

            const auto key =
                std::make_pair(gesture.componentId, gesture.sourceIndex);
            if (gesture.type == GestureEvent::Type::END)
            {
                drags.erase(key);
                return gesture;
            }
            if (!std::isfinite(gesture.normX) ||
                !std::isfinite(gesture.normY) ||
                !std::isfinite(gesture.continuousDelta))
            {
                // Re-anchor after invalid samples instead of accumulating a
                // jump.
                if (auto it = drags.find(key); it != drags.end())
                {
                    it->second.angle.reset();
                }
                return std::nullopt;
            }

            const float x = gesture.normX - 0.5f;
            const float y = gesture.normY - 0.5f;
            const float radius = std::hypot(x, y);
            const float angle = std::atan2(y, x);
            if (gesture.type == GestureEvent::Type::BEGIN ||
                gesture.type == GestureEvent::Type::REPEAT)
            {
                // Outer half of the radius includes the DATA wheel's dimple.
                const bool radial =
                    mode == RotaryDragMode::Circular ||
                    (mode == RotaryDragMode::ByPosition && radius >= 0.25f);
                drags[key] = {radial, radius >= 0.1f
                                          ? std::optional<float>{angle}
                                          : std::nullopt};
                gesture.type = GestureEvent::Type::BEGIN;
                return gesture;
            }

            const auto it = drags.find(key);
            if (it == drags.end() || gesture.type != GestureEvent::Type::UPDATE)
            {
                return std::nullopt;
            }

            auto &drag = it->second;
            if (drag.radial)
            {
                // Angles near the center are unstable. Re-anchor upon leaving
                // this dead zone so crossing the center cannot cause a
                // half-turn.
                if (radius < 0.1f)
                {
                    drag.angle.reset();
                    return std::nullopt;
                }
                const auto previousAngle = drag.angle;
                drag.angle = angle;
                if (!previousAngle)
                {
                    return std::nullopt;
                }

                constexpr float pi = 3.14159265358979323846f;
                const float radians =
                    std::remainder(angle - *previousAngle, 2.f * pi);
                if (gesture.componentId == DATA_WHEEL)
                {
                    // 100 model steps per revolution. Compensate for linear
                    // sensitivity; modifier/multitouch acceleration still
                    // applies.
                    const float base =
                        gesture.inputDeviceType ==
                                GestureEvent::InputDeviceType::Mouse
                            ? 0.3f
                            : 0.1f;
                    gesture.continuousDelta =
                        radians * 100.f / (2.f * pi * base);
                }
                else
                {
                    // Pots have a 306-degree visual sweep; each relative unit
                    // changes the model by 0.01.
                    gesture.continuousDelta = radians * 100.f / (1.7f * pi);
                }
            }
            gesture.movement = GestureEvent::Movement::Relative;
            return gesture;
        }
    };
} // namespace mpc::input
