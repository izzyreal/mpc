#pragma once

#include "input/HostInputEvent.hpp"
#include "lcdgui/LcdInteraction.hpp"

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

namespace mpc::lcdgui
{
    class LayeredScreen;
    class ScreenComponent;
} // namespace mpc::lcdgui

namespace mpc::input
{
    class LcdGestureHandler final
    {
    public:
        struct Result
        {
            HostInputResult inputResult = HostInputResult::Ignored;
            std::vector<GestureEvent> derivedGestures;
        };

        explicit LcdGestureHandler(
            std::shared_ptr<lcdgui::LayeredScreen> layeredScreen);

        Result handle(const GestureEvent &gesture);
        std::vector<GestureEvent> cancelAll();

    private:
        struct ActiveInteraction
        {
            lcdgui::LcdHitTarget target;
            GestureEvent lastGesture;
            float startNormX = 0.f;
            float startNormY = 0.f;
            float previousNormY = 0.f;
            int layerIndex = -1;
            std::weak_ptr<lcdgui::ScreenComponent> screen;
            bool wheelGestureStarted = false;
        };

        std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::unordered_map<int, ActiveInteraction> activeInteractions;

        static lcdgui::LcdHitTestOptions
        hitTestOptionsFor(GestureEvent::InputDeviceType deviceType);
        static std::optional<hardware::ComponentId>
        functionKeyComponentId(int functionKeyIndex);
        static GestureEvent deriveGesture(const GestureEvent &source,
                                          GestureEvent::Type type,
                                          hardware::ComponentId componentId,
                                          GestureEvent::Movement movement,
                                          float continuousDelta = 0.f);
        bool fieldCaptureIsCurrent(const ActiveInteraction &interaction) const;
        static void endInteraction(const ActiveInteraction &interaction,
                                   std::vector<GestureEvent> &derivedGestures);
    };
} // namespace mpc::input
